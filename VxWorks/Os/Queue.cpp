#include <Os/Queue.hpp>

#include <vxWorks.h>
#include <msgQLib.h>

#include <stdio.h>
#include <string.h>
#include <logLib.h>

namespace Os {
    
    Queue::Queue(): m_handle(0) {
        
    }

    Queue::QueueStatus Queue::createInternal(const Fw::StringBase &name, NATIVE_INT_TYPE depth, NATIVE_INT_TYPE msgSize) {

        this->m_name = "QV_";
        this->m_name += name;

        MSG_Q_ID handle = msgQCreate(depth,msgSize,MSG_Q_PRIORITY);
        this->m_handle = handle == NULL?0:(POINTER_CAST)handle;
        if (NULL == handle) {
            return QUEUE_UNINITIALIZED;
        }
        Queue::s_numQueues++;
        return QUEUE_OK;
    }
    
    Queue::~Queue() {
        (void)msgQDelete((MSG_Q_ID) this->m_handle);
    }

    Queue::QueueStatus Queue::send(const U8* buffer, NATIVE_INT_TYPE size, NATIVE_INT_TYPE priority, QueueBlocking block) {

        if (0 == this->m_handle) {
            return QUEUE_UNINITIALIZED;
        }
        
        if (NULL == buffer) {
            return QUEUE_EMPTY_BUFFER;
        }
        
        NATIVE_INT_TYPE vxPrio = MSG_PRI_NORMAL;
        if (priority > 0) {
            vxPrio = MSG_PRI_URGENT;
        }
        
        STATUS stat = msgQSend((MSG_Q_ID) this->m_handle, (char*) buffer, size, (QUEUE_NONBLOCKING == block)?NO_WAIT:WAIT_FOREVER, vxPrio);
        
        if (ERROR == stat) {
            switch (errno) {
                case S_msgQLib_INVALID_MSG_LENGTH:
                    return QUEUE_SIZE_MISMATCH;
                case S_objLib_OBJ_UNAVAILABLE:
                case S_msgQLib_NON_ZERO_TIMEOUT_AT_INT_LEVEL:
                    return QUEUE_FULL;
                default:
                    logMsg("Queue send error %s! %d\n",(int)strerror(errno),(int)this->m_handle,0,0,0,0);
                    return QUEUE_UNKNOWN_ERROR;
            }
        } else {
            return QUEUE_OK;
        }
    }

    Queue::QueueStatus Queue::receive(U8* buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE &actualSize, NATIVE_INT_TYPE &priority, QueueBlocking block) {
        
        if (0 == this->m_handle) {
            return QUEUE_UNINITIALIZED;
        }

        actualSize = msgQReceive((MSG_Q_ID) this->m_handle, (char*)buffer, capacity, (QUEUE_NONBLOCKING == block)?NO_WAIT:WAIT_FOREVER);
        
        if (ERROR == actualSize) {
            actualSize = 0;
            switch (errno) {
                case S_msgQLib_INVALID_MSG_LENGTH:
                    return QUEUE_SIZE_MISMATCH;
                case S_objLib_OBJ_UNAVAILABLE:
                    return QUEUE_NO_MORE_MSGS;
                default:
                    logMsg("Queue receive error %s! %d %d\n",(int)strerror(errno),(int)this->m_handle,(int)block,0,0,0);
                    return QUEUE_UNKNOWN_ERROR;
            }
        } else {
            return QUEUE_OK;
        }
    }

    NATIVE_INT_TYPE Queue::getNumMsgs(void) const {
        return msgQNumMsgs((MSG_Q_ID) this->m_handle);
    }

}

