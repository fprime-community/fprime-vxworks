// ======================================================================
// \title VxWorks/Os/ConditionVariable.hpp
// \brief VxWorks definitions for Os::ConditionVariable
// ======================================================================
#ifndef OS_VXWORKS_CONDITION_VARIABLE_HPP
#define OS_VXWORKS_CONDITION_VARIABLE_HPP
#include <condVarLib.h>
#include <Os/Condition.hpp>

namespace Os {
namespace VxWorks {
namespace Mutex {

struct VxWorksConditionVariableHandle : public ConditionVariableHandle {
    CONDVAR_ID m_condition;  //! The condition variable
};

//! \brief VxWorks implementation of Os::ConditionVariable
//!
//! VxWorks implementation of `ConditionVariable` for use as a delegate class handling error-only file operations.
//!
class VxWorksConditionVariable : public ConditionVariableInterface {
  public:
    //! \brief constructor
    //!
    VxWorksConditionVariable();

    //! \brief destructor
    //!
    ~VxWorksConditionVariable() override;

    ConditionVariableInterface& operator=(const ConditionVariableInterface& other) override = delete;

    //! \brief wait releasing mutex
    void wait(Os::Mutex& mutex) override;

    //! \brief notify a single waiter
    void notify() override;

    //! \brief notify all current waiters
    void notifyAll() override;

    //! \brief get handle
    ConditionVariableHandle* getHandle() override;

  private:
    //! Handle for VxWorksMutex
    VxWorksConditionVariableHandle m_handle;
};

}  // namespace Mutex
}  // namespace VxWorks
}  // namespace Os
#endif  // OS_VXWORKS_CONDITION_VARIABLE_HPP
