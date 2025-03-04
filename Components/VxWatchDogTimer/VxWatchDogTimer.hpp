// ======================================================================
// \title  VxWatchDogTimer.hpp
// \author ortega
// \brief  hpp file for VxWatchDogTimer component implementation class
// ======================================================================

#ifndef Components_VxWatchDogTimer_HPP
#define Components_VxWatchDogTimer_HPP

#include <wdLib.h>
#include "Components/VxWatchDogTimer/VxWatchDogTimerComponentAc.hpp"

namespace Components {

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

    void startWatchdog(U32 ticks);

    void startWatchdog(Fw::TimeInterval interval);

    void stopWatchdog();

    PRIVATE :

        // static ISR callback
        static void
        s_driverISR(void* arg);

    // ----------------------------------------------------------------------
    // Member variable
    // ----------------------------------------------------------------------
    WDOG_ID m_watchdogId = nullptr;
    _Vx_ticks_t m_tickDelay = 0;
};

}  // namespace Components

#endif
