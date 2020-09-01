#include <Os/Mutex.hpp>
#include <Fw/Types/Assert.hpp>

#include <semLib.h>

namespace Os {
    Mutex::Mutex(void) {
        this->m_handle = (POINTER_CAST) semMCreate(SEM_Q_PRIORITY |
                        SEM_INVERSION_SAFE | SEM_DELETE_SAFE);
        FW_ASSERT(this->m_handle != 0);
    }
    
    Mutex::~Mutex(void) {
        semDelete((SEM_ID)this->m_handle);
    }
    
    void Mutex::lock(void) {
        STATUS stat = semTake((SEM_ID)this->m_handle,WAIT_FOREVER);
        FW_ASSERT(OK == stat);
    }

    void Mutex::unLock(void) {
        STATUS stat = semGive((SEM_ID)this->m_handle);
        FW_ASSERT(OK == stat);
    }
            
}


