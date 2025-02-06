// ======================================================================
// \title VxWorks/Os/Queue.cpp
// \brief VxWorks implementation for Os::Queue
// ======================================================================
#include "Queue.hpp"

namespace Os {
namespace VxWorks {
namespace Queue {

QueueInterface::Status VxWorksQueue::create(const Fw::StringBase& name, FwSizeType depth, FwSizeType messageSize) {
    return QueueInterface::Status::UNKNOWN_ERROR;
}

QueueInterface::Status VxWorksQueue::send(const U8* buffer,
                                          FwSizeType size,
                                          FwQueuePriorityType priority,
                                          QueueInterface::BlockingType blockType) {
    return QueueInterface::Status::UNINITIALIZED;
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
