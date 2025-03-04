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
    this->m_tickDelay = static_cast<_Vx_ticks_t>(ticks);
    FW_ASSERT(this->m_watchdogId != nullptr);
    STATUS status = wdStart(this->m_watchdogId, this->m_tickDelay, reinterpret_cast<FUNCPTR>(s_driverISR),
                            reinterpret_cast<_Vx_usr_arg_t>(this));
    FW_ASSERT(status == VXWORKS_OK);
}

void VxWatchDogTimer::startWatchdog(Fw::TimeInterval interval) {
    static constexpr U32 MS_PER_SECS = 1000;
    static constexpr U32 USECS_PER_MS = 1000;
    U64 delayInMs = (interval.getSeconds() * MS_PER_SECS) + (interval.getUSeconds() * USECS_PER_MS);
    FW_ASSERT(delayInMs <= std::numeric_limits<U32>::max(), static_cast<FwAssertArgType>(delayInMs));
    // Calculate ticks per interval by multiplying interval by number of ticks per second, then round up.
    U64 ticksPerInterval = (((delayInMs * sysClkRateGet()) + (MS_PER_SECS - 1)) / MS_PER_SECS);
    FW_ASSERT(ticksPerInterval <= std::numeric_limits<U32>::max(), static_cast<FwAssertArgType>(ticksPerInterval));
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
    compPtr->startWatchdog(compPtr->m_tickDelay);
}

}  // namespace Components
