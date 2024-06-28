#include <Os/Task.hpp>
#include <Fw/Types/Assert.hpp>

#include <vxWorks.h>
#include <taskLib.h> // need it for VX_FP_TASK
#include <sysLib.h>
#include <tickLib.h>

#include <errno.h>
#include <string.h>

namespace Os {
    Task::Task() : m_handle(0), m_identifier(0), m_affinity(-1), m_started(false), m_suspendedOnPurpose(false) {
    }
    
    Task::~Task() {
        // If a registry has been registered, remove task
        if (Task::s_taskRegistry) {
            Task::s_taskRegistry->removeTask(this);
        }
        
    }
    
    Task::TaskStatus Task::delay(NATIVE_UINT_TYPE milliseconds)
    {

        // Lookup the number of clock ticks for the specified
        // number of milliseconds. Use the ticks to call the
        // task delay function call.
        Task::TaskStatus status = TASK_DELAY_ERROR;
        
        U32 ticks = ((milliseconds * sysClkRateGet() + 999) / 1000);
        
        if (taskDelay(ticks) == OK) {
            status = TASK_OK;
        }
        return status;

    }
    
    void Task::suspend(bool onPurpose) {
        FW_ASSERT(taskSuspend(this->m_handle) == OK);
    }
                    
    void Task::resume(void) {
        FW_ASSERT(taskResume(this->m_handle) == OK);
    }

    bool Task::isSuspended(void) {
        return (taskIsSuspended(this->m_handle) == TRUE)?true:false;
    }

    TaskId Task::getOsIdentifier(void) {
      TaskId T; 
      return T;
    }

    Task::TaskStatus Task::join(void **value_ptr) {
        // No task join in taskLib.  Return error if used.
        return TASK_JOIN_ERROR;
    }
}