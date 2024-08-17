// ======================================================================
// \title VxWorks/Os/error.hpp
// \brief header for VxWorks errno conversion
// ======================================================================
#include "Os/Task.hpp"
#ifndef OS_VXWORKS_ERRNO_HPP
#define OS_VXWORKS_ERRNO_HPP

namespace Os {
namespace VxWorks {

//! Convert an vxworks task representation of an error to the Os::Task::Status representation.
//! \param vxworks_status: errno representation of the error
//! \return: Os::Task::Status representation of the error
//!
Os::Task::Status vxworks_status_to_task_status(PlatformIntType vxworks_status);

}
}
#endif
