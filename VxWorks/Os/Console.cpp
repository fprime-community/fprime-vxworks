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
        FwIndexType m_tail_index = this->m_handle.m_tail_index;
        FW_ASSERT(m_tail_index < MAX_CONSOLE_CAPACITY, m_tail_index, MAX_CONSOLE_CAPACITY);
        FwSizeType minSize = FW_MIN(size, MAX_CONSOLE_MESSAGE_BYTE_SIZE);
        (void)memcpy(this->m_handle.circularBuffer[m_tail_index], message, minSize);
        this->m_handle.circularBuffer[m_tail_index][minSize] = '\0';
        logMsg(this->m_handle.circularBuffer[m_tail_index], 0, 0, 0, 0, 0, 0);
        m_tail_index = ((m_tail_index + 1) % MAX_CONSOLE_CAPACITY);
        this->m_handle.m_tail_index = m_tail_index;
    }
}

ConsoleHandle* VxWorksConsole::getHandle() {
    return &this->m_handle;
}

}  // namespace Console
}  // namespace VxWorks
}  // namespace Os
