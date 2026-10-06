// ======================================================================
// \title VxWorks/Os/RawTime.hpp
// \brief VxWorks definitions for Os::RawTime
// ======================================================================
#ifndef OS_VXWORKS_RAWTIME_HPP
#define OS_VXWORKS_RAWTIME_HPP

#include <Os/RawTime.hpp>

namespace Os {
namespace VxWorks {
namespace RawTime {

struct VxWorksRawTimeHandle : public RawTimeHandle {
    U32 m_seconds = 0;      //!< Seconds elapsed since the timestamp timer was enabled
    U32 m_nanoseconds = 0;  //!< Nanoseconds within m_seconds
};

//! \brief VxWorks implementation of Os::RawTime
//!
//! VxWorks implementation of `RawTimeInterface` built on the BSP timestamp timer (`sysTimestampLock()`,
//! `INCLUDE_TIMESTAMP`). The timestamp counter wraps every `sysTimestampPeriod()` counts, so this implementation
//! counts rollovers through `sysTimestampConnect()` and combines them with the counter to produce a monotonic
//! (seconds, nanoseconds) pair. When the BSP refuses `sysTimestampConnect()` the timestamp timer is the system clock
//! timer and `tickGet()` is used as the rollover count instead.
//!
//! The timestamp timer is enabled on the first call to `now()`. The `RawTimeSource` selection is ignored: VxWorks
//! exposes a single timestamp timer.
//!
class VxWorksRawTime : public RawTimeInterface {
  public:
    //! \brief constructor
    //!
    VxWorksRawTime() = default;

    //! \brief destructor
    //!
    ~VxWorksRawTime() override = default;

    //! \brief return the underlying RawTime handle (implementation specific)
    //! \return internal RawTime handle representation
    RawTimeHandle* getHandle() override;

    // ------------------------------------------------------------
    // Implementation-specific RawTime overrides
    // ------------------------------------------------------------
    //! \brief Get the current time.
    //!
    //! Reads the timestamp timer and stores the elapsed time in the RawTime object. The first call enables the
    //! timestamp timer.
    //!
    //! \return OP_OK on success, NOT_SUPPORTED if the BSP does not provide a usable timestamp timer.
    Status now() override;

    //! \brief Calculate the time interval between this and another raw time.
    //!
    //! \param other The other RawTime to compare against.
    //! \param interval Output parameter to store the calculated time interval.
    //! \return Status indicating the result of the operation.
    Status getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& interval) const override;

    //! \brief Serialize the contents of the RawTimeInterface object into a buffer.
    //!
    //! Serializes the (seconds, nanoseconds) pair as two U32 values, requiring `FW_RAW_TIME_SERIALIZATION_MAX_SIZE`
    //! to be at least 8 bytes.
    //!
    //! \param buffer The buffer to serialize the contents into.
    //! \param mode Endianness to use when serializing to buffer.
    //! \return Fw::SerializeStatus indicating the result of the serialization.
    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer,
                                    Fw::Endianness mode = Fw::Endianness::BIG) const override;

    //! \brief Deserialize the contents of the RawTimeInterface object from a buffer.
    //!
    //! \param buffer The buffer to deserialize the contents from.
    //! \param mode Endianness to use when deserializing from the buffer.
    //! \return Fw::SerializeStatus indicating the result of the deserialization.
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer,
                                        Fw::Endianness mode = Fw::Endianness::BIG) override;

  private:
    //! Handle for VxWorksRawTime
    VxWorksRawTimeHandle m_handle;
};

}  // namespace RawTime
}  // namespace VxWorks
}  // namespace Os
#endif  // OS_VXWORKS_RAWTIME_HPP
