// ======================================================================
// \title VxWorks/Os/Console.cpp
// \brief VxWorks implementation for Os::Console using logMsg
// ======================================================================
#include "Console.hpp"
#include <logLib.h>
#include <Fw/Types/Assert.hpp>
#include <cstring>

namespace Os {
namespace VxWorks {
namespace Console {

void VxWorksConsole::myLogMsg(const FwSizeType currentIndex) {
    // logMsg() checks to see whether or not it is running in interrupt context.
    // If it is, it will not block. However, if invoked from a task, it can cause
    // the task to block.
    (void)logMsg(this->m_handle.circularBuffer[currentIndex], 0, 0, 0, 0, 0, 0);
}

}  // namespace Console
}  // namespace VxWorks
}  // namespace Os
