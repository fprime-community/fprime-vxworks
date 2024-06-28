// ======================================================================
// \title VxWorks/Os/DefaultTask.cpp
// \brief sets default Os::Task to vxworks implementation via linker
// ======================================================================
#include <cerrno>
#include "Os/Task.hpp"
#include "VxWorks/Os/VxWorks7/Task.hpp"
#include "Os/Delegate.hpp"

namespace Os {
    Os::Task::Status TaskInterface::delay(Fw::Time interval) {
        Os::Task::Status task_status = Os::Task::OP_OK;
        timespec sleep_interval;
        sleep_interval.tv_sec = interval.getSeconds();
        sleep_interval.tv_nsec = interval.getUSeconds() * 1000;

        timespec remaining_interval;
        remaining_interval.tv_sec = 0;
        remaining_interval.tv_nsec = 0;

        while (true) {
            PlatformIntType status = nanosleep(&sleep_interval, &remaining_interval);
            // Success, return ok
            if (0 == status) {
                break;
            }
            // Interrupted, reset sleep and iterate
            else if (EINTR == errno) {
                sleep_interval = remaining_interval;
                continue;
            }
            // Anything else is an error
            else {
                task_status = Os::Task::Status::DELAY_ERROR;
                break;
            }
        }
        return task_status;
    }

    TaskInterface* TaskInterface::getDelegate(HandleStorage& aligned_new_memory) {
        return Os::Delegate::makeDelegate<TaskInterface, Os::VxWorks::Task::VxWorksTask>(aligned_new_memory);
    }

}
