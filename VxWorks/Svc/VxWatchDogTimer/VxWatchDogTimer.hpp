// ======================================================================
// \title  VxWatchDogTimer.hpp
// \author ortega
// \brief  hpp file for VxWatchDogTimer component implementation class
// ======================================================================

#ifndef SVC_VXWORKS_VXWATCHDOGTIMER_HPP
#define SVC_VXWORKS_VXWATCHDOGTIMER_HPP

#include <wdLib.h>
#include "VxWorks/Svc/VxWatchDogTimer/VxWatchDogTimerComponentAc.hpp"

namespace VxWorksSvc {

class VxWatchDogTimer : public VxWatchDogTimerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct VxWatchDogTimer object
    VxWatchDogTimer(const char* const compName  //!< The component name
    );

    //! Destroy VxWatchDogTimer object
    ~VxWatchDogTimer();

    void startTimer(U32 ticks);

    void startTimer(Fw::TimeInterval interval);

    void quit();

    private :

        // static ISR callback
        static void
        s_driverISR(void* arg);

    // ----------------------------------------------------------------------
    // Member variable
    // ----------------------------------------------------------------------
    WDOG_ID m_watchdogId = nullptr;
    _Vx_ticks_t m_tickDelay = 0;
};

}  // namespace VxWorksSvc

#endif
