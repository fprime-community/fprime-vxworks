// ======================================================================
// \title  VxWatchDogTimer.cpp
// \author ortega
// \brief  cpp file for VxWatchDogTimer component implementation class
// ======================================================================

#include "Components/VxWatchDogTimer/VxWatchDogTimer.hpp"

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

void VxWatchDogTimer::callIsr(U32 ticks) {
    this->m_tickDelay = ticks;
    s_driverISR(this);
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
    FW_ASSERT(compPtr->m_watchdogId != nullptr);
    STATUS status = wdStart(compPtr->m_watchdogId, compPtr->m_tickDelay, reinterpret_cast<FUNCPTR>(s_driverISR),
                            reinterpret_cast<_Vx_usr_arg_t>(compPtr));
    FW_ASSERT(status == VXWORKS_OK);
}

}  // namespace Components
