// ======================================================================
// \title VxWorks/Os/error.cpp
// \brief implementation for VxWorks errno conversion
// ======================================================================
#include "VxWorks/Os/error.hpp"
#include <condVarLib.h>  // Needed for condition-var's errno

namespace Os {
namespace VxWorks {

Task::Status vxworks_status_to_task_status(int vxworks_status) {
    Task::Status status = Task::Status::UNKNOWN_ERROR;
    switch (vxworks_status) {
        case VXWORKS_OK:
            status = Task::Status::OP_OK;
            break;
        default:
            status = Task::Status::UNKNOWN_ERROR;
            break;
    }
    return status;
}

Mutex::Status vxworks_status_to_mutex_status(int vxworks_status) {
    Mutex::Status status = Mutex::Status::ERROR_OTHER;
    switch (vxworks_status) {
        case VXWORKS_OK:
            status = Mutex::Status::OP_OK;
            break;
        default:
            status = Mutex::Status::ERROR_OTHER;
            break;
    }
    return status;
}

ConditionVariable::Status vxworks_status_to_conditional_status(int vxworks_status) {
    ConditionVariable::Status status = ConditionVariable::Status::ERROR_OTHER;
    switch (vxworks_status) {
        case VXWORKS_OK:
            status = ConditionVariable::Status::OP_OK;
            break;
        case S_semLib_INVALID_OPERATION:
            status = ConditionVariable::Status::ERROR_MUTEX_NOT_HELD;
            break;
        case S_condVarLib_INVALID_OPERATION:
            status = ConditionVariable::Status::ERROR_DIFFERENT_MUTEX;
            break;
        default:
            status = ConditionVariable::Status::ERROR_OTHER;
            break;
    }
    return status;
}

}  // namespace VxWorks
}  // namespace Os
