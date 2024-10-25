// ======================================================================
// \title VxWorks/Os/Console.hpp
// \brief VxWorks implementation for Os::Console, header and test definitions
// ======================================================================
#include <Os/Console.hpp>
#include <cstdio>
#ifndef OS_VXWORKS_Console_HPP
#define OS_VXWORKS_Console_HPP

namespace Os {
namespace VxWorks {
namespace Console {

//! ConsoleHandle class definition for VxWorks implementations.
//!
struct VxWorksConsoleHandle : public ConsoleHandle {
    //! VxWorks console file descriptor
    FILE* m_file_descriptor = stdout;
};

//! \brief VxWorks implementation of Os::ConsoleInterface
//!
//! VxWorks implementation of `ConsoleInterface` for use as a delegate class handling VxWorks console operations.
//! VxWorks consoles write to either standard out or standard error. The default file descriptor used is standard out.
//! This may be changed by calling `setOutputStream`.
//!
class VxWorksConsole : public ConsoleInterface {
  public:
    //! Stream selection enumeration
    enum Stream {
        STANDARD_OUT = 0,   //!< Use standard output stream
        STANDARD_ERROR = 1  //!< Use standard error stream
    };
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

    //! \brief select the output stream
    //!
    //! There are two streams defined: standard out, and standard error. This allows users of the VxWorks log
    //! implementation to chose which stream to use.
    void setOutputStream(Stream stream);

  private:
    //! File handle for VxWorksFile
    VxWorksConsoleHandle m_handle;
};
}  // namespace Console
}  // namespace VxWorks
}  // namespace Os

#endif  // OS_VXWORKS_Console_HPP
