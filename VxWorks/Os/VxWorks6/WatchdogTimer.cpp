#include <Os/WatchdogTimer.hpp>

#include <vxWorks.h>
#include <wdLib.h>
#include <sysLib.h>

namespace Os {
    
    WatchdogTimer::WatchdogStatus WatchdogTimer::startTicks( I32 delayInTicks, WatchdogCb p_callback, void* parameter) {

        if (NULL == (WDOG_ID)this->m_handle) {
            return WATCHDOG_INIT_ERROR;
        }
        
        this->m_timerTicks = delayInTicks;
        this->m_cb = p_callback;
        this->m_parameter = parameter;
        
        STATUS stat = wdStart(
                        (WDOG_ID)this->m_handle, 
                        this->m_timerTicks,
                        (FUNCPTR)this->m_cb, 
                        (int)this->m_parameter);
        
        if (ERROR == stat) {
            return WATCHDOG_START_ERROR;
        } else {
            return WATCHDOG_OK;
        }
    }

    WatchdogTimer::WatchdogStatus WatchdogTimer::restart(void) {

        if (NULL == (WDOG_ID)this->m_handle) {
            return WATCHDOG_INIT_ERROR;
        }
        
        // use stashed values
        STATUS stat = wdStart(
                        (WDOG_ID)this->m_handle, 
                        this->m_timerTicks,
                        (FUNCPTR)this->m_cb, 
                        (int)this->m_parameter);
        
        if (ERROR == stat) {
            return WATCHDOG_START_ERROR;
        } else {
            return WATCHDOG_OK;
        }
        
    }
}