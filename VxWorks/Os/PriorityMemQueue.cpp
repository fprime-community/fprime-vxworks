// ======================================================================
// \title  Os/VxWorks/PriorityMemQueue.cpp
// \author B. Duckett
// \brief  cpp file for VxWorks msgQ-based priority queue implementation for Os::Queue
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#include "PriorityMemQueue.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>
#include "Fw/LanguageHelpers.hpp"
#include "Fw/Logger/Logger.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/MemAllocator.hpp"
#include "config/MemoryAllocatorTypeEnumAc.hpp"
#include <vxWorks.h>
#include <msgQLib.h>
#include <semLib.h>
#include <intLib.h>

namespace Os {
namespace VxWorks {

// Arbitrary (but small) limit to prevent infinite loops
// Don't expect a message queue depth to ever exceed three digits
constexpr U32 LOOP_GUARD_LIMIT = 2000;

// Initialize static members
PriorityMemQueue::QueueConfig* PriorityMemQueue::s_configs = nullptr;
FwSizeType PriorityMemQueue::s_numConfigs = 0;
bool PriorityMemQueue::s_requirePrioritySizing = false;
std::atomic<bool>* PriorityMemQueue::s_configsUsed = nullptr;
bool PriorityMemQueue::s_configured = false;

//! \brief Get the bit mask for a priority
//! \param priority: priority to get mask for
//! \return bit mask with the priority bit set
static constexpr U32 priorityBitMask(FwQueuePriorityType priority) {
    return 1U << priority;
}

void PriorityMemQueueHandle::init() {
    // Arrays must be allocated via allocateArrays() before init() is called
    // Initialize all msgQ IDs to null if arrays are allocated
    if (this->m_msgQueues != nullptr && this->m_msgSizes != nullptr && this->m_depths != nullptr) {
        for (FwSizeType i = 0; i < this->m_maxPriorities; ++i) {
            this->m_msgQueues[i] = nullptr;
            this->m_msgSizes[i] = 0;
            this->m_depths[i] = 0;
        }
    }

    // Initialize high water marks to zero if array is allocated
    if (this->m_highWaterMarks != nullptr) {
        for (FwSizeType i = 0; i < this->m_maxPriorities; ++i) {
            this->m_highWaterMarks[i].store(0, std::memory_order_relaxed);
        }
    }

    // Initialize the not-empty semaphore with count 0 (queue starts empty)
    // Delete old semaphore if it exists
    if (this->m_notEmptySem != nullptr) {
        STATUS status = semDelete(this->m_notEmptySem);
        FW_ASSERT(status == OK, status);
    }
    // Create VxWorks counting semaphore with initial count 0
    this->m_notEmptySem = semCCreate(SEM_Q_FIFO, 0);
    FW_ASSERT(this->m_notEmptySem != nullptr, 0);

    // Initialize atomic variables
    this->m_priorityMask.store(1U << Os::VxWorks::Queue::DEFAULT_PRIORITY, std::memory_order_relaxed);
    this->m_numPriorities = 0;
}

bool PriorityMemQueueHandle::allocateArrays(Fw::MemAllocator& allocator, FwEnumStoreType allocatorId) {
    // Store allocator ID for later deallocation
    this->m_allocatorId = allocatorId;
    
    // Set max priorities to support
    this->m_maxPriorities = Os::VxWorks::Queue::MAX_PRIORITIES;
    
    // Allocate memory for msgQueues array
    FwSizeType msgQueuesSize = sizeof(MSG_Q_ID) * this->m_maxPriorities;
    void* msgQueuesMem = allocator.checkedAllocate(allocatorId, msgQueuesSize, alignof(MSG_Q_ID));
    if (msgQueuesMem == nullptr) {
        return false;
    }
    // Use placement new to construct array
    this->m_msgQueues = new (msgQueuesMem) MSG_Q_ID[this->m_maxPriorities];
    
    // Allocate memory for msgSizes array
    FwSizeType msgSizesSize = sizeof(FwSizeType) * this->m_maxPriorities;
    void* msgSizesMem = allocator.checkedAllocate(allocatorId, msgSizesSize, alignof(FwSizeType));
    if (msgSizesMem == nullptr) {
        allocator.deallocate(allocatorId, this->m_msgQueues);
        this->m_msgQueues = nullptr;
        return false;
    }
    // Use placement new to construct array
    this->m_msgSizes = new (msgSizesMem) FwSizeType[this->m_maxPriorities];
    
    // Allocate memory for depths array
    FwSizeType depthsSize = sizeof(FwSizeType) * this->m_maxPriorities;
    void* depthsMem = allocator.checkedAllocate(allocatorId, depthsSize, alignof(FwSizeType));
    if (depthsMem == nullptr) {
        allocator.deallocate(allocatorId, this->m_msgQueues);
        allocator.deallocate(allocatorId, this->m_msgSizes);
        this->m_msgQueues = nullptr;
        this->m_msgSizes = nullptr;
        return false;
    }
    // Use placement new to construct array
    this->m_depths = new (depthsMem) FwSizeType[this->m_maxPriorities];
    
    // Allocate memory for highWaterMarks array
    FwSizeType hwmSize = sizeof(std::atomic<U32>) * this->m_maxPriorities;
    void* hwmMem = allocator.checkedAllocate(allocatorId, hwmSize, alignof(std::atomic<U32>));
    if (hwmMem == nullptr) {
        allocator.deallocate(allocatorId, this->m_msgQueues);
        allocator.deallocate(allocatorId, this->m_msgSizes);
        allocator.deallocate(allocatorId, this->m_depths);
        this->m_msgQueues = nullptr;
        this->m_msgSizes = nullptr;
        this->m_depths = nullptr;
        return false;
    }
    // Use placement new to construct array of atomics
    this->m_highWaterMarks = reinterpret_cast<std::atomic<U32>*>(hwmMem);
    for (FwSizeType i = 0; i < this->m_maxPriorities; ++i) {
        new (&this->m_highWaterMarks[i]) std::atomic<U32>(0);
    }
    
    return true;
}

void PriorityMemQueueHandle::deallocateArrays(Fw::MemAllocator& allocator, FwEnumStoreType allocatorId) {
    // Deallocate arrays in reverse order using stored allocator ID
    if (this->m_highWaterMarks != nullptr) {
        // Explicitly destroy atomics before deallocation
        for (FwSizeType i = 0; i < this->m_maxPriorities; ++i) {
            this->m_highWaterMarks[i].~atomic();
        }
        allocator.deallocate(allocatorId, this->m_highWaterMarks);
        this->m_highWaterMarks = nullptr;
    }
    if (this->m_depths != nullptr) {
        allocator.deallocate(allocatorId, this->m_depths);
        this->m_depths = nullptr;
    }
    if (this->m_msgSizes != nullptr) {
        allocator.deallocate(allocatorId, this->m_msgSizes);
        this->m_msgSizes = nullptr;
    }
    if (this->m_msgQueues != nullptr) {
        allocator.deallocate(allocatorId, this->m_msgQueues);
        this->m_msgQueues = nullptr;
    }
    this->m_maxPriorities = 0;
    this->m_allocatorId = 0;
}

void PriorityMemQueueHandle::enablePriority(FwQueuePriorityType priority) {
    FW_ASSERT(priority < Os::VxWorks::Queue::MAX_PRIORITIES, this->m_id);

    // MEMORY ORDERING: seq_cst for control path operations ensures total ordering
    // Atomic update of priority mask using fetch_or with seq_cst (control path)
    this->m_priorityMask.fetch_or(priorityBitMask(priority), std::memory_order_seq_cst);
}

void PriorityMemQueueHandle::disablePriority(FwQueuePriorityType priority) {
    FW_ASSERT(priority < Os::VxWorks::Queue::MAX_PRIORITIES, this->m_id);

    // MEMORY ORDERING: seq_cst for control path operations ensures total ordering
    // Atomic update of priority mask using fetch_and with seq_cst (control path)
    this->m_priorityMask.fetch_and(~priorityBitMask(priority), std::memory_order_seq_cst);
}

PriorityMemQueue::PriorityMemQueue() {
    // Initialize handle to safe defaults
    this->m_handle.init();
}

//! \brief Find most significant bit set (IPC-style priority finding)
//! \param value: bit mask to search
//! \return bit position of MSB, or -1 if no bits set
static inline I32 findMSB(U32 value) {
    if (value == 0) {
        return -1;
    }
    // Use compiler builtin for CLZ (count leading zeros) if available
#if defined(__GNUC__) || defined(__clang__)
    return 31 - __builtin_clz(value);
#else
    // Fallback: software implementation
    I32 msb = 31;
    U32 mask = 0x80000000;
    while (mask && !(mask & value)) {
        --msb;
        mask >>= 1;
    }
    return msb;
#endif
}

FwQueuePriorityType PriorityMemQueue::findHighestPriority(U32 priorities) {
    // MEMORY ORDERING: Use acquire to synchronize with priority enable/disable operations
    // Get enabled priorities
    if (priorities == 0) {
        priorities = this->m_handle.m_priorityMask.load(std::memory_order_acquire);
    }

    if (priorities == 0) {
        return Os::VxWorks::Queue::MAX_PRIORITIES;
    }

    // Use IPC-style MSB finding for performance
    I32 msb = findMSB(priorities);
    if (msb < 0 || msb >= static_cast<I32>(Os::VxWorks::Queue::MAX_PRIORITIES)) {
        return Os::VxWorks::Queue::MAX_PRIORITIES;
    }
    return static_cast<FwQueuePriorityType>(msb);
}

bool PriorityMemQueue::isPriorityEnabled(FwQueuePriorityType priority) {
    FW_ASSERT(priority < Os::VxWorks::Queue::MAX_PRIORITIES, this->m_handle.m_id);

    return (this->m_handle.m_priorityMask.load(std::memory_order_seq_cst) & priorityBitMask(priority)) != 0;
}

void PriorityMemQueue::setPriorityEnabled(FwQueuePriorityType priority, bool enabled) {
    FW_ASSERT(priority < Os::VxWorks::Queue::MAX_PRIORITIES, this->m_handle.m_id);

    // MEMORY ORDERING: seq_cst for control path, acquire for data path (see receive())
    // Atomic update of priority mask using fetch_or/fetch_and with seq_cst (control path)
    if (enabled) {
        this->m_handle.m_priorityMask.fetch_or(priorityBitMask(priority), std::memory_order_seq_cst);
    } else {
        this->m_handle.m_priorityMask.fetch_and(~priorityBitMask(priority), std::memory_order_seq_cst);
    }
}

Fw::MemAllocator& PriorityMemQueue::getAllocator() {
    return Fw::MemAllocatorRegistry::getInstance().getAnAllocator(
        Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);
}

void PriorityMemQueue::configure(QueueConfig* queueConfigs, FwSizeType numQueueConfigs, bool required, FwEnumStoreType allocatorId) {
    // Accept NULL pointer if and only if numQueueConfigs is 0
    FW_ASSERT((queueConfigs != nullptr) || (numQueueConfigs == 0), 0);

    // Assert if already configured - that's not a supported use case
    FW_ASSERT(!s_configured, 0);

    s_configured = true;

    // Assert if required is true but num priorities is 0
    FW_ASSERT(!(required && numQueueConfigs == 0), required, static_cast<FwAssertArgType>(numQueueConfigs));

    // Check for duplicate instance IDs and invalid configurations
    if (queueConfigs != nullptr) {
        for (FwSizeType i = 0; i < numQueueConfigs; ++i) {
            QueueConfig* currentConfig = &queueConfigs[i];

            // Assert if numPriorities is 0
            FW_ASSERT(currentConfig->numPriorities > 0, static_cast<FwAssertArgType>(i), currentConfig->instanceId);

            // Check for duplicate instance IDs
            for (FwSizeType j = i + 1; j < numQueueConfigs; ++j) {
                QueueConfig* otherConfig = &queueConfigs[j];
                FW_ASSERT(currentConfig->instanceId != otherConfig->instanceId, currentConfig->instanceId,
                          static_cast<FwAssertArgType>(i), static_cast<FwAssertArgType>(j));
            }

            // Check priority configurations
            QueuePriorityConfig* priorityConfigs = currentConfig->priorityConfigs;
            for (FwSizeType p = 0; p < currentConfig->numPriorities; ++p) {
                QueuePriorityConfig* pConfig = &priorityConfigs[p];

                // Assert if maxMsgSize or numMsgs is 0
                FW_ASSERT(pConfig->maxMsgSize > 0, static_cast<FwAssertArgType>(i), static_cast<FwAssertArgType>(p),
                          pConfig->priority);
                FW_ASSERT(pConfig->numMsgs > 0, static_cast<FwAssertArgType>(i), static_cast<FwAssertArgType>(p),
                          pConfig->priority);
                FW_ASSERT(pConfig->priority >= 0 && pConfig->priority < Os::VxWorks::Queue::MAX_PRIORITIES,
                          static_cast<FwAssertArgType>(i), static_cast<FwAssertArgType>(p), pConfig->priority);

                // Check for duplicate priority values
                for (FwSizeType q = p + 1; q < currentConfig->numPriorities; ++q) {
                    QueuePriorityConfig* qConfig = &priorityConfigs[q];
                    FW_ASSERT(pConfig->priority != qConfig->priority, static_cast<FwAssertArgType>(i),
                              currentConfig->instanceId, pConfig->priority);
                }
            }
        }
    }
    // Get the memory allocator configured for priority queues
    // NOTE: This dynamic memory is intentionally never freed in production because configure()
    //       is called once and only once during system initialization (enforced by s_configured check).
    //       In test environments, test harnesses should explicitly call destructors before deallocating
    //       to prevent memory leaks (see PriorityMemQueueTestHelper::resetConfig() for example).
    // Note: This is a static method so we can't use getAllocator() instance method
    // Note: Initialization is single threaded, so using atomics here is _probably_ overkill 
    Fw::MemAllocator& allocator = Fw::MemAllocatorRegistry::getInstance().getAnAllocator(
        Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);
    // Allocate memory for tracking used configurations
    if (numQueueConfigs > 0) {
        FwSizeType expSize = numQueueConfigs * sizeof(std::atomic<bool>);
        s_configsUsed = reinterpret_cast<std::atomic<bool>*>(
            allocator.checkedAllocate(allocatorId, expSize, alignof(std::atomic<bool>)));
        FW_ASSERT(s_configsUsed != nullptr);
        // Initialize all entries to false using placement new
        for (FwSizeType i = 0; i < numQueueConfigs; ++i) {
            new (&s_configsUsed[i]) std::atomic<bool>(false);
        }
    }

    // Store configuration
    s_configs = queueConfigs;
    s_numConfigs = numQueueConfigs;
    s_requirePrioritySizing = required;
}

void PriorityMemQueue::resetConfig() {
    // Only call this in test environments after all queues are destroyed
    if (s_configsUsed != nullptr) {
        // Get allocator (same as used in config())
        Fw::MemAllocator& allocator = Fw::MemAllocatorRegistry::getInstance().getAnAllocator(
            Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);
        FwEnumStoreType allocatorId = 0;
        
        // Deallocate the tracking array
        allocator.deallocate(allocatorId, s_configsUsed);
        s_configsUsed = nullptr;
    }
    
    // Reset all static state
    s_configs = nullptr;
    s_numConfigs = 0;
    s_requirePrioritySizing = false;
    s_configured = false;
}

PriorityMemQueue::~PriorityMemQueue() {
    this->teardownInternal();
}

QueueInterface::Status PriorityMemQueue::create(FwEnumStoreType id,
                                                const Fw::ConstStringBase& name,
                                                FwSizeType depth,
                                                FwSizeType messageSize) {
    FW_ASSERT(depth > 0, id);
    FW_ASSERT(messageSize > 0, id);

    // Initialize the handle ID
    this->m_handle.m_id = id;

    // Get the memory allocator for queue operations
    Fw::MemAllocator& allocator = this->getAllocator();
    
    // Allocate arrays for priority data
    if (!this->m_handle.allocateArrays(allocator, id)) {
        return Os::QueueInterface::Status::ALLOCATION_FAILED;
    }
    
    // Initialize the handle with allocated arrays
    this->m_handle.init();

    // Find a matching configuration if one exists
    QueueConfig* queueConfig = findMatchingConfig(id);

    // Create the priority queues based on configuration
    if (queueConfig != nullptr) {
        // Use the found configuration to create multiple priority queues
        return createConfiguredQueues(queueConfig, allocator, id);
    } else {
        // No configuration found, create a single default priority queue
        return createDefaultQueue(depth, messageSize, allocator, id);
    }
}

// Helper method to find a matching configuration for the given ID
PriorityMemQueue::QueueConfig* PriorityMemQueue::findMatchingConfig(FwEnumStoreType id) {
    
    if (s_configs != nullptr && s_configsUsed != nullptr) {
        
        for (FwSizeType i = 0; i < s_numConfigs; ++i) {
            if (s_configs[i].instanceId == id) {
                // Atomic check-and-set to claim configuration
                bool expected = false;
                if (s_configsUsed[i].compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
                    return &s_configs[i];
                } else {
                    // Configuration already in use, assert failure
                    FW_ASSERT(false, id, s_configs[i].instanceId);
                }
            }
        }
    }
    return nullptr;
}

// Helper method to create queues based on configuration
QueueInterface::Status PriorityMemQueue::createConfiguredQueues(QueueConfig* queueConfig,
                                                                Fw::MemAllocator& allocator,
                                                                FwEnumStoreType allocatorId) {
    FW_ASSERT(queueConfig != nullptr, this->m_handle.m_id);
    FW_ASSERT(queueConfig->priorityConfigs != nullptr, this->m_handle.m_id, queueConfig->numPriorities);
    
    this->m_handle.m_numPriorities = static_cast<U32>(queueConfig->numPriorities);

    for (FwSizeType i = 0; i < queueConfig->numPriorities; ++i) {
        const QueuePriorityConfig& priorityConfig = queueConfig->priorityConfigs[i];
        FwQueuePriorityType priority = priorityConfig.priority;

        // Create and initialize the priority queue
        QueueInterface::Status status =
            this->createPriorityQueue(priority, priorityConfig.maxMsgSize, priorityConfig.numMsgs, allocator, allocatorId);

        if (status != Os::QueueInterface::Status::OP_OK) {
            return status;
        }

        // Enable this priority
        this->setPriorityEnabled(priority, true);
    }

    return Os::QueueInterface::Status::OP_OK;
}

// Helper method to create a single default priority queue
QueueInterface::Status PriorityMemQueue::createDefaultQueue(FwSizeType depth,
                                                            FwSizeType messageSize,
                                                            Fw::MemAllocator& allocator,
                                                            FwEnumStoreType allocatorId) {
    this->m_handle.m_numPriorities = 1;

    // Create and initialize the default priority queue
    return this->createPriorityQueue(Os::VxWorks::Queue::DEFAULT_PRIORITY, messageSize, depth, allocator, allocatorId);
}

// Helper method to create a single priority queue using VxWorks msgQ
QueueInterface::Status PriorityMemQueue::createPriorityQueue(FwQueuePriorityType priority,
                                                             FwSizeType maxMsgSize,
                                                             FwSizeType numMsgs,
                                                             Fw::MemAllocator& allocator,
                                                             FwEnumStoreType allocatorId) {
    FW_ASSERT(this->m_handle.m_msgQueues != nullptr, this->m_handle.m_id, priority);
    FW_ASSERT(this->m_handle.m_msgSizes != nullptr, this->m_handle.m_id, priority);
    FW_ASSERT(this->m_handle.m_depths != nullptr, this->m_handle.m_id, priority);
    FW_ASSERT(priority < Os::VxWorks::Queue::MAX_PRIORITIES, this->m_handle.m_id, priority);
    
    // Create VxWorks message queue
    // MSG_Q_FIFO ensures FIFO ordering within this priority level
    MSG_Q_ID msgQ = msgQCreate(static_cast<int>(numMsgs), static_cast<int>(maxMsgSize), MSG_Q_FIFO);
    
    if (msgQ == nullptr) {
        Fw::Logger::log("ERROR: msgQCreate failed for priority %d\n", priority);
        this->teardownInternal();
        return Os::QueueInterface::Status::ALLOCATION_FAILED;
    }
    
    // Store the msgQ handle and configuration
    this->m_handle.m_msgQueues[priority] = msgQ;
    this->m_handle.m_msgSizes[priority] = maxMsgSize;
    this->m_handle.m_depths[priority] = numMsgs;

    return Os::QueueInterface::Status::OP_OK;
}

void PriorityMemQueue::teardown() {
    this->teardownInternal();
}

void PriorityMemQueue::teardownInternal() {
    // NOTE: This function is intended for use in unit tests or single-threaded teardown.
    //       Thread safety is not guaranteed for concurrent teardown operations.
    
    // Delete all VxWorks message queues if arrays are allocated
    if (this->m_handle.m_msgQueues != nullptr) {
        for (FwSizeType i = 0; i < this->m_handle.m_maxPriorities; ++i) {
            MSG_Q_ID msgQ = this->m_handle.m_msgQueues[i];
            if (msgQ != nullptr) {
                STATUS status = msgQDelete(msgQ);
                if (status != OK) {
                    Fw::Logger::log("ERROR: msgQDelete failed for priority %lu\n", static_cast<unsigned long>(i));
                }
                this->m_handle.m_msgQueues[i] = nullptr;
                this->m_handle.m_msgSizes[i] = 0;
                this->m_handle.m_depths[i] = 0;
            }
        }
    }

    // Delete the not-empty semaphore
    if (this->m_handle.m_notEmptySem != nullptr) {
        STATUS status = semDelete(this->m_handle.m_notEmptySem);
        if (status != OK) {
            Fw::Logger::log("ERROR: semDelete failed for not-empty semaphore\n");
        }
        this->m_handle.m_notEmptySem = nullptr;
    }

    // Reset handle state
    this->m_handle.m_numPriorities = 0;
    this->m_handle.m_priorityMask.store(1U << Os::VxWorks::Queue::DEFAULT_PRIORITY, std::memory_order_relaxed);

    // Deallocate arrays using stored allocator ID
    Fw::MemAllocator& allocator = this->getAllocator();
    this->m_handle.deallocateArrays(allocator, this->m_handle.m_allocatorId);

    // If we were using a configuration, mark it as unused
    // NOTE: This release operation is not thread-safe by design (teardown is UT-only)
    FW_ASSERT(s_configs != nullptr || s_configsUsed == nullptr, this->m_handle.m_id);
    FW_ASSERT(s_configsUsed != nullptr || s_configs == nullptr, this->m_handle.m_id);
    
    if (s_configs != nullptr && s_configsUsed != nullptr) {
        for (FwSizeType i = 0; i < s_numConfigs; ++i) {
            if (s_configs[i].instanceId == this->m_handle.m_id && s_configsUsed[i].load()) {
                s_configsUsed[i].store(false);
                break;
            }
        }
    }
}

QueueInterface::Status PriorityMemQueue::send(const U8* buffer,
                                              FwSizeType size,
                                              FwQueuePriorityType priority,
                                              QueueInterface::BlockingType blockType) {
    // Validate input parameters
    FW_ASSERT(buffer != nullptr, this->m_handle.m_id, priority, blockType);
    FW_ASSERT(size > 0, this->m_handle.m_id, size, priority);

    // Check if priority is valid
    if (priority >= Os::VxWorks::Queue::MAX_PRIORITIES) {
        return QueueInterface::Status::INVALID_PRIORITY;
    }

    // Check if the queue is initialized
    if (this->m_handle.m_numPriorities == 0 || this->m_handle.m_msgQueues == nullptr) {
        return QueueInterface::Status::UNINITIALIZED;
    }

    // Check if the priority queue exists - no fallback, fail fast
    MSG_Q_ID msgQ = this->m_handle.m_msgQueues[priority];
    if (msgQ == nullptr) {
        if (s_requirePrioritySizing) {
            FW_ASSERT(0, this->m_handle.m_id, s_requirePrioritySizing, priority);
        }
        Fw::Logger::log("ERROR: Priority %d not found for %u, using default priority (%lu)\n", priority,
                        this->m_handle.m_id, Os::VxWorks::Queue::DEFAULT_PRIORITY);
        priority = Os::VxWorks::Queue::DEFAULT_PRIORITY;
        msgQ = this->m_handle.m_msgQueues[priority];
    }
    FW_ASSERT(msgQ != nullptr, this->m_handle.m_id, priority, this->m_handle.m_numPriorities);

    // Check for sizing problem
    if (size > this->m_handle.m_msgSizes[priority]) {
        return QueueInterface::Status::SIZE_MISMATCH;
    }

    // Send message using VxWorks msgQSend
    // msgQSend is ISR-safe and SMP-safe
    // NOTE: const_cast is required because VxWorks msgQSend API signature uses non-const char*
    //       even though it does not modify the buffer. This is a legacy C API limitation.
    int timeout = (blockType == QueueInterface::BlockingType::BLOCKING) ? WAIT_FOREVER : NO_WAIT;
    STATUS status = msgQSend(msgQ, reinterpret_cast<char*>(const_cast<U8*>(buffer)), 
                            static_cast<UINT>(size), timeout, MSG_PRI_NORMAL);
    
    if (status != OK) {
        return (errno == S_objLib_OBJ_UNAVAILABLE) ? QueueInterface::Status::FULL : QueueInterface::Status::UNKNOWN_ERROR;
    }

    // Update per-priority high water mark
    FW_ASSERT(this->m_handle.m_highWaterMarks != nullptr, this->m_handle.m_id, priority);
    
    int numMsgs = msgQNumMsgs(msgQ);
    U32 currentDepth = static_cast<U32>(numMsgs);
    U32 prevMax = this->m_handle.m_highWaterMarks[priority].load(std::memory_order_acquire);
    
    // Bounded retry loop for CAS operation (max 100 attempts)
    constexpr U32 MAX_CAS_RETRIES = 100;
    U32 casRetries = 0;
    while (currentDepth > prevMax && casRetries < MAX_CAS_RETRIES) {
        if (this->m_handle.m_highWaterMarks[priority].compare_exchange_weak(prevMax, currentDepth, 
                                                                            std::memory_order_release,
                                                                            std::memory_order_acquire)) {
            break;
        }
        ++casRetries;
    }
    FW_ASSERT(casRetries < MAX_CAS_RETRIES, this->m_handle.m_id, priority, casRetries);
    
    // Post semaphore to wake up receiver (if any)
    FW_ASSERT(this->m_handle.m_notEmptySem != nullptr, this->m_handle.m_id, priority);
    STATUS semStatus = semGive(this->m_handle.m_notEmptySem);
    FW_ASSERT(semStatus == OK, semStatus);
    
    return QueueInterface::Status::OP_OK;
}

QueueInterface::Status PriorityMemQueue::receive(U8* destination,
                                                 FwSizeType capacity,
                                                 QueueInterface::BlockingType blockType,
                                                 FwSizeType& actualSize,
                                                 FwQueuePriorityType& priority) {
    // Validate input parameters
    FW_ASSERT(destination != nullptr, blockType, this->m_handle.m_id);

    // Check if the queue is initialized
    if (this->m_handle.m_numPriorities == 0 || this->m_handle.m_msgQueues == nullptr) {
        return QueueInterface::Status::UNINITIALIZED;
    }

    // Check if the blocking type is valid
    FW_ASSERT(
        blockType == QueueInterface::BlockingType::BLOCKING || blockType == QueueInterface::BlockingType::NONBLOCKING,
        blockType, this->m_handle.m_id);

    // RECEIVE FLOW: Scan all enabled priorities from highest to lowest
    // Loop:
    //   1. Get enabled priority mask
    //   2. Scan from highest to lowest priority
    //   3. Try msgQReceive NO_WAIT on each enabled priority until message found
    //   4. If no message: wait on semaphore (blocking) or return empty (non-blocking)
    //
    // DESIGN: No state tracking - directly check VxWorks queues.
    // Semaphore count may desync (multiple receivers wake on single message).
    // Result: One gets message, others find all queues empty, block again.
    //
    // MEMORY ORDERING: Use acquire on priority mask to ensure visibility of queue state.
    
    // Bounded loop with compile-time limit for JPL Power of Ten compliance
    for (U32 reps = 0; reps < LOOP_GUARD_LIMIT; ++reps) {
        // Get enabled priorities
        U32 enabledPriorities = this->m_handle.m_priorityMask.load(std::memory_order_acquire);
        
        // Scan from highest to lowest priority
        for (I32 p = Os::VxWorks::Queue::MAX_PRIORITIES - 1; p >= 0; --p) {
            FwQueuePriorityType testPriority = static_cast<FwQueuePriorityType>(p);
            
            // Skip if priority not enabled
            if ((enabledPriorities & priorityBitMask(testPriority)) == 0) {
                continue;
            }
            
            FW_ASSERT(this->m_handle.m_msgQueues != nullptr, this->m_handle.m_id, testPriority);
            MSG_Q_ID msgQ = this->m_handle.m_msgQueues[testPriority];
            if (msgQ == nullptr) {
                continue;
            }
            
            // Try to receive with NO_WAIT (non-blocking within this attempt)
            int bytesRead = msgQReceive(msgQ, reinterpret_cast<char*>(destination), 
                                      static_cast<UINT>(capacity), NO_WAIT);
            
            if (bytesRead > 0) {
                actualSize = static_cast<FwSizeType>(bytesRead);
                priority = testPriority;
                
                return QueueInterface::Status::OP_OK;
            }
        }
        
        // No message available
        if (blockType == QueueInterface::BlockingType::BLOCKING) {
            // Wait for a message
            FW_ASSERT(this->m_handle.m_notEmptySem != nullptr, this->m_handle.m_id);
            STATUS semStatus = semTake(this->m_handle.m_notEmptySem, WAIT_FOREVER);
            FW_ASSERT(semStatus == OK, semStatus);
        } else {
            // Non-blocking, return empty
            return QueueInterface::Status::EMPTY;
        }
    }
    
    // Should never reach here - loop guard prevents infinite loop
    FW_ASSERT(false, this->m_handle.m_id, LOOP_GUARD_LIMIT);
    return QueueInterface::Status::UNKNOWN_ERROR;
}

FwSizeType PriorityMemQueue::getMessagesAvailable() const {
    FwSizeType total = 0;

    if (this->m_handle.m_msgQueues != nullptr) {
        for (FwSizeType i = 0; i < this->m_handle.m_maxPriorities; ++i) {
            MSG_Q_ID msgQ = this->m_handle.m_msgQueues[i];
            if (msgQ != nullptr) {
                int numMsgs = msgQNumMsgs(msgQ);
                total += static_cast<FwSizeType>(numMsgs);
            }
        }
    }

    return total;
}

FwSizeType PriorityMemQueue::getMessageHighWaterMark() const {
    // Return the maximum high water mark across all priorities
    // MEMORY ORDERING: Use acquire to ensure visibility of latest HWM updates
    U32 maxHwm = 0;
    if (this->m_handle.m_highWaterMarks != nullptr) {
        for (FwSizeType i = 0; i < this->m_handle.m_maxPriorities; ++i) {
            U32 hwm = this->m_handle.m_highWaterMarks[i].load(std::memory_order_acquire);
            if (hwm > maxHwm) {
                maxHwm = hwm;
            }
        }
    }
    return static_cast<FwSizeType>(maxHwm);
}

QueueHandle* PriorityMemQueue::getHandle() {
    return &this->m_handle;
}

}  // namespace VxWorks
}  // namespace Os
