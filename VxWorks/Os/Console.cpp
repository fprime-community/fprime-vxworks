// ======================================================================
// \title VxWorks/Os/Console.cpp
// \brief VxWorks implementation for Os::Console
// ======================================================================
#include "Console.hpp"
#include <logLib.h>
#include <Fw/Types/Assert.hpp>
#include <cstring>

namespace Os {
namespace VxWorks {
namespace Console {

ConsoleHandle* VxWorksConsole::getHandle() {
    return &this->m_handle;
}

}  // namespace Console
}  // namespace VxWorks
}  // namespace Os
