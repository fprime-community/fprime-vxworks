// ======================================================================
// \title VxWorks/Os/Mutex.cpp
// \brief VxWorks implementation for Os::Mutex
// ======================================================================
#include "VxWorks/Os/Mutex.hpp"
#include <Fw/Types/Assert.hpp>
#include "VxWorks/Os/error.hpp"

namespace Os {
namespace VxWorks {
namespace Mutex {

VxWorksMutex::VxWorksMutex() : Os::MutexInterface(), m_handle() {
    this->m_handle.m_mutex_descriptor = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE);
    FW_ASSERT(this->m_handle.m_mutex_descriptor != SEM_ID_NULL);
}

VxWorksMutex::~VxWorksMutex() {
    (void)semDelete(this->m_handle.m_mutex_descriptor);
}

VxWorksMutex::Status VxWorksMutex::take() {
    STATUS status = semTake(this->m_handle.m_mutex_descriptor, WAIT_FOREVER);
    return Os::VxWorks::vxworks_status_to_mutex_status(status);
}

VxWorksMutex::Status VxWorksMutex::release() {
    STATUS status = semGive(this->m_handle.m_mutex_descriptor);
    return Os::VxWorks::vxworks_status_to_mutex_status(status);
}

MutexHandle* VxWorksMutex::getHandle() {
    return &this->m_handle;
}
}  // namespace Mutex
}  // namespace VxWorks
}  // namespace Os
