// ======================================================================
// \title  VxWorksUartDriverImpl.cpp
// \author tcanham
// \brief  cpp file for VxWorksUartDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <VxWorks/Drv/VxWorksUartDriver/VxWorksUartDriver.hpp>
#include <Os/TaskString.hpp>

#include "Fw/Types/BasicTypes.hpp"

#include <fcntl.h>
#include <ioLib.h>
#include <sioLib.h>
#include <termios.h>
#include <selectLib.h>
#include <cerrno>
#include <cstring>

namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

VxWorksUartDriver ::VxWorksUartDriver(const char* const compName)
    : VxWorksUartDriverComponentBase(compName),
      m_fd(-1),
      m_allocationSize(0),
      m_device("NOT_EXIST"),
      m_bytesSent(0),
      m_bytesReceived(0),
      m_quitReadThread(false) {}

bool VxWorksUartDriver::open(const char* const device,
                              UartBaudRate baud,
                              UartFlowControl fc,
                              UartParity parity,
                              FwSizeType allocationSize) {
    FW_ASSERT(device != nullptr);
    int fd = -1;
    int stat = -1;
    this->m_allocationSize = allocationSize;

    this->m_device = device;

    fd = ::open(device, O_RDWR, 0);

    if (fd == -1) {
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, this->m_fd, _err);
        return false;
    }

    this->m_fd = fd;

    // Set baud rate using ioctl
    stat = ::ioctl(fd, SIO_BAUD_SET, static_cast<int>(baud));
    if (stat == -1) {
        ::close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }

    // Set hardware options: 8 data bits, 1 stop bit
    int hwOptions = CS8 | CREAD | CLOCAL;

    // Set parity
    switch (parity) {
        case PARITY_ODD:
            hwOptions |= (PARENB | PARODD);
            break;
        case PARITY_EVEN:
            hwOptions |= PARENB;
            break;
        case PARITY_NONE:
            break;
        default:
            FW_ASSERT(0, parity);
            break;
    }

    stat = ::ioctl(fd, SIO_HW_OPTS_SET, hwOptions);
    if (stat == -1) {
        ::close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }

    // Set flow control
    if (fc == HW_FLOW) {
        stat = ::ioctl(fd, SIO_HW_OPTS_SET, hwOptions | CRTSCTS);
        if (stat == -1) {
            ::close(fd);
            Fw::LogStringArg _arg = device;
            Fw::LogStringArg _err = strerror(errno);
            this->log_WARNING_HI_OpenError(_arg, fd, _err);
            return false;
        }
    }

    // Set to raw mode (non-canonical)
    stat = ::ioctl(fd, FIOSETOPTIONS, OPT_RAW);
    if (stat == -1) {
        ::close(fd);
        Fw::LogStringArg _arg = device;
        Fw::LogStringArg _err = strerror(errno);
        this->log_WARNING_HI_OpenError(_arg, fd, _err);
        return false;
    }

    // All done!
    Fw::LogStringArg _arg = device;
    this->log_ACTIVITY_HI_PortOpened(_arg);
    if (this->isConnected_ready_OutputPort(0)) {
        this->ready_out(0);
    }
    return true;
}

VxWorksUartDriver ::~VxWorksUartDriver() {
    if (this->m_fd != -1) {
        (void)::close(this->m_fd);
    }
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void VxWorksUartDriver ::run_handler(FwIndexType portNum, U32 context) {
    this->tlmWrite_BytesSent(this->m_bytesSent);
    this->tlmWrite_BytesRecv(this->m_bytesReceived);
}

Drv::ByteStreamStatus VxWorksUartDriver ::send_handler(const FwIndexType portNum, Fw::Buffer& serBuffer) {
    Drv::ByteStreamStatus status = Drv::ByteStreamStatus::OP_OK;
    if (this->m_fd == -1 || serBuffer.getData() == nullptr || serBuffer.getSize() == 0) {
        status = Drv::ByteStreamStatus::OTHER_ERROR;
    } else {
        unsigned char* data = serBuffer.getData();
        FW_ASSERT_NO_OVERFLOW(serBuffer.getSize(), size_t);
        size_t xferSize = static_cast<size_t>(serBuffer.getSize());

        ssize_t stat = ::write(this->m_fd, data, xferSize);

        if (-1 == stat || static_cast<size_t>(stat) != xferSize) {
            Fw::LogStringArg _arg = this->m_device;
            this->log_WARNING_HI_WriteError(_arg, static_cast<I32>(stat));
            status = Drv::ByteStreamStatus::OTHER_ERROR;
        } else {
            this->m_bytesSent += static_cast<FwSizeType>(stat);
        }
    }
    return status;
}

void VxWorksUartDriver::recvReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    this->deallocate_out(0, fwBuffer);
}

void VxWorksUartDriver ::serialReadTaskEntry(void* ptr) {
    FW_ASSERT(ptr != nullptr);
    Drv::ByteStreamStatus status = ByteStreamStatus::OTHER_ERROR;
    VxWorksUartDriver* comp = reinterpret_cast<VxWorksUartDriver*>(ptr);
    while (!comp->m_quitReadThread) {
        Fw::Buffer buff = comp->allocate_out(0, comp->m_allocationSize);

        if (buff.getData() == nullptr) {
            Fw::LogStringArg _arg = comp->m_device;
            comp->log_WARNING_HI_NoBuffers(_arg);
            status = ByteStreamStatus::OTHER_ERROR;
            comp->recv_out(0, buff, status);
            Os::Task::delay(Fw::TimeInterval(0, 50000));
            continue;
        }

        int stat = 0;

        FW_ASSERT_NO_OVERFLOW(buff.getSize(), size_t);

        // Use select to implement timeout
        fd_set readfds;
        struct timeval timeout;

        FD_ZERO(&readfds);
        FD_SET(comp->m_fd, &readfds);

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int selectRet = ::select(comp->m_fd + 1, &readfds, NULL, NULL, &timeout);

        if (selectRet > 0 && FD_ISSET(comp->m_fd, &readfds)) {
            stat = static_cast<int>(::read(comp->m_fd, buff.getData(), static_cast<size_t>(buff.getSize())));
        } else if (selectRet == 0) {
            stat = 0;
        } else {
            stat = -1;
        }

        buff.setSize(0);

        if (stat == -1) {
            Fw::LogStringArg _arg = comp->m_device;
            comp->log_WARNING_HI_ReadError(_arg, stat);
            status = ByteStreamStatus::OTHER_ERROR;
        } else if (stat > 0) {
            buff.setSize(static_cast<U32>(stat));
            status = ByteStreamStatus::OP_OK;
            comp->m_bytesReceived += static_cast<FwSizeType>(stat);
        } else {
            status = ByteStreamStatus::OTHER_ERROR;
        }

        comp->recv_out(0, buff, status);
    }
}

void VxWorksUartDriver ::start(FwTaskPriorityType priority,
                                Os::Task::ParamType stackSize,
                                Os::Task::ParamType cpuAffinity) {
    Os::TaskString task;
    task.format("VUD_%s", this->m_device);
    Os::Task::Arguments arguments(task, serialReadTaskEntry, this, priority, stackSize, cpuAffinity);
    Os::Task::Status stat = this->m_readTask.start(arguments);
    FW_ASSERT(stat == Os::Task::OP_OK, stat);
}

void VxWorksUartDriver ::quitReadThread() {
    this->m_quitReadThread = true;
}

Os::Task::Status VxWorksUartDriver ::join() {
    return m_readTask.join();
}

}  // end namespace Drv
