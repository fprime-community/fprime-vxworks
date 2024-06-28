#include <Os/Task.hpp>

#include <vxWorks.h>
#include <taskLib.h> // need it for VX_FP_TASK
#include <sysLib.h>
#include <tickLib.h>

#include <errno.h>
#include <string.h>

namespace Os {
    
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
}