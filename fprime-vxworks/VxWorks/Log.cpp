#include <Os/Log.hpp>

#include <vxWorks.h>
#include <logLib.h>
#include <string.h>

namespace Os {
    Log::Log() {

        // Register myself as a logger at construction time. If used in unison with LogDefault.cpp, this will
        // automatically create this as a default logger.
        this->registerLogger(this);
    }


    static char logBuffers[FW_LOG_TEXT_BUFFER_DEPTH][FW_LOG_TEXT_BUFFER_SIZE];
    static NATIVE_INT_TYPE logEntry = 0;

    // Instance implementation
    void Log::log(
        const char* fmt,
        POINTER_CAST a0,
        POINTER_CAST a1,
        POINTER_CAST a2,
        POINTER_CAST a3,
        POINTER_CAST a4,
        POINTER_CAST a5,
        POINTER_CAST a6,
        POINTER_CAST a7,
        POINTER_CAST a8,
        POINTER_CAST a9
    ) {

        // copy the possibly transient string to a circular buffer
        // this relies on the VxWorks log task to keep up with
        // the circular buffer

        strncpy(logBuffers[logEntry],fmt,FW_LOG_TEXT_BUFFER_SIZE);
        // null terminate
        logBuffers[logEntry][FW_LOG_TEXT_BUFFER_SIZE - 1] = 0;
        ::logMsg(logBuffers[logEntry], a1, a2, a3, a4, a5, a6);
        logEntry = (logEntry + 1)%FW_LOG_TEXT_BUFFER_DEPTH;
    }
}
