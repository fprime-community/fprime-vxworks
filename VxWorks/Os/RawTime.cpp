// ======================================================================
// \title VxWorks/Os/RawTime.cpp
// \brief VxWorks implementation for Os::RawTime
// ======================================================================
#include "VxWorks/Os/RawTime.hpp"

#include <sysLib.h>
#include <tickLib.h>
#include <vxWorks.h>

#include <atomic>

#include <Fw/Types/Assert.hpp>

namespace Os {
namespace VxWorks {
namespace RawTime {

static constexpr U32 NANOSECONDS_PER_SECOND = 1000000000;
//! Maximum number of re-reads performed by now() to obtain a consistent (rollovers, count) pair
static constexpr U32 MAX_READ_ATTEMPTS = 3;

//! State of the timestamp timer, shared by all RawTime instances
enum TimerState : int {
    TIMER_UNINITIALIZED,  //!< now() has not been called yet
    TIMER_INITIALIZING,   //!< A task is enabling the timestamp timer
    TIMER_READY,          //!< Timestamp timer is enabled
    TIMER_UNAVAILABLE     //!< BSP does not provide a usable timestamp timer
};

static std::atomic<int> s_timer_state(TIMER_UNINITIALIZED);
static std::atomic<U32> s_rollovers(0);   //!< Timestamp timer rollovers counted by rolloverIsr()
static bool s_use_tick_rollover = false;  //!< Use tickGet() as the rollover count instead of s_rollovers
static U32 s_period = 0;                  //!< Timestamp timer counts per rollover (sysTimestampPeriod())
static U32 s_frequency = 0;               //!< Timestamp timer counts per second (sysTimestampFreq())

//! \brief ISR connected via sysTimestampConnect(), invoked at each timestamp timer rollover
//! \return VXWORKS_OK, as required by the FUNCPTR signature
static int rolloverIsr(_Vx_usr_arg_t arg) {
    (void)arg;
    s_rollovers.fetch_add(1);
    return VXWORKS_OK;
}

//! \brief enable the timestamp timer once, on behalf of all RawTime instances
//! \return current timer state
static TimerState initializeTimer() {
    int expected = TIMER_UNINITIALIZED;
    if (s_timer_state.compare_exchange_strong(expected, TIMER_INITIALIZING)) {
        // sysTimestampConnect() returns ERROR when the timestamp timer is the system clock timer: its rollover
        // interrupt is the system clock interrupt, whose count is already available through tickGet()
        s_use_tick_rollover = (sysTimestampConnect(reinterpret_cast<FUNCPTR>(rolloverIsr), 0) != VXWORKS_OK);
        STATUS enabled = sysTimestampEnable();
        s_period = sysTimestampPeriod();
        s_frequency = sysTimestampFreq();
        TimerState state =
            ((enabled == VXWORKS_OK) && (s_period > 0) && (s_frequency > 0)) ? TIMER_READY : TIMER_UNAVAILABLE;
        s_timer_state.store(state);
    }
    return static_cast<TimerState>(s_timer_state.load());
}

//! \brief read the number of timestamp timer rollovers since the timer was enabled
static U32 readRollovers() {
    return s_use_tick_rollover ? static_cast<U32>(tickGet()) : s_rollovers.load();
}

VxWorksRawTime::Status VxWorksRawTime::now() {
    TimerState state = initializeTimer();
    if (state == TIMER_UNAVAILABLE) {
        return Status::NOT_SUPPORTED;
    } else if (state != TIMER_READY) {
        // Another task is enabling the timer; the caller may retry
        return Status::OTHER_ERROR;
    }

    // The counter may roll over between reading the rollover count and the counter. Re-read until the rollover
    // count is unchanged across the counter read. A rollover whose interrupt is still pending at the second read
    // cannot be detected and makes the sample early by one period.
    U32 rollovers = readRollovers();
    U32 count = sysTimestampLock();
    for (U32 attempt = 0; attempt < MAX_READ_ATTEMPTS; attempt++) {
        U32 rollovers_check = readRollovers();
        if (rollovers_check == rollovers) {
            break;
        }
        rollovers = rollovers_check;
        count = sysTimestampLock();
    }

    U64 total_counts = (static_cast<U64>(rollovers) * s_period) + count;
    this->m_handle.m_seconds = static_cast<U32>(total_counts / s_frequency);
    this->m_handle.m_nanoseconds =
        static_cast<U32>(((total_counts % s_frequency) * NANOSECONDS_PER_SECOND) / s_frequency);
    return Status::OP_OK;
}

VxWorksRawTime::Status VxWorksRawTime::getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& interval) const {
    // const_cast: RawTimeInterface::getHandle() is non-const; the handle is only read here
    const VxWorksRawTimeHandle* other_handle =
        static_cast<const VxWorksRawTimeHandle*>(const_cast<Os::RawTime&>(other).getHandle());
    FW_ASSERT(other_handle != nullptr);

    // Order the two times so that the subtraction below cannot underflow
    const VxWorksRawTimeHandle* later = &this->m_handle;
    const VxWorksRawTimeHandle* earlier = other_handle;
    if ((later->m_seconds < earlier->m_seconds) or
        ((later->m_seconds == earlier->m_seconds) and (later->m_nanoseconds < earlier->m_nanoseconds))) {
        later = other_handle;
        earlier = &this->m_handle;
    }

    U32 seconds = later->m_seconds - earlier->m_seconds;
    U32 nanoseconds = 0;
    if (later->m_nanoseconds < earlier->m_nanoseconds) {
        seconds -= 1;  // borrow one second for the nanosecond subtraction
        nanoseconds = later->m_nanoseconds + (NANOSECONDS_PER_SECOND - earlier->m_nanoseconds);
    } else {
        nanoseconds = later->m_nanoseconds - earlier->m_nanoseconds;
    }

    interval.set(seconds, nanoseconds / 1000);
    return Status::OP_OK;
}

Fw::SerializeStatus VxWorksRawTime::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    static_assert(VxWorksRawTime::SERIALIZED_SIZE >= 2 * sizeof(U32),
                  "VxWorksRawTime implementation requires at least 2*sizeof(U32) serialization size");
    Fw::SerializeStatus status = buffer.serializeFrom(this->m_handle.m_seconds, mode);
    if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        return status;
    }
    return buffer.serializeFrom(this->m_handle.m_nanoseconds, mode);
}

Fw::SerializeStatus VxWorksRawTime::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    static_assert(VxWorksRawTime::SERIALIZED_SIZE >= 2 * sizeof(U32),
                  "VxWorksRawTime implementation requires at least 2*sizeof(U32) serialization size");
    U32 seconds = 0;
    U32 nanoseconds = 0;
    Fw::SerializeStatus status = buffer.deserializeTo(seconds, mode);
    if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        return status;
    }
    status = buffer.deserializeTo(nanoseconds, mode);
    if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        return status;
    }
    this->m_handle.m_seconds = seconds;
    this->m_handle.m_nanoseconds = nanoseconds;
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

RawTimeHandle* VxWorksRawTime::getHandle() {
    return &this->m_handle;
}

}  // namespace RawTime
}  // namespace VxWorks
}  // namespace Os
