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

void VxWorksConsole::writeMessage(const CHAR* message, const FwSizeType size) {
    if (message != nullptr) {
        FwSizeType currentIndex = this->m_handle.m_tail_index;
        FW_ASSERT(currentIndex < MAX_CONSOLE_CAPACITY, static_cast<FwAssertArgType>(currentIndex),
                  static_cast<FwAssertArgType>(MAX_CONSOLE_CAPACITY));
        FwSizeType minSize = FW_MIN(size, MAX_CONSOLE_MESSAGE_BYTE_SIZE);
        (void)memcpy(this->m_handle.circularBuffer[currentIndex], message, minSize);
        this->m_handle.circularBuffer[currentIndex][minSize] = '\0';
        logMsg(this->m_handle.circularBuffer[currentIndex], 0, 0, 0, 0, 0, 0);
        this->m_handle.m_tail_index = ((currentIndex + 1) % MAX_CONSOLE_CAPACITY);
    }
}

ConsoleHandle* VxWorksConsole::getHandle() {
    return &this->m_handle;
}

}  // namespace Console
}  // namespace VxWorks
}  // namespace Os
