// ======================================================================
// \title VxWorks/Os/IntLock.hpp
// \brief VxWorks implementation for interrupt locks
// ======================================================================

#include <Fw/Types/BasicTypes.hpp>

namespace Os {
namespace VxWorks {

class IntLock {

    public:
        // Default constructor
        IntLock();
        // disallow copies
        IntLock(const IntLock&) = delete;
        IntLock& operator=(const IntLock&) = delete;

        // Lock interrupts
        void lock();
        // unlock interrupts
        void unLock();
    private:
        
        PlatformIntType m_lock; //!< lock value



};

}  // namespace VxWorks
}  // namespace Os
