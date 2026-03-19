// ======================================================================
// \title VxWorks/Os/ConsoleNoWait.cpp
// \brief VxWorks implementation for Os::Console using logMsgNoWait
// ======================================================================
#include "Console.hpp"
#include <logLib.h>
#include <Fw/Types/Assert.hpp>
#include <cstring>

namespace Os {
namespace VxWorks {
namespace Console {

void VxWorksConsole::myLogMsg(const FwSizeType currentIndex) {
    // logMsgNoWait() will not block when called from a task or when called from an ISR.
    (void)logMsgNoWait(this->m_handle.circularBuffer[currentIndex], 0, 0, 0, 0, 0, 0);
}

}  // namespace Console
}  // namespace VxWorks
}  // namespace Os
