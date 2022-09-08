#include <Os/WatchdogTimer.hpp>
#include <Fw/Types/Assert.hpp>

#include <vxWorks.h>
#include <wdLib.h>
#include <sysLib.h>

namespace Os {

    //!< This wrapper is needed due to the change in FUNCPTR's signature
    //!< from VxWorks6 to VxWorks7. This static function implements the
    //!< FUNCPTR signature required by VxWorks7 and calls the routine
    //!< with the expected parameter. This wrapper function is expected to be
    //!< passed in to wdStart.
    static int myWatchdogCallback(
        int arg //!< arg is the WatchdogTimer instance that initiated the wdStart
    )
    {
        WatchdogTimer *comp = reinterpret_cast<WatchdogTimer*>(arg);
        FW_ASSERT(comp != nullptr);
        comp->expire();
        return OK;
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
                        (FUNCPTR)myWatchdogCallback,
                        (int)this);
        
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
                        (FUNCPTR)myWatchdogCallback,
                        (int)this);
        
        if (ERROR == stat) {
            return WATCHDOG_START_ERROR;
        } else {
            return WATCHDOG_OK;
        }
        
    }
}