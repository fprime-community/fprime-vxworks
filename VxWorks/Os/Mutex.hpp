// ======================================================================
// \title VxWorks/Os/Mutex.hpp
// \brief VxWorks definitions for Os::Mutex
// ======================================================================
#ifndef OS_VXWORKS_MUTEX_HPP
#define OS_VXWORKS_MUTEX_HPP
#include <semLib.h>
#include <Os/Mutex.hpp>

namespace Os {
namespace VxWorks {
namespace Mutex {

struct VxWorksMutexHandle : public MutexHandle {
    SEM_ID m_mutex_descriptor = SEM_ID_NULL;
    // Counter helps keep track of re-entry semTakes, which we want to avoid.
    U32 m_sem_take_counter = 0;
};

//! \brief VxWorks implementation of Os::Mutex
//!
//! VxWorks implementation of `MutexInterface` for use as a delegate class handling error-only file operations.
//!
class VxWorksMutex : public MutexInterface {
  public:
    //! \brief constructor
    //!
    VxWorksMutex();

    //! \brief destructor
    //!
    ~VxWorksMutex() override;

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal mutex handle representation
    MutexHandle* getHandle() override;

    Status take() override;     //!<  lock the mutex and get return status
    Status release() override;  //!<  unlock the mutex and get return status

  private:
    //! Handle for VxWorksMutex
    VxWorksMutexHandle m_handle;
};

}  // namespace Mutex
}  // namespace VxWorks
}  // namespace Os
#endif  // OS_VXWORKS_MUTEX_HPP
