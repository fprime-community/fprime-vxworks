// ======================================================================
// \title VxWorks/Os/ConsoleNoWait.hpp
// \brief VxWorks implementation for Os::Console using logMsgNoWait, header and test definitions
// ======================================================================
#include <Os/Console.hpp>
#include <config/VxWorksCfg.hpp>
#ifndef OS_VXWORKS_ConsoleNoWait_HPP
#define OS_VXWORKS_ConsoleNoWait_HPP

#include <atomic>

namespace Os {
namespace VxWorks {
namespace ConsoleNoWait {

//! ConsoleHandle class definition for VxWorks implementations.
//!
struct VxWorksConsoleHandle : public ConsoleHandle {
    char circularBuffer[MAX_CONSOLE_CAPACITY][MAX_CONSOLE_MESSAGE_BYTE_SIZE];  // Circular buffer to store messages
    std::atomic<FwSizeType> m_tail_index;  // Index pointing to the tail of the circular buffer

    VxWorksConsoleHandle() { m_tail_index = 0; }

    VxWorksConsoleHandle(const VxWorksConsoleHandle& other) {
        if (&other == this) {
            return;
        }
        m_tail_index = 0;
    }

    VxWorksConsoleHandle& operator=(const VxWorksConsoleHandle& other) {
        this->m_tail_index = 0;
        return *this;
    }
};

//! \brief VxWorks implementation of Os::ConsoleInterface using logMsgNoWait
//!
//! VxWorks implementation of `ConsoleInterface` for use as a delegate class handling VxWorks console operations.
//! This implementation uses logMsgNoWait instead of logMsg, which does not block if the message queue is full.
//! Messages may be dropped if the queue is full, but the calling task will not be blocked.
//!
class VxWorksConsoleNoWait : public ConsoleInterface {
  public:
    //! \brief constructor
    //!
    VxWorksConsoleNoWait() = default;

    //! \brief copy constructor
    VxWorksConsoleNoWait(const VxWorksConsoleNoWait& other) = default;

    //! \brief assignment operator that copies the internal representation
    VxWorksConsoleNoWait& operator=(const VxWorksConsoleNoWait& other) = default;

    //! \brief destructor
    //!
    ~VxWorksConsoleNoWait() override = default;

    // ------------------------------------
    // Functions overrides
    // ------------------------------------

    //! \brief write message to console
    //!
    //! Write a message to the console with a bounded size. This uses logMsgNoWait which will not block
    //! if the message queue is full (messages may be dropped).
    //!
    //! \param message: raw message to write
    //! \param size: size of the message to write to the console
    void writeMessage(const CHAR* message, const FwSizeType size) override;

    //! \brief returns the raw console handle
    //!
    //! Gets the raw console handle from the implementation. Note: users must include the implementation specific
    //! header to make any real use of this handle. Otherwise it will be as an opaque type.
    //!
    //! \return raw console handle
    //!
    ConsoleHandle* getHandle() override;

  private:
    //! File handle for VxWorksFile
    VxWorksConsoleHandle m_handle;
};
}  // namespace ConsoleNoWait
}  // namespace VxWorks
}  // namespace Os

#endif  // OS_VXWORKS_ConsoleNoWait_HPP
