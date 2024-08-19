// ======================================================================
// \title VxWorks/Os/error.cpp
// \brief implementation for VxWorks errno conversion
// ======================================================================
#include <cerrno>
#include "VxWorks/Os/error.hpp"

namespace Os {
namespace VxWorks {

Task::Status vxworks_status_to_task_status(STATUS vxworks_status) {
    Task::Status status = Task::Status::UNKNOWN_ERROR;
    switch (vxworks_status) {
        case OK:
            status = Task::Status::OP_OK;
            break;
        default:
            status = Task::Status::UNKNOWN_ERROR;
            break;
    }
    return status;
}

Mutex::Status vxworks_status_to_mutex_status(STATUS vxworks_status){
    Mutex::Status status = Mutex::Status::ERROR_OTHER;
    switch (vxworks_status) {
        case OK:
            status = Mutex::Status::OP_OK;
            break;
        default:
            status = Mutex::Status::ERROR_OTHER;
            break;
    }
    return status;
}

}
}
