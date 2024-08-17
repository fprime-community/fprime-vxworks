// ======================================================================
// \title VxWorks/Os/DefaultMutex.cpp
// \brief sets default Os::Mutex VxWorks implementation via linker
// ======================================================================
#include "Mutex.hpp"
#include "Os/Delegate.hpp"
namespace Os {

//! \brief get a delegate for MutexInterface that intercepts calls for VxWorks
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
MutexInterface *MutexInterface::getDelegate(HandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<MutexInterface, Os::VxWorks::Mutex::VxWorksMutex>(
            aligned_new_memory
    );
}
}
