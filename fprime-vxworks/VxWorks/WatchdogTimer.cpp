#include <Os/WatchdogTimer.hpp>

#include <vxWorks.h>
#include <wdLib.h>
#include <sysLib.h>

namespace Os {
    
    WatchdogTimer::WatchdogTimer()
    : m_cb(0),m_parameter(0),m_timerTicks(0),m_timerMs(0)
    {
        this->m_handle = (I32)wdCreate();
    }
    
    WatchdogTimer::~WatchdogTimer() {

        if (NULL != (WDOG_ID)this->m_handle) {
            (void)wdDelete((WDOG_ID)this->m_handle);
        }
        
    }
    
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

    WatchdogTimer::WatchdogStatus WatchdogTimer::startMs( I32 delayInMs, WatchdogCb p_callback, void* parameter) {
        
        if (NULL == (WDOG_ID)this->m_handle) {
            return WATCHDOG_INIT_ERROR;
        }
        
        // convert to ticks
        // Only perform this conversion on VxWorks. Take the number
        // of ticks per second and multiply by the number of seconds.
        // Add 999 to force a roundup situation.

        I32 ticks = ((delayInMs * sysClkRateGet() + 999) / 1000);
        
        return this->startTicks(ticks,p_callback,parameter);

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

    WatchdogTimer::WatchdogStatus WatchdogTimer::cancel(void) {

        if (NULL == (WDOG_ID)this->m_handle) {
            return WATCHDOG_INIT_ERROR;
        }
        
        STATUS stat = wdCancel(
                        (WDOG_ID)this->m_handle);
        
        if (ERROR == stat) {
            return WATCHDOG_CANCEL_ERROR;
        } else {
            return WATCHDOG_OK;
        }

    }
    
}


