// ======================================================================
// \title VxWorks/Os/Queue.cpp
// \brief VxWorks implementation for Os::Queue
// ======================================================================
#include "Queue.hpp"
#include <intLib.h>
#include <limits.h>
#include <Fw/Types/Assert.hpp>

namespace Os {
namespace VxWorks {
namespace Queue {

VxWorksQueue::~VxWorksQueue() {
    (void)msgQDelete(this->m_handle.m_queue);
}

QueueInterface::Status VxWorksQueue::create(const Fw::ConstStringBase& name, FwSizeType depth, FwSizeType messageSize) {
    this->m_handle.m_queue = msgQCreate(static_cast<size_t>(depth), static_cast<size_t>(messageSize), MSG_Q_PRIORITY);
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

    // It is illegal to block while in interrupt context
    FW_ASSERT(!(intContext() && blockType == QueueInterface::BlockingType::BLOCKING));

    // VxWorks queues do not support true priority.  Priority of 1 is only allowed in cases where an ISR needs to
    // be shuttled to the front of the line. This is because prioritized messages in VxWorks are in LIFO order and only
    // support a boolean priority, which violates key expectations of Os::Queue. This is allowed in interrupt context as
    // interrupts typically need to be processed with high priority.
    if (!((intContext() && priority < 2) || priority == 0)) {
        return QueueInterface::Status::NOT_SUPPORTED;
    }

    int vxPrio = (priority > 0) ? MSG_PRI_URGENT : MSG_PRI_NORMAL;

    // Casting buffer to match API
    STATUS stat =
        msgQSend(this->m_handle.m_queue, reinterpret_cast<char*>(const_cast<U8*>(buffer)), static_cast<size_t>(size),
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
    actualSize =
        msgQReceive(this->m_handle.m_queue, reinterpret_cast<char*>(destination), static_cast<size_t>(capacity),
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
    return std::numeric_limits<FwSizeType>::max();
}

QueueHandle* VxWorksQueue::getHandle() {
    return &this->m_handle;
}

}  // namespace Queue
}  // namespace VxWorks
}  // namespace Os
