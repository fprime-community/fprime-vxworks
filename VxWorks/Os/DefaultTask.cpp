// ======================================================================
// \title VxWorks/Os/DefaultTask.cpp
// \brief sets default Os::Task to VxWorks implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Task.hpp"
#include "VxWorks/Os/Task.hpp"

namespace Os {

TaskInterface* TaskInterface::getDelegate(TaskHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<TaskInterface, Os::VxWorks::Task::VxWorksTask>(aligned_new_memory);
}

}  // namespace Os
