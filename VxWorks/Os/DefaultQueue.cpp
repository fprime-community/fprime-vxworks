// ======================================================================
// \title VxWorks/Os/DefaultQueue.cpp
// \brief sets default Os::Queue to VxWorks implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Queue.hpp"
#include "VxWorks/Os/Queue.hpp"

namespace Os {
QueueInterface* QueueInterface::getDelegate(QueueHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<QueueInterface, Os::VxWorks::Queue::VxWorksQueue>(aligned_new_memory);
}
}  // namespace Os
