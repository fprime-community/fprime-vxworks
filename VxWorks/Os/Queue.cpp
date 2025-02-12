// ======================================================================
// \title VxWorks/Os/Queue.cpp
// \brief VxWorks implementation for Os::Queue
// ======================================================================
#include "Queue.hpp"
#include <Fw/Types/Assert.hpp>

namespace Os {
namespace VxWorks {
namespace Queue {

VxWorksQueue::~VxWorksQueue() {
    (void)msgQDelete(this->m_handle.m_queue);
}

QueueInterface::Status VxWorksQueue::create(const Fw::StringBase& name, FwSizeType depth, FwSizeType messageSize) {
    this->m_handle.m_queue = msgQCreate(depth, messageSize, MSG_Q_PRIORITY);
    if (this->m_handle.m_queue == MSG_Q_ID_NULL) {
        return QueueInterface::Status::UNINITIALIZED;
    }
    return QueueInterface::Status::OP_OK;
}

QueueInterface::Status VxWorksQueue::send(const U8* buffer,
                                          FwSizeType size,
                                          FwQueuePriorityType priority,
                                          QueueInterface::BlockingType blockType) {
    FW_ASSERT(buffer != nullptr);
    if (this->m_handle.m_queue == MSG_Q_ID_NULL) {
        return QueueInterface::Status::UNINITIALIZED;
    }

    PlatformIntType vxPrio = (priority > 0) ? MSG_PRI_URGENT : MSG_PRI_NORMAL;

    // Doing a c-style cast here because msgQSend requires a char* and this is more
    // efficient than copying from a const U8 buffer to a non-const char* buffer.
    STATUS stat = msgQSend(this->m_handle.m_queue, (char*)buffer, size,
                           (QueueInterface::BlockingType::NONBLOCKING == blockType) ? NO_WAIT : WAIT_FOREVER, vxPrio);

    if (stat == VXWORKS_ERROR) {
        switch (errno) {
            case S_msgQLib_INVALID_MSG_LENGTH:
                return QueueInterface::Status::SIZE_MISMATCH;
            case S_objLib_OBJ_UNAVAILABLE:
            case S_msgQLib_NON_ZERO_TIMEOUT_AT_INT_LEVEL:
                return QueueInterface::Status::FULL;
            default:
                return QueueInterface::Status::UNKNOWN_ERROR;
        }
    }

#ifdef ENABLE_HIGH_WATERMARK
    // Protect critical data m_highMark
    {
        Os::ScopeLock lock(const_cast<Mutex&>(this->m_handle.m_data_lock));
        this->m_handle.m_highMark = FW_MAX(this->m_handle.m_highMark, this->getMessagesAvailable());
    }
#endif
    return QueueInterface::Status::OP_OK;
}

QueueInterface::Status VxWorksQueue::receive(U8* destination,
                                             FwSizeType capacity,
                                             QueueInterface::BlockingType blockType,
                                             FwSizeType& actualSize,
                                             FwQueuePriorityType& priority) {
    FW_ASSERT(destination != nullptr);
    if (this->m_handle.m_queue == MSG_Q_ID_NULL) {
        return QueueInterface::Status::UNINITIALIZED;
    }

    // Casting destination to match API
    actualSize = msgQReceive(this->m_handle.m_queue, reinterpret_cast<char*>(destination), capacity,
                             (QueueInterface::BlockingType::NONBLOCKING == blockType) ? NO_WAIT : WAIT_FOREVER);

    if (actualSize == VXWORKS_ERROR) {
        actualSize = 0;
        switch (errno) {
            case S_msgQLib_INVALID_MSG_LENGTH:
                return QueueInterface::Status::SIZE_MISMATCH;
            case S_objLib_OBJ_UNAVAILABLE:
                return QueueInterface::Status::EMPTY;
            default:
                return QueueInterface::Status::UNKNOWN_ERROR;
        }
    }
    return QueueInterface::Status::OP_OK;
}

FwSizeType VxWorksQueue::getMessagesAvailable() const {
    FW_ASSERT(this->m_handle.m_queue != MSG_Q_ID_NULL);
    return msgQNumMsgs(this->m_handle.m_queue);
}

FwSizeType VxWorksQueue::getMessageHighWaterMark() const {
#ifdef ENABLE_HIGH_WATERMARK
    // Safe to cast away const in this context because scope lock will restore unlocked state on return
    Os::ScopeLock lock(const_cast<Mutex&>(this->m_handle.m_data_lock));
    return this->m_handle.m_highMark;
#else
    return 0;
#endif
}

QueueHandle* VxWorksQueue::getHandle() {
    return &this->m_handle;
}

}  // namespace Queue
}  // namespace VxWorks
}  // namespace Os
