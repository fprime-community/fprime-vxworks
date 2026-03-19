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
        static_assert(std::is_unsigned<FwSizeType>::value, "FwSizeType is expected to be unsigned.");
        static_assert(MAX_CONSOLE_CAPACITY > 0, "Avoid dividing by zero.");
        static_assert(MAX_CONSOLE_MESSAGE_BYTE_SIZE > 0, "Should not have a message size of 0 bytes.");
        // Rely on unsigned overflow to atomically roll over tail index
        const FwSizeType currentIndex = this->m_handle.m_tail_index.fetch_add(1) % MAX_CONSOLE_CAPACITY;
        FW_ASSERT(currentIndex < MAX_CONSOLE_CAPACITY, static_cast<FwAssertArgType>(currentIndex),
                  static_cast<FwAssertArgType>(MAX_CONSOLE_CAPACITY));
        FwSizeType minSize = FW_MIN(size, MAX_CONSOLE_MESSAGE_BYTE_SIZE - 1);
        (void)memcpy(this->m_handle.circularBuffer[currentIndex], message, minSize);
        this->m_handle.circularBuffer[currentIndex][minSize] = '\0';
        this->myLogMsg(currentIndex);
    }
}
ConsoleHandle* VxWorksConsole::getHandle() {
    return &this->m_handle;
}

}  // namespace Console
}  // namespace VxWorks
}  // namespace Os
