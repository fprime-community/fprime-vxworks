// ======================================================================
// \title VxWorks/Os/IntLock.hpp
// \brief VxWorks implementation for interrupt locks
// ======================================================================

#include <VxWorks/Os/IntLock.hpp>
#include <intLib.h>

namespace Os {
namespace VxWorks {

IntLock::IntLock() : m_lock(0) {}

void IntLock::lock() {
    this->m_lock = intLock();
}
// unlock interrupts
void IntLock::unLock() {
    intUnlock(this->m_lock);
}

}  // namespace VxWorks
}  // namespace Os
