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
    FW_ASSERT(this->m_handle.m_condition != CONDVAR_ID_NULL);  // If this fails, something horrible happened.
}
VxWorksConditionVariable::~VxWorksConditionVariable() {
    (void)condVarDelete(this->m_handle.m_condition);
}

void VxWorksConditionVariable::wait(Os::Mutex& mutex) {
    VxWorksMutexHandle* mutex_handle = reinterpret_cast<VxWorksMutexHandle*>(mutex.getHandle());
    FW_ASSERT(mutex_handle != nullptr);
    auto status = condVarWait(this->m_handle.m_condition, mutex_handle->m_mutex_descriptor, WAIT_FOREVER);
    FW_ASSERT(status == VXWORKS_OK, static_cast<FwAssertArgType>(status));
}
void VxWorksConditionVariable::notify() {
    auto status = condVarSignal(this->m_handle.m_condition);
    FW_ASSERT(status == VXWORKS_OK, static_cast<FwAssertArgType>(status));
}
void VxWorksConditionVariable::notifyAll() {
    auto status = condVarBroadcast(this->m_handle.m_condition);
    FW_ASSERT(status == VXWORKS_OK, static_cast<FwAssertArgType>(status));
}

ConditionVariableHandle* VxWorksConditionVariable::getHandle() {
    return &m_handle;
}

}  // namespace Mutex
}  // namespace VxWorks
}  // namespace Os
