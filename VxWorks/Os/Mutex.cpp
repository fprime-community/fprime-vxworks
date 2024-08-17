// ======================================================================
// \title VxWorks/Os/Mutex.cpp
// \brief VxWorks implementation for Os::Mutex
// ======================================================================
#include "Mutex.hpp"
#include "error.hpp"
#include <Fw/Types/Assert.hpp>

namespace Os {
namespace VxWorks {
namespace Mutex {

VxWorksMutex::VxWorksMutex() : Os::MutexInterface(), m_handle() {
    // set attributes
    pthread_mutexattr_t attribute;
    PlatformIntType status = pthread_mutexattr_init(&attribute);
    FW_ASSERT(status == 0, status);

    // set to normal mutex type
    status = pthread_mutexattr_settype(&attribute, PTHREAD_MUTEX_NORMAL);
    FW_ASSERT(status == 0, status);

    // set to check for priority inheritance
    status = pthread_mutexattr_setprotocol(&attribute, PTHREAD_PRIO_INHERIT);
    FW_ASSERT(status == 0, status);

    status = pthread_mutex_init(&this->m_handle.m_mutex_descriptor, &attribute);
    FW_ASSERT(status == 0, status);
}

VxWorksMutex::~VxWorksMutex() {
    PlatformIntType status = pthread_mutex_destroy(&this->m_handle.m_mutex_descriptor);
    FW_ASSERT(status == 0, status);
}

VxWorksMutex::Status VxWorksMutex::take() {
    PlatformIntType status = pthread_mutex_lock(&this->m_handle.m_mutex_descriptor);
    return Os::VxWorks::vxworks_status_to_mutex_status(status);
}

VxWorksMutex::Status VxWorksMutex::release() {
    PlatformIntType status = pthread_mutex_unlock(&this->m_handle.m_mutex_descriptor);
    return Os::VxWorks::vxworks_status_to_mutex_status(status);
}

MutexHandle* VxWorksMutex::getHandle() {
    return &this->m_handle;
}
}  // namespace Mutex
}  // namespace VxWorks
}  // namespace Os
