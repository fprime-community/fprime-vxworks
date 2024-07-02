#include <vxWorks.h>
#include <taskLib.h> // need it for VX_FP_TASK
#include <sysLib.h>
#include <tickLib.h>


#include <Os/Task.hpp>
#include <Fw/Types/Assert.hpp>

#include <errno.h>
#include <string.h>

namespace Os {

    //!< This wrapper is needed due to the change in FUNCPTR's signature
    //!< from VxWorks6 to VxWorks7. This static function implements the
    //!< FUNCPTR signature required by VxWorks7 and calls the routine
    //!< with the provides arg. This wrapper function is expected to be
    //!< passed in to taskCreate.
    static int myRoutineWrapper(
        Task::taskRoutine routine, //!< Routine that should run in a thread
        void* arg //!< Argument passed to the routine
    )
    {
        routine(arg);
        return VX_OK;
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
        
        NATIVE_INT_TYPE stat = taskCreate((char*)this->m_name.toChar(),vxPriority,VX_FP_TASK,stackSize,(FUNCPTR)myRoutineWrapper,reinterpret_cast<int>(routine), reinterpret_cast<int>(arg),0,0,0,0,0,0,0,0);
        
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