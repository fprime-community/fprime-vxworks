// ======================================================================
// \title VxWorks/Os/DefaultConsole.cpp
// \brief sets default Os::Console to posix implementation via linker
// ======================================================================
#include "Os/Console.hpp"
#include "Os/Delegate.hpp"
#include "VxWorks/Os/Console.hpp"

namespace Os {
ConsoleInterface* ConsoleInterface::getDelegate(ConsoleHandleStorage& aligned_new_memory,
                                                const ConsoleInterface* to_copy) {
    return Os::Delegate::makeDelegate<ConsoleInterface, Os::VxWorks::Console::VxWorksConsole>(aligned_new_memory,
                                                                                              to_copy);
}
}  // namespace Os
