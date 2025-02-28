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
    m_watchdogId = wdCreate();
}

VxWatchDogTimer ::~VxWatchDogTimer() {
    if (m_watchdogId != nullptr) {
        (void)wdCancel(m_watchdogId);
        (void)wdDelete(m_watchdogId);
    }
}

void VxWatchDogTimer::callIsr(U32 ticks) {
    this->m_tickDelay = ticks;
    s_driverISR(this);
}

void VxWatchDogTimer::s_driverISR(void* arg) {
    FW_ASSERT(arg);
    // cast argument to component instance
    VxWatchDogTimer* compPtr = static_cast<VxWatchDogTimer*>(arg);
    compPtr->InterruptReport_internalInterfaceInvoke(0);
    FW_ASSERT(compPtr->m_watchdogId != nullptr);
    STATUS status = wdStart(compPtr->m_watchdogId, compPtr->m_tickDelay, reinterpret_cast<FUNCPTR>(s_driverISR),
                            reinterpret_cast<_Vx_usr_arg_t>(compPtr));
    FW_ASSERT(status == VXWORKS_OK);
}
// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void VxWatchDogTimer ::BufferIn_handler(FwIndexType portNum, Drv::DataBuffer& buff) {
    // just a pass-through
    this->BufferOut_out(0, buff);
}

void VxWatchDogTimer ::PingIn_handler(FwIndexType portNum, U32 key) {
    // call ping output port
    this->PingOut_out(0, key);
}

void VxWatchDogTimer ::Sched_handler(FwIndexType portNum, U32 context) {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined internal interfaces
// ----------------------------------------------------------------------

void VxWatchDogTimer ::InterruptReport_internalInterfaceHandler(U32 interrupt) {
    // get time
    Os::RawTime time;
    time.now();
    // call output timing signal
    this->CycleOut_out(0, time);
    // increment cycles and write channel
    this->tlmWrite_Cycles(this->m_cycles);
    this->m_cycles++;
}

}  // namespace Components
