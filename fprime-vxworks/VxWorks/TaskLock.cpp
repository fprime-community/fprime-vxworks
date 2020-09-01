// File: TaskLock.cpp
// Author: Nathan Serafin (nathan.serafin@jpl.nasa.gov)
// Date: 25 June, 2018
//
// VxWorks implementation of task switching lockout.
//
// This is not sufficient to ensure that you will never be preempted: a
// call to InterruptLock::lock() is also required. See intLock(2) for
// details.
//
// This is not callable from an interrupt context! Task rescheduling will
// not occur until after the ISR completes, so task locking is not
// necessary.  See intLock(2) for details.

#include <Os/TaskLock.hpp>

#include <taskLib.h>

namespace Os {
    I32 TaskLock::lock(void) {
        return static_cast<I32>(taskLock());
    }

    I32 TaskLock::unLock(void) {
        return static_cast<I32>(taskUnlock());
    }
}
