// ======================================================================
// \title VxWorks/Os/DefaultTask.cpp
// \brief sets default Os::Task to vxworks implementation via linker
// ======================================================================
#include <cerrno>
#include "Os/Task.hpp"
#include "VxWorks/Os/Task.hpp"
#include "Os/Delegate.hpp"

namespace Os {
    Os::Task::Status TaskInterface::delay(Fw::Time interval) {


        // Lookup the number of clock ticks for the specified
        // number of milliseconds. Use the ticks to call the
        // task delay function call.
        Os::Task::Status status = Os::Task::Status::DELAY_ERROR;

        U32 milliseconds = interval.getUSeconds()/1000 + interval.getSeconds()*1000;
        
        U32 ticks = ((milliseconds * sysClkRateGet() + 999) / 1000);
        
        if (taskDelay(ticks) == VXWORKS_OK) {
            status = Os::Task::Status::OP_OK;
        }
        return status;

    }

    TaskInterface* TaskInterface::getDelegate(HandleStorage& aligned_new_memory) {
        return Os::Delegate::makeDelegate<TaskInterface, Os::VxWorks::Task::VxWorksTask>(aligned_new_memory);
    }

}
