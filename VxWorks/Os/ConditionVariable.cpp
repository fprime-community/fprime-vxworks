// ======================================================================
// \title VxWorks/Os/ConditionVariable.cpp
// \brief VxWorks implementations for Os::ConditionVariable
// ======================================================================
#include "VxWorks/Os/ConditionVariable.hpp"
#include "Fw/Types/Assert.hpp"
#include "VxWorks/Os/Mutex.hpp"
#include "VxWorks/Os/error.hpp"

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

VxWorksConditionVariable::Status VxWorksConditionVariable::pend(Os::Mutex& mutex) {
    VxWorksMutexHandle* mutex_handle = reinterpret_cast<VxWorksMutexHandle*>(mutex.getHandle());
    FW_ASSERT(mutex_handle != nullptr);

    // Keep track of the number sem takes in order to detect and avoid re-entry semTakes
    mutex_handle->m_sem_take_counter--;
    // Assert mutex is not taken otherwise we have a re-entry mutex, which we do not want.
    FW_ASSERT(mutex_handle->m_sem_take_counter == 0, static_cast<FwAssertArgType>(mutex_handle->m_sem_take_counter));

    // condVarWait is expected to release and retake mutex. Hence why we decrement and increment m_sem_take_counter.
    auto status = condVarWait(this->m_handle.m_condition, mutex_handle->m_mutex_descriptor, WAIT_FOREVER);

    // Keep track of the number sem takes in order to detect re-entry semTakes
    mutex_handle->m_sem_take_counter++;

    PlatformIntType statusReturn = (status == VXWORKS_OK) ? VXWORKS_OK : errno;

    return Os::VxWorks::vxworks_status_to_conditional_status(statusReturn);
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
