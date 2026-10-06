// ======================================================================
// \title VxWorks/Os/DefaultRawTime.cpp
// \brief sets default Os::RawTime VxWorks implementation via linker
// ======================================================================
#include <config/RawTimeSource.hpp>
#include "Os/Delegate.hpp"
#include "VxWorks/Os/RawTime.hpp"

namespace Os {

//! \brief get a delegate for RawTimeInterface that intercepts calls for VxWorks
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \param source: clock source selection, ignored as VxWorks exposes a single timestamp timer
//! \return: pointer to delegate
RawTimeInterface* RawTimeInterface::getDelegate(RawTimeHandleStorage& aligned_new_memory,
                                                const RawTimeInterface* to_copy,
                                                RawTimeSource source) {
    (void)source;
    return Os::Delegate::makeDelegate<RawTimeInterface, Os::VxWorks::RawTime::VxWorksRawTime, RawTimeHandleStorage>(
        aligned_new_memory, to_copy);
}

}  // namespace Os
