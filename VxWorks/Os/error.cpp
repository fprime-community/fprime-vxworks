// ======================================================================
// \title VxWorks/Os/error.cpp
// \brief implementation for VxWorks errno conversion
// ======================================================================
#include <cerrno>
#include "VxWorks/Os/error.hpp"

namespace Os {
namespace VxWorks {

Task::Status vxworks_status_to_task_status(PlatformIntType vxworks_status) {
    Task::Status status = Task::Status::OP_OK;
    switch (vxworks_status) {
        case 0:
            status = Task::Status::OP_OK;
            break;
        case EINVAL:
            status = Task::Status::INVALID_PARAMS;
            break;
        case EPERM:
            status = Task::Status::ERROR_PERMISSION;
            break;
        case EAGAIN:
            status = Task::Status::ERROR_RESOURCES;
            break;
        default:
            status = Task::Status::UNKNOWN_ERROR;
            break;
    }
    return status;
}

Mutex::Status vxworks_status_to_mutex_status(PlatformIntType vxworks_status){
    Mutex::Status status = Mutex::Status::ERROR_OTHER;
    switch (vxworks_status) {
        case 0:
            status = Mutex::Status::OP_OK;
            break;
        case EBUSY:
            status = Mutex::Status::ERROR_BUSY;
            break;
        case EDEADLK:
            status = Mutex::Status::ERROR_DEADLOCK;
            break;
        default:
            status = Mutex::Status::ERROR_OTHER;
            break;
    }
    return status;
}

}
}
