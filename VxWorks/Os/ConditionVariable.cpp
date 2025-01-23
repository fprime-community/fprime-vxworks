// ======================================================================
// \title VxWorks/Os/ConditionVariable.cpp
// \brief VxWorks implementations for Os::ConditionVariable
// ======================================================================
#include "VxWorks/Os/ConditionVariable.hpp"
#include "Fw/Types/Assert.hpp"
#include "VxWorks/Os/Mutex.hpp"

namespace Os {
namespace VxWorks {
namespace Mutex {

VxWorksConditionVariable::VxWorksConditionVariable() {
    this->m_handle.m_condition = condVarCreate(CONDVAR_Q_PRIORITY);
    FW_ASSERT(this->m_handle.m_condition != CONDVAR_ID_NULL,
              static_cast<FwAssertArgType>(errno));  // If this fails, something horrible happened.
}
VxWorksConditionVariable::~VxWorksConditionVariable() {
    (void)condVarDelete(this->m_handle.m_condition);
}

ConditionVariableInterface::Status VxWorksConditionVariable::pend(Os::Mutex& mutex) {
    VxWorksMutexHandle* mutex_handle = reinterpret_cast<VxWorksMutexHandle*>(mutex.getHandle());
    FW_ASSERT(mutex_handle != nullptr);

    // Keep track of the number sem takes in order to detect and avoid re-entry semTakes
    mutex_handle->m_sem_take_counter--;
    // Assert mutex is not taken otherwise we have a re-entry mutex, which we do not want.
    FW_ASSERT(mutex_handle->m_sem_take_counter == 0, static_cast<FwAssertArgType>(mutex_handle->m_sem_take_counter));

    // condVarWait is expected to release and retake mutex. Hence why we decrement and increment m_sem_take_counter.
    auto status = condVarWait(this->m_handle.m_condition, mutex_handle->m_mutex_descriptor, WAIT_FOREVER);

    ConditionVariableInterface::Status statusReturn = ConditionVariableInterface::Status::OP_OK;
    // Keep track of the number sem takes in order to detect re-entry semTakes
    mutex_handle->m_sem_take_counter++;

    if (status == VXWORKS_OK) {
        statusReturn = ConditionVariableInterface::Status::OP_OK;
    } else if (errno == S_semLib_INVALID_OPERATION) {
        statusReturn = ConditionVariableInterface::Status::ERROR_MUTEX_NOT_HELD;
    } else if (errno == S_condVarLib_INVALID_OPERATION) {
        statusReturn = ConditionVariableInterface::Status::ERROR_DIFFERENT_MUTEX;
    } else {
        statusReturn = ConditionVariableInterface::Status::ERROR_OTHER;
    }

    return statusReturn;
}
void VxWorksConditionVariable::notify() {
    auto status = condVarSignal(this->m_handle.m_condition);
    FW_ASSERT(status == VXWORKS_OK, static_cast<FwAssertArgType>(errno));
}
void VxWorksConditionVariable::notifyAll() {
    auto status = condVarBroadcast(this->m_handle.m_condition);
    FW_ASSERT(status == VXWORKS_OK, static_cast<FwAssertArgType>(errno));
}

ConditionVariableHandle* VxWorksConditionVariable::getHandle() {
    return &m_handle;
}

}  // namespace Mutex
}  // namespace VxWorks
}  // namespace Os
