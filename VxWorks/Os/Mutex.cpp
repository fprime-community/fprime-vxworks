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
    FW_ASSERT(this->m_handle.m_mutex_descriptor != SEM_ID_NULL, static_cast<FwAssertArgType>(errno));
}

VxWorksMutex::~VxWorksMutex() {
    STATUS status = semDelete(this->m_handle.m_mutex_descriptor);
    FW_ASSERT(status == VXWORKS_OK, static_cast<FwAssertArgType>(errno));
}

VxWorksMutex::Status VxWorksMutex::take() {
    STATUS status = semTake(this->m_handle.m_mutex_descriptor, WAIT_FOREVER);
    PlatformIntType statusType = (status == VXWORKS_OK) ? VXWORKS_OK : errno;

    // Keep track of semTakes and assert we are not re-entering.
    this->m_handle.m_sem_take_counter++;
    FW_ASSERT(this->m_handle.m_sem_take_counter == 1, static_cast<FwAssertArgType>(this->m_handle.m_sem_take_counter));

    return Os::VxWorks::vxworks_status_to_mutex_status(statusType);
}

VxWorksMutex::Status VxWorksMutex::release() {
    // Keep track of semTakes
    this->m_handle.m_sem_take_counter--;
    STATUS status = semGive(this->m_handle.m_mutex_descriptor);
    PlatformIntType statusType = (status == VXWORKS_OK) ? VXWORKS_OK : errno;
    return Os::VxWorks::vxworks_status_to_mutex_status(statusType);
}

MutexHandle* VxWorksMutex::getHandle() {
    return &this->m_handle;
}
}  // namespace Mutex
}  // namespace VxWorks
}  // namespace Os
