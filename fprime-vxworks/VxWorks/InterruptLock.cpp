#include <Os/InterruptLock.hpp>

#include <intLib.h>

namespace Os {
    InterruptLock::InterruptLock(void): m_key(0) {}
    InterruptLock::~InterruptLock(void) {}
    
    void InterruptLock::lock(void) {
        this->m_key = (POINTER_CAST)intLock();
    }

    void InterruptLock::unLock(void) {
        intUnlock((int)this->m_key);
    }
    
    POINTER_CAST InterruptLock::getKey(void) {
        return this->m_key;
    }
        
}


