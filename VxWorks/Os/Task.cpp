#include <vxWorks.h>
#include <taskLib.h> // need it for VX_FP_TASK
#include <sysLib.h>

#include <cstring>
#include <unistd.h>
#include <climits>
#include <cerrno>

#include "Fw/Types/Assert.hpp"
#include "Fw/Types/BasicTypes.hpp"
#include "Os/Task.hpp"

#include "VxWorks/Os/Task.hpp"

namespace Os {
namespace VxWorks {
namespace Task {

    //!< This wrapper is needed due to the change in FUNCPTR's signature
    //!< from VxWorks6 to VxWorks7. This static function implements the
    //!< FUNCPTR signature required by VxWorks7 and calls the routine
    //!< with the provides arg. This wrapper function is expected to be
    //!< passed in to taskCreate.
    static int myRoutineWrapper(
        Os::TaskInterface::taskRoutine routine, //!< Routine that should run in a thread
        void* arg //!< Argument passed to the routine
    )
    {
        routine(arg);
        return VXWORKS_OK;
    }

    void VxWorksTask::onStart() {}

    Os::Task::Status VxWorksTask::start(const Arguments& arguments) {
        FW_ASSERT(arguments.m_routine != nullptr);

        // Get taskName into a non-const variable because that is what
        // VxWorks' taskCreate wants.
        char taskName[arguments.m_name.getCapacity()];
        memcpy(taskName, arguments.m_name.toChar(), arguments.m_name.getCapacity());

        // convert priority from Posix range to VxWorks range
        PlatformIntType vxPriority = 255 - arguments.m_priority;

        TASK_ID taskId = taskCreate(
            taskName,
            vxPriority,
            VX_FP_TASK,
            arguments.m_stackSize,
            reinterpret_cast<FUNCPTR>(myRoutineWrapper),
            0,0,0,0,0,0,0,0,0,0);
//TODO
            //reinterpret_cast<int>(arguments.m_routine_argument),0,0,0,0,0,0,0,0,0);
        
        if (TASK_ID_NULL == taskId) {
            return Os::Task::Status::UNKNOWN_ERROR; 
        }
        
        this->m_handle.m_task_descriptor = taskId;
        
#ifdef _WRS_CONFIG_SMP
        if (arguments.m_cpuAffinity != -1) {
            cpuset_t aff;

            CPUSET_ZERO (aff);
            CPUSET_SET (aff, arguments.m_cpuAffinity);

	    if (taskCpuAffinitySet (this->m_handle.m_task_descriptor, aff) == ERROR) {
               /* Either CPUs are not enabled or we are in UP mode */
               taskDelete (this->m_handle.m_task_descriptor);
               return Os::Task::Status::INVALID_AFFINITY;
	    }

        }
#endif

        auto status = taskActivate(taskId); // activate task
        if (ERROR == status) {
            return Os::Task::Status::UNKNOWN_ERROR; 
        }

        return Os::Task::Status::OP_OK;
    }

    Os::Task::Status VxWorksTask::join() {
        // There is no task join in taskLib.
        return Os::Task::Status::JOIN_ERROR;;
    }

    TaskHandle* VxWorksTask::getHandle() {
        return &this->m_handle;
    }

    void VxWorksTask::suspend(Os::Task::SuspensionType /*suspensionType*/) {
        STATUS status = taskSuspend(this->m_handle.m_task_descriptor);
        FW_ASSERT(status == VXWORKS_OK, status);
    }

    void VxWorksTask::resume() {
        STATUS status = taskResume(this->m_handle.m_task_descriptor);
        FW_ASSERT(status == OK, status);
    }
} // end namespace Task
} // end namespace VxWorks
} // end namespace Os



#if 0

// old!!!!
namespace Os {
    Task::Task() : m_handle(0), m_identifier(0), m_affinity(-1), m_started(false), m_suspendedOnPurpose(false) {
    }

    Task::TaskStatus Task::start(const Fw::StringBase &name, taskRoutine routine, void* arg, NATIVE_UINT_TYPE priority, NATIVE_UINT_TYPE stackSize,  NATIVE_UINT_TYPE cpuAffinity, NATIVE_UINT_TYPE identifier) {

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

#endif
