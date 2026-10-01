# VxWorksDrv::VxWorksUartDriver

## 1. Introduction

The VxWorksUartDriver component provides a VxWorks implementation of a UART (Universal Asynchronous Receiver-Transmitter) serial communication driver. It implements the byte stream driver model interface (see [`Drv.ByteStreamDriver`](https://github.com/nasa/fprime/blob/devel/Drv/Interfaces/ByteStreamDriver.fpp)) to enable serial communication with external devices through serial (`tty`) devices on VxWorks systems.

The component wraps the VxWorks I/O system (`open`/`read`/`write`/`select` plus the `sioLib` and `ioLib` `ioctl` requests) to provide configurable serial communication with support for several baud rates, hardware flow control, and parity settings. It implements bidirectional communication using a dedicated receive task and synchronous send operations.

It is the VxWorks counterpart of F´'s [`Drv::PosixUartDriver`](https://github.com/nasa/fprime/blob/devel/Drv/PosixUartDriver/docs/sdd.md), which configures the device through `termios` instead.

For more information on the ByteStreamDriverModel see: [`Drv::ByteStreamDriverModel`](https://github.com/nasa/fprime/blob/devel/Drv/ByteStreamDriverModel/docs/sdd.md).

## 2. Requirements

| Name | Description | Validation |
|---|---|---|
| VXWORKS-UART-COMP-001 | The VxWorksUartDriver component shall implement the Drv.ByteStreamDriver interface | inspection |
| VXWORKS-UART-COMP-002 | The VxWorksUartDriver component shall provide configurable baud rates from 9600 to 921600 | inspection |
| VXWORKS-UART-COMP-003 | The VxWorksUartDriver component shall provide configurable flow control (none/hardware) | inspection |
| VXWORKS-UART-COMP-004 | The VxWorksUartDriver component shall provide configurable parity (none/odd/even) | inspection |
| VXWORKS-UART-COMP-005 | The VxWorksUartDriver component shall provide a dedicated read task for receiving data | inspection |
| VXWORKS-UART-COMP-006 | The VxWorksUartDriver component shall report telemetry for bytes sent and received | inspection |
| VXWORKS-UART-COMP-007 | The VxWorksUartDriver component shall handle UART errors and report them via events | inspection |
| VXWORKS-UART-COMP-008 | The VxWorksUartDriver component shall allocate receive buffers through the `allocate` port and release them through the `deallocate` port | inspection |

## 3. Design

The VxWorksUartDriver component implements the design specified by the [`Drv.ByteStreamDriver`](https://github.com/nasa/fprime/blob/devel/Drv/Interfaces/ByteStreamDriver.fpp) interface. It is a passive component; the only thread of execution it owns is the receive task started by `start()`.

### 3.1 Architecture

The component consists of the following key elements:

- **UART Configuration**: Opens the device with `open(device, O_RDWR)` and configures it with VxWorks `ioctl` requests (`SIO_BAUD_SET`, `SIO_HW_OPTS_SET`, `FIOSETOPTIONS`)
- **Send Handler**: Synchronous transmission of data via the `send` port (guarded input port)
- **Receive Task**: Asynchronous reception of data via a dedicated task that calls the `recv` output port
- **Buffer Management**: Receive buffers are obtained from the `allocate` port and returned through the `deallocate` port when they come back on `recvReturnIn`
- **Telemetry Reporting**: Tracks and reports bytes sent and received statistics on the `run` port
- **Error Handling**: Error detection and event reporting for open, configuration, read, write, and buffer allocation failures

### 3.2 Device Configuration

`open()` performs the following steps; any failure closes the file descriptor, emits `OpenError` with the `errno` string, and returns `false`:

1. `::open(device, O_RDWR, 0)`
2. `ioctl(SIO_BAUD_SET, baud)` sets the baud rate
3. `ioctl(SIO_HW_OPTS_SET, CS8 | CREAD | CLOCAL [| PARENB [| PARODD]])` sets 8 data bits, 1 stop bit, and the requested parity
4. If hardware flow control is requested, `ioctl(SIO_HW_OPTS_SET, ... | CRTSCTS)`
5. `ioctl(FIOSETOPTIONS, OPT_RAW)` puts the tty in raw (non-canonical) mode

On success the component emits `PortOpened` and, if connected, invokes the `ready` output port.

### 3.3 Send Operation

When data is sent via the `send` input port:

1. The component checks that the device is open and that the buffer is valid (`Fw::Buffer::isValid()`); otherwise `OTHER_ERROR` is returned
2. Data is written to the UART device using `write()`
3. A short write or a write error emits `WriteError` (throttled) and returns `OTHER_ERROR`
4. On success the bytes-sent counter is updated and `OP_OK` is returned

Ownership of the buffer stays with the caller in all cases.

### 3.4 Receive Operation

The receive operation runs in the dedicated receive task (`serialReadTaskEntry`) and loops until `quitReadThread()` is called. Each iteration:

1. A buffer of `allocationSize` bytes is requested from the `allocate` port. If no buffer is available, `NoBuffers` is emitted (throttled), the empty buffer is forwarded on `recv` with `OTHER_ERROR`, the task sleeps 50 ms, and the loop restarts
2. The task waits on `select()` with a 1 second timeout for the device to become readable
3. If data is available, `read()` fills the buffer; the buffer size is set to the number of bytes read, the bytes-received counter is updated, and the buffer is sent on `recv` with `OP_OK`
4. On a `select()`/`read()` error, `ReadError` is emitted (throttled) and the buffer is sent on `recv` with size 0 and `OTHER_ERROR`
5. On a `select()` timeout, the buffer is sent on `recv` with size 0 and `OTHER_ERROR`

Buffers sent out on `recv` are returned to the driver through `recvReturnIn` and released via the `deallocate` port.

### 3.5 Threading Model

The component uses a single dedicated task for receive operations. This task:

- Is created by `start()` with the component instance name as the task name
- Runs continuously until `quitReadThread()` is called; the 1 second `select()` timeout bounds how long the task takes to observe the quit flag
- Can be started with configurable priority, stack size, and CPU affinity
- Can be joined with `join()`

The byte counters are `std::atomic` so that `run_handler` (rate group thread) and the receive task can update and read them without additional locking.

## 4. Usage

The VxWorksUartDriver must be configured with device parameters before use. The typical usage pattern is:

1. **Open Device**: Configure the UART device with desired parameters
2. **Start Receive Task**: Begin the receive task for incoming data
3. **Send/Receive Data**: Use the ByteStreamDriver ports for communication
4. **Shutdown**: Stop the receive task and join it

`open()` must be called before `start()`, since the receive task reads from the file descriptor established by `open()`.

### 4.1 Configuration Example

The VxWorksUartDriver should be instantiated in the FPP topology and configured using separate functions following F´ patterns:

```cpp
// Configuration function - called during topology setup
void configureTopology() {
    // Open UART device with configuration
    bool success = uart.open("/tyCo/1",                                 // Device path
                             VxWorksDrv::VxWorksUartDriver::BAUD_115K,   // 115200 baud rate
                             VxWorksDrv::VxWorksUartDriver::NO_FLOW,     // No flow control
                             VxWorksDrv::VxWorksUartDriver::PARITY_NONE, // No parity
                             1024);                                      // Receive buffer size
    if (!success) {
        // Handle configuration error
    }
    ...
}

// Startup function - called when starting tasks
void setupTopology() {
    // Start receive task
    uart.start(UART_PRIORITY,           // Task priority
               32 * 1024,               // Task stack size
               Os::Task::TASK_DEFAULT); // Task CPU affinity mask
}

// Shutdown function - called during teardown
void teardownTopology() {
    uart.quitReadThread();
    uart.join();
}
```

### 4.2 Integration with Rate Groups

The component includes a `run` input port for telemetry reporting that should be connected to a rate group in the FPP topology:

```fpp
# In topology.fpp connections section
connections RateGroups {
  # Connect UART driver to rate group for telemetry
  rateGroup1Comp.RateGroupMemberOut[N] -> uart.run
}
```

### 4.3 Buffer Management

The `allocate` and `deallocate` ports must be connected to a buffer manager (e.g. `Svc.BufferManager`) that can serve buffers of at least `allocationSize` bytes:

```fpp
uart.allocate -> bufferManager.bufferGetCallee
uart.deallocate -> bufferManager.bufferSendIn
```

## 5. Configuration

### 5.1 Device Parameters

| Parameter | Type | Description | Valid Values |
|-----------|------|-------------|--------------|
| device | const char* | Path to UART device | VxWorks tty device path (e.g., "/tyCo/1") |
| baud | VxWorksDrv::VxWorksUartDriver::UartBaudRate | Communication baud rate | See baud rate enumeration |
| fc | VxWorksDrv::VxWorksUartDriver::UartFlowControl | Flow control setting | NO_FLOW, HW_FLOW |
| parity | VxWorksDrv::VxWorksUartDriver::UartParity | Parity setting | PARITY_NONE, PARITY_ODD, PARITY_EVEN |
| allocationSize | FwSizeType | Receive buffer size requested from the `allocate` port | Positive integer (bytes) |

### 5.2 Baud Rate Options

The enumeration values are the numeric baud rates passed directly to `SIO_BAUD_SET`; whether a given rate is accepted depends on the serial driver of the board support package.

| Enumeration | Numeric Value |
|-------------|---------------|
| BAUD_9600 | 9600 |
| BAUD_19200 | 19200 |
| BAUD_38400 | 38400 |
| BAUD_57600 | 57600 |
| BAUD_115K | 115200 |
| BAUD_230K | 230400 |
| BAUD_460K | 460800 |
| BAUD_921K | 921600 |

### 5.3 Task Configuration

The receive task can be configured with:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| priority | FwTaskPriorityType | TASK_PRIORITY_DEFAULT | Task priority |
| stackSize | Os::Task::ParamType | TASK_DEFAULT | Task stack size |
| cpuAffinity | Os::Task::ParamType | TASK_DEFAULT | CPU affinity mask |

### 5.4 Events and Telemetry

The component generates the following events:

- **OpenError**: UART device open or configuration (`ioctl`) failures, with the `errno` string
- **WriteError**: Data transmission errors (throttled)
- **ReadError**: Data reception errors (throttled)
- **PortOpened**: Successful device configuration
- **NoBuffers**: Buffer allocation failures (throttled)

`ConfigError` and `BufferTooSmall` are declared in the model for interface compatibility with `Drv::PosixUartDriver` but are not emitted by this implementation; configuration failures are reported through `OpenError`.

The component reports the following telemetry:

- **BytesSent**: Total bytes transmitted
- **BytesRecv**: Total bytes received
