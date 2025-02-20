// ======================================================================
// \title VxWorks/Os/Console.hpp
// \brief VxWorks implementation for Os::Console, header and test definitions
// ======================================================================
#include <Os/Console.hpp>
#include <VxWorksCfg.hpp>
#ifndef OS_VXWORKS_Console_HPP
#define OS_VXWORKS_Console_HPP

namespace Os {
namespace VxWorks {
namespace Console {

//! ConsoleHandle class definition for VxWorks implementations.
//!
struct VxWorksConsoleHandle : public ConsoleHandle {
    char circularBuffer[MAX_CONSOLE_CAPACITY][MAX_CONSOLE_MESSAGE_BYTE_SIZE];  // Circular buffer to store messages
    FwIndexType m_tail_index = 0;  // Index pointing to the tail of the circular buffer
};

//! \brief VxWorks implementation of Os::ConsoleInterface
//!
//! VxWorks implementation of `ConsoleInterface` for use as a delegate class handling VxWorks console operations.
//! VxWorks consoles write to either standard out or standard error. The default file descriptor used is standard out.
//! This may be changed by calling `setOutputStream`.
//!
class VxWorksConsole : public ConsoleInterface {
  public:
    //! \brief constructor
    //!
    VxWorksConsole() = default;

    //! \brief copy constructor
    VxWorksConsole(const VxWorksConsole& other) = default;

    //! \brief assignment operator that copies the internal representation
    VxWorksConsole& operator=(const VxWorksConsole& other) = default;

    //! \brief destructor
    //!
    ~VxWorksConsole() override = default;

    // ------------------------------------
    // Functions overrides
    // ------------------------------------

    //! \brief write message to console
    //!
    //! Write a message to the console with a bounded size. This will use the active file descriptor as the output
    //! destination.
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

    // ------------------------------------
    // Helper functions
    // ------------------------------------

  private:
    //! File handle for VxWorksFile
    VxWorksConsoleHandle m_handle;
};
}  // namespace Console
}  // namespace VxWorks
}  // namespace Os

#endif  // OS_VXWORKS_Console_HPP
