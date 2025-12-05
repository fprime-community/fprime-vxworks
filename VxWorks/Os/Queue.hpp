// ======================================================================
// \title VxWorks/Os/Queue.hpp
// \brief VxWorks definitions for Os::Queue
// ======================================================================
#ifndef OS_VXWORKS_QUEUE_HPP
#define OS_VXWORKS_QUEUE_HPP
#include <msgQLib.h>
#include "Os/Mutex.hpp"
#include "Os/Queue.hpp"

namespace Os {
namespace VxWorks {
namespace Queue {

struct VxWorksQueueHandle : public QueueHandle {
    MSG_Q_ID m_queue = MSG_Q_ID_NULL;
};

//! \brief VxWorks queue implementation with injectable statuses
//!
//! \note This queue implementation does not implement high watermark.
//! \note This queue implementation is not a true priority queue. Any message with a priority of 1 will be
//! added to the head of the list and any message with a priority of 0 will be added to the tail of the list.
//! \note Messages with a priority of 1 are only supported in ISR context.
class VxWorksQueue : public QueueInterface {
  public:
    //! \brief default queue interface constructor
    VxWorksQueue() = default;

    //! \brief queue destructor
    ~VxWorksQueue() override;

    //! \brief copy constructor is forbidden
    VxWorksQueue(const QueueInterface& other) = delete;

    //! \brief copy constructor is forbidden
    VxWorksQueue(const QueueInterface* other) = delete;

    //! \brief assignment operator is forbidden
    VxWorksQueue& operator=(const QueueInterface& other) override = delete;

    //! \brief create queue storage
    //!
    //! Creates a queue ensuring sufficient storage to hold `depth` messages of `messageSize` size each.
    //! \param name: name of queue
    //! \param depth: depth of queue in number of messages
    //! \param messageSize: size of an individual message
    //! \return: status of the creation
    Status create(FwEnumStoreType id, const Fw::ConstStringBase& name, FwSizeType depth, FwSizeType messageSize) override;

    //! \brief send a message into the queue
    //!
    //! Send a message into the queue, providing the message data, size, priority, and blocking type. When
    //! `blockType` is set to BLOCKING, this call will block on queue full. Otherwise, this will return an error
    //! status on queue full.
    //!
    //! \param buffer: message data
    //! \param size: size of message data
    //! \param priority: priority of the message. Only messages with a priority of 1 or 0 are allowed in ISR context,
    //!                  otherwise only 0 priority is allowed.
    //! \param blockType: BLOCKING to block for space or NONBLOCKING to return error when queue is full
    //! \return: status of the send
    Status send(const U8* buffer, FwSizeType size, FwQueuePriorityType priority, BlockingType blockType) override;

    //! \brief receive a message from the queue
    //!
    //! Receive a message from the queue, providing the message destination, capacity, priority, and blocking type.
    //! When `blockType` is set to BLOCKING, this call will block on queue empty. Otherwise, this will return an
    //! error status on queue empty. Actual size received is set on success status. Priority of message is never set.
    //!
    //! \param destination: destination for message data
    //! \param capacity: maximum size of message data
    //! \param blockType: BLOCKING to wait for message or NONBLOCKING to return error when queue is empty
    //! \param actualSize: (output) actual size of message read
    //! \param priority: (output) This function never set this argument
    //! \return: status of the send
    Status receive(U8* destination,
                   FwSizeType capacity,
                   BlockingType blockType,
                   FwSizeType& actualSize,
                   FwQueuePriorityType& priority) override;

    //! \brief get number of messages available
    //!
    //! \return number of messages available
    FwSizeType getMessagesAvailable() const override;

    //! \brief get maximum messages stored at any given time
    //!
    //! \note this function is not implemented
    //!
    //! \return std::numeric_limits<FwSizeType>::max();
    FwSizeType getMessageHighWaterMark() const override;

    QueueHandle* getHandle() override;

    VxWorksQueueHandle m_handle;
};

}  // namespace Queue
}  // namespace VxWorks
}  // namespace Os

#endif  // OS_VXWORKS_QUEUE_HPP
