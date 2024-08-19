// ======================================================================
// \title VxWorks/Os/error.hpp
// \brief header for VxWorks errno conversion
// ======================================================================
#ifndef OS_VXWORKS_ERRNO_HPP
#define OS_VXWORKS_ERRNO_HPP

#include "Os/Task.hpp"
#include <vxWorks.h>

namespace Os {
namespace VxWorks {

//! Convert an vxworks task representation of an error to the Os::Task::Status representation.
//! \param vxworks_status: errno representation of the error
//! \return: Os::Task::Status representation of the error
//!
Os::Task::Status vxworks_status_to_task_status(STATUS vxworks_status);

//! Convert a VxWorks return status (int) for mutex operations to the Os::Mutex::Status representation.
//! \param vxworks_status: return status
//! \return: Os::Mutex::Status representation of the error
//!
Os::Mutex::Status vxworks_status_to_mutex_status(STATUS vxworks_status);

}
}
#endif
