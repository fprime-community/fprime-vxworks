// ======================================================================
// \title  VxWatchDogTimer.cpp
// \author ortega
// \brief  cpp file for VxWatchDogTimer component implementation class
// ======================================================================

#include "Components/VxWatchDogTimer/VxWatchDogTimer.hpp"
#include <sysLib.h>

namespace Components {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

VxWatchDogTimer ::VxWatchDogTimer(const char* const compName) : VxWatchDogTimerComponentBase(compName) {
    this->m_watchdogId = wdCreate();
}

VxWatchDogTimer ::~VxWatchDogTimer() {
    if (this->m_watchdogId != nullptr) {
        (void)wdCancel(this->m_watchdogId);
        (void)wdDelete(this->m_watchdogId);
    }
}

void VxWatchDogTimer::startWatchdog(U32 ticks) {
    this->m_tickDelay = ticks;
    FW_ASSERT(this->m_watchdogId != nullptr);
    STATUS status = wdStart(this->m_watchdogId, ticks, reinterpret_cast<FUNCPTR>(s_driverISR),
                            reinterpret_cast<_Vx_usr_arg_t>(this));
    FW_ASSERT(status == VXWORKS_OK);
}

void VxWatchDogTimer::startWatchdog(Fw::TimeInterval interval) {
    static constexpr U32 USECS_PER_SECS = 1000000;
    U32 delayInUsecs = (interval.getSeconds() * USECS_PER_SECS) + interval.getUSeconds();
    // Calculate ticks per interval by multiplying interval by number of ticks per second, then round up.
    static_assert(USECS_PER_SECS != 0, "This constant cannot be 0.");
    U32 ticksPerInterval = (((delayInUsecs * sysClkRateGet()) + (USECS_PER_SECS - 1)) / USECS_PER_SECS);
    this->startWatchdog(ticksPerInterval);
}

void VxWatchDogTimer::stopWatchdog() {
    if (this->m_watchdogId != nullptr) {
        STATUS status = wdCancel(this->m_watchdogId);
        FW_ASSERT(status == VXWORKS_OK);
    }
}

void VxWatchDogTimer::s_driverISR(void* arg) {
    FW_ASSERT(arg != nullptr);

    // cast argument to component instance
    VxWatchDogTimer* compPtr = static_cast<VxWatchDogTimer*>(arg);

    // get time
    Os::RawTime time;
    time.now();

    // call output timing signal
    compPtr->CycleOut_out(0, time);

    // Start watchdog timer again
    FW_ASSERT(compPtr->m_watchdogId != nullptr);
    STATUS status = wdStart(compPtr->m_watchdogId, compPtr->m_tickDelay, reinterpret_cast<FUNCPTR>(s_driverISR),
                            reinterpret_cast<_Vx_usr_arg_t>(compPtr));
    FW_ASSERT(status == VXWORKS_OK);
}

}  // namespace Components
