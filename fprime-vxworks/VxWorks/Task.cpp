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
    
    Task::TaskStatus Task::start(const Fw::StringBase &name, NATIVE_INT_TYPE identifier, NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize, taskRoutine routine, void* arg, NATIVE_INT_TYPE cpuAffinity) {

        this->m_name = "TV_";
        this->m_name += name;
        this->m_identifier = identifier;
        this->m_started = false;
        this->m_affinity = cpuAffinity;
        
        // convert priority from Posix range to VxWorks range
        NATIVE_INT_TYPE vxPriority = 255 - priority;
        
        // If a registry has been registered, register task
        if (Task::s_taskRegistry) {
            Task::s_taskRegistry->addTask(this);
        }
        
        NATIVE_INT_TYPE stat = taskCreate((char*)this->m_name.toChar(),vxPriority,VX_FP_TASK,stackSize,(FUNCPTR)routine,(int)arg,0,0,0,0,0,0,0,0,0);
        
        if (ERROR == stat) {
            return TASK_UNKNOWN_ERROR; 
        }
        
        this->m_handle = (NATIVE_INT_TYPE)stat;
        
#ifdef _WRS_CONFIG_SMP
        if (cpuAffinity != -1) {
            cpuset_t aff;

            CPUSET_ZERO (aff);
            CPUSET_SET (aff, cpuAffinity);

	    if (taskCpuAffinitySet (this->m_handle, aff) == ERROR) {
               /* Either CPUs are not enabled or we are in UP mode */
               taskDelete (this->m_handle);
               return TASK_INVALID_AFFINITY;
	    }

        }
#endif

        stat = taskActivate(stat); // activate task
        if (ERROR == stat) {
            return TASK_UNKNOWN_ERROR;
        }


        Task::s_numTasks++;
        
        return TASK_OK;
    
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

