// ======================================================================
// \title VxWorks/Os/DefaultMutex.cpp
// \brief sets default Os::Mutex VxWorks implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "VxWorks/Os/ConditionVariable.hpp"
#include "VxWorks/Os/Mutex.hpp"

namespace Os {

//! \brief get a delegate for MutexInterface that intercepts calls for VxWorks
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
MutexInterface* MutexInterface::getDelegate(MutexHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<MutexInterface, Os::VxWorks::Mutex::VxWorksMutex>(aligned_new_memory);
}

//! \brief get a delegate for MutexInterface that intercepts calls for VxWorks
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
ConditionVariableInterface* ConditionVariableInterface::getDelegate(
    ConditionVariableHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<ConditionVariableInterface, Os::VxWorks::Mutex::VxWorksConditionVariable,
                                      ConditionVariableHandleStorage>(aligned_new_memory);
}
}  // namespace Os
