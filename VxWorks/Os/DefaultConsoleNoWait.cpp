// ======================================================================
// \title VxWorks/Os/DefaultConsoleNoWait.cpp
// \brief sets default Os::Console to VxWorks ConsoleNoWait implementation via linker
// ======================================================================
#include "Os/Console.hpp"
#include "Os/Delegate.hpp"
#include "VxWorks/Os/ConsoleNoWait.hpp"

namespace Os {
ConsoleInterface* ConsoleInterface::getDelegate(ConsoleHandleStorage& aligned_new_memory,
                                                const ConsoleInterface* to_copy) {
    return Os::Delegate::makeDelegate<ConsoleInterface, Os::VxWorks::ConsoleNoWait::VxWorksConsoleNoWait>(aligned_new_memory,
                                                                                                           to_copy);
}
}  // namespace Os
