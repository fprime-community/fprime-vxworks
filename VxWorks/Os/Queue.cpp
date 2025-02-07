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
    return QueueInterface::Status::UNKNOWN_ERROR;
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

    return QueueInterface::Status::OP_OK;
}

QueueInterface::Status VxWorksQueue::receive(U8* destination,
                                             FwSizeType capacity,
                                             QueueInterface::BlockingType blockType,
                                             FwSizeType& actualSize,
                                             FwQueuePriorityType& priority) {
    return QueueInterface::Status::UNINITIALIZED;
}

FwSizeType VxWorksQueue::getMessagesAvailable() const {
    return 0;
}

FwSizeType VxWorksQueue::getMessageHighWaterMark() const {
    return 0;
}

QueueHandle* VxWorksQueue::getHandle() {
    return &this->m_handle;
}

}  // namespace Queue
}  // namespace VxWorks
}  // namespace Os
