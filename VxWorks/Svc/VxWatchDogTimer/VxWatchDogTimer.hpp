// ======================================================================
// \title  VxWatchDogTimer.hpp
// \author ortega
// \brief  hpp file for VxWatchDogTimer component implementation class
// ======================================================================

#ifndef SVC_VXWORKS_VXWATCHDOGTIMER_HPP
#define SVC_VXWORKS_VXWATCHDOGTIMER_HPP

#include <sysLib.h>
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

    void quit();

    void startTimer(Fw::TimeInterval interval);

    void startTimer(FwSizeType microseconds);

    void startTimerTick(_Vx_ticks_t ticks);

  private:
    // static ISR callback
    static void s_driverISR(void* arg);

    // ----------------------------------------------------------------------
    // Member variable
    // ----------------------------------------------------------------------
    static constexpr U32 MS_PER_SECS = 1000;
    static constexpr U32 USECS_PER_MS = 1000;
    static constexpr U32 USECS_PER_SECS = 1000000;
    WDOG_ID m_watchdogId = nullptr;
    _Vx_ticks_t m_tickDelay = 0;
};

}  // namespace VxWorksSvc

#endif
