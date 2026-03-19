// ======================================================================
// \title VxWorks/Os/ConsoleNoWait.cpp
// \brief VxWorks implementation for Os::Console using logMsgNoWait
// ======================================================================
#include <logLib.h>
#include <Fw/Types/Assert.hpp>
#include "Console.hpp"

namespace Os {
namespace VxWorks {
namespace Console {

void VxWorksConsole::logMsgImpl(const FwSizeType currentIndex) {
    FW_ASSERT(currentIndex < MAX_CONSOLE_CAPACITY, static_cast<FwAssertArgType>(currentIndex),
              static_cast<FwAssertArgType>(MAX_CONSOLE_CAPACITY));
    // logMsgNoWait() will not block when called from a task or when called from an ISR.
    (void)logMsgNoWait(this->m_handle.circularBuffer[currentIndex], 0, 0, 0, 0, 0, 0);
}

}  // namespace Console
}  // namespace VxWorks
}  // namespace Os
