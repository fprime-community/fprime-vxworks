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

void VxWatchDogTimer::startTimer(FwSizeType microseconds) {
    FW_ASSERT(microseconds != 0);
    // Avoid overflow
    // usecs * clkRate + (us_p_s - 1) <= max
    // clkRate <= (max - (us_p_s - 1)) / usecs
    FW_ASSERT(sysClkRateGet() <= (std::numeric_limits<_Vx_ticks_t>::max() - (USECS_PER_SECS - 1)) / microseconds, static_cast<FwAssertArgType>(sysClkRateGet()), static_cast<FwAssertArgType>(microseconds));
    this->m_tickDelay = ((microseconds * sysClkRateGet()) + (USECS_PER_SECS - 1)) / USECS_PER_SECS;
    this->startTimerTick(this->m_tickDelay);
}

void VxWatchDogTimer::startTimer(Fw::TimeInterval interval) {
    // Avoid overflow when converting interval to microseconds
    // (seconds * us_p_ms * ms_p_s) + usecs <= max
    // seconds <= (max - usecs) / (us_p_ms * ms_p_s)
    FW_ASSERT(interval.getSeconds() <= (std::numeric_limits<FwSizeType>::max() - interval.getUSeconds()) / USECS_PER_SECS, static_cast<FwAssertArgType>(interval.getSeconds()), static_cast<FwAssertArgType>(interval.getUSeconds()));
    FwSizeType microseconds = (interval.getSeconds() * USECS_PER_SECS) + interval.getUSeconds();
    this->startTimer(microseconds);
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
