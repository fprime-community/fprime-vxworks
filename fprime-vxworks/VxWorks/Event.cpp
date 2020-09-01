// File: Event.cpp
// Author: Nathan Serafin (nathan.serafin@jpl.nasa.gov)
// Date: 27 July, 2018
//
// VxWorks implementation of events.

#include <Os/Event.hpp>

#include <eventLib.h>

namespace Os
{
    I32 Event::send(const TaskId& tid, const U32 events)
    {
        return eventSend(tid.getRepr(), events);
    }

    I32 Event::receive(const U32 events, const U8 options,
                       const I32 timeout, U32* eventsReceived)
    {
        return eventReceive(events, options, timeout,
                            reinterpret_cast<UINT32*>(eventsReceived));
    }

    I32 Event::clear(void)
    {
        return eventClear();
    }
}
