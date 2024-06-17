// ======================================================================
// \title Os/Posix/Task.hpp
// \brief definitions of Posix implementation of Os::Task
// ======================================================================
#ifndef Os_Posix_Task_hpp_
#define Os_Posix_Task_hpp_

#include <atomic>
#include <pthread.h>
#include <Os/Task.hpp>

#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Os/TaskString.hpp>
#include <Os/Mutex.hpp>
#include <Fw/Deprecate.hpp>

// VxWorks headers
#include <vxWorks.h>
#include <taskLib.h> // need it for VX_FP_TASK
#include <sysLib.h>
#include <tickLib.h>

namespace Os {
namespace VxWorks {
namespace Task {

    //! TaskHandle class definition for posix implementations.
    //!
    struct VxWorksTaskHandle : public TaskHandle {
        static constexpr PlatformIntType SUCCESS = 0;

        //! Posix task descriptor
        FwNativeIntType m_task_descriptor;
    };

    //! Posix task implementation as driven by pthreads implementation
    class VxWorksTask : public TaskInterface {
      public:

        //! \brief default constructor
        VxWorksTask() = default;

        //! \brief default virtual destructor
        ~VxWorksTask() = default;

        //! \brief copy constructor is forbidden
        VxWorksTask(const VxWorksTask& other) = delete;

        //! \brief assignment operator is forbidden
        VxWorksTask& operator=(const VxWorksTask& other) = delete;


        //! \brief perform required task start actions
        void onStart() override;

        //! \brief start the task
        //!
        //! Starts the task given the supplied arguments. This is done via the a task routine wrapper intermediary that
        //! ensures that `setStarted` is called once the task has actually started to run. The task then runs the user
        //! routine. This function may return before the new task begins to run.
        //
        //! It is illegal for arguments.m_routine to be null.
        //!
        //! \param arguments: arguments supplied to the task start call
        //! \return status of the task start
        Status start(const Arguments& arguments) override;

        //! \brief block until the task has ended
        //!
        //! Blocks the current (calling) task until this task execution has ended. Callers should ensure that any
        //! signals required to stop this task have already been emitted or will be emitted by another task.
        //!
        //! \return status of the block
        Status join() override; //!< Wait for task to finish

        //! \brief suspend the task given the suspension type
        //!
        //! Suspends the task. Some implementations track if the suspension of a task was intentional or
        //! unintentional. The supplied `suspensionType` parameter indicates that this was intentional or
        //! unintentional. The type of suspension is also returned when calling `isSuspended`.
        //!
        //! \param suspensionType intentionality of the suspension
        void suspend(SuspensionType suspensionType) override;

        //! \brief resume a suspended task
        //!
        //! Resumes this task. Not started, running, and exited tasks take no action.
        //!
        void resume() override;

        //! \brief return the underlying task handle (implementation specific)
        //! \return internal task handle representation
        TaskHandle* getHandle() override;
      PRIVATE:

        VxWorksTaskHandle m_handle; //!< Posix task tracking
    };
} // end namespace Task
} // end namespace VxWorks
} // end namespace Os
#endif
