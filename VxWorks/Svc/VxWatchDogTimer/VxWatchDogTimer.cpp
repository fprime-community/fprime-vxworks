// ======================================================================
// \title  VxWatchDogTimer.cpp
// \author ortega
// \brief  cpp file for VxWatchDogTimer component implementation class
// ======================================================================

#include "VxWorks/Svc/VxWatchDogTimer/VxWatchDogTimer.hpp"
#include <sysLib.h>

namespace VxWorksSvc {

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

void VxWatchDogTimer::startTimer(FwSizeType milliseconds) {
    // Calculate ticks per interval by multiplying interval by number of ticks per second, then round up.
    // Make sure we do not overflow
    // ms * rate + (ms_p_sec - 1) <= max
    FW_ASSERT(milliseconds != 0);
    FW_ASSERT(sysClkRateGet() <= ((std::numeric_limits<_Vx_ticks_t>::max() - (MS_PER_SECS - 1)) / milliseconds),
              static_cast<FwAssertArgType>(sysClkRateGet()), static_cast<FwAssertArgType>(milliseconds));
    this->m_tickDelay = ((milliseconds * sysClkRateGet()) + (MS_PER_SECS - 1)) / MS_PER_SECS;
    this->startTimerTick(this->m_tickDelay);
}

void VxWatchDogTimer::startTimer(Fw::TimeInterval interval) {
    // Avoid overflow when converting seconds to milliseconds
    // seconds*ms_p_sec <= max
    // FW_ASSERT(interval.getSeconds() <= (std::numeric_limits<FwSizeType>::max() / MS_PER_SECS),
    FW_ASSERT(MS_PER_SECS <= (std::numeric_limits<FwSizeType>::max() / interval.getSeconds()),
              static_cast<FwAssertArgType>(interval.getSeconds()));
    // Avoid overflow when converting interval to milliseconds
    // seconds*ms_p_sec + usecs/us_p_ms <= max
    FW_ASSERT((interval.getSeconds() * MS_PER_SECS) <=
                  (std::numeric_limits<FwSizeType>::max() - (interval.getUSeconds() / USECS_PER_MS)),
              static_cast<FwAssertArgType>(interval.getSeconds()),
              static_cast<FwAssertArgType>(interval.getUSeconds()));
    FwSizeType milliseconds = (interval.getSeconds() * MS_PER_SECS) + (interval.getUSeconds() / USECS_PER_MS);
    this->startTimer(milliseconds);
}

void VxWatchDogTimer::startTimerTick(_Vx_ticks_t ticks) {
    FW_ASSERT(this->m_watchdogId != nullptr);
    STATUS status = wdStart(this->m_watchdogId, this->m_tickDelay, reinterpret_cast<FUNCPTR>(s_driverISR),
                            reinterpret_cast<_Vx_usr_arg_t>(this));
    FW_ASSERT(status == VXWORKS_OK);
}

void VxWatchDogTimer::quit() {
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
    compPtr->startTimerTick(compPtr->m_tickDelay);
}

}  // namespace VxWorksSvc
