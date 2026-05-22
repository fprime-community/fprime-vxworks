# VxWorks PriorityMemQueue Implementation

## Os::PriorityMemQueue

Os::PriorityMemQueue is an ISR-safe and SMP-safe, priority-based memory queue implementation for F´ using VxWorks message queues (msgQ). This implementation leverages the VxWorks msgQ API to provide robust multi-core synchronization without the need for a critical section lock, reducing complexity (at the cost of portability).

The key components are:

1. **VxWorks msgQ** - One msgQ per priority level for ISR-safe + SMP-safe message storage
2. **Atomic Priority Tracking** - Lock-free bitmasks to track which priorities have messages
3. **Counting Semaphore** - Notification mechanism for blocking receive operations
4. **PriorityMemQueue** - Main queue implementation that implements Os::QueueInterface

### Requirements

The requirements for `Os::Generic::PriorityMemQueue` are as follows:

Requirement | Description | Verification Method
----------- | ----------- | -------------------
PMQ-001 | The PriorityMemQueue shall implement the Os::QueueInterface for compatibility with F´ components | Inspection, Unit Test
PMQ-002 | The PriorityMemQueue shall support up to 16 priority levels | Inspection, Unit Test
PMQ-003 | The PriorityMemQueue shall allocate separate memory pools for each priority level | Inspection, Unit Test
PMQ-004 | The PriorityMemQueue shall support per-priority configuration of message size and depth | Inspection, Unit Test
PMQ-005 | The PriorityMemQueue shall support blocking and non-blocking send operations | Unit Test
PMQ-006 | The PriorityMemQueue shall support blocking and non-blocking receive operations | Unit Test
PMQ-007 | The PriorityMemQueue shall dequeue messages in priority order, with the highest enabled priority serviced first | Unit Test
PMQ-008 | The PriorityMemQueue shall support dynamic enable/disable of individual priority levels | Unit Test
PMQ-009 | The PriorityMemQueue shall be configurable to be ISR-safe for message enqueue and dequeue operations | Platform dependent
PMQ-010 | The PriorityMemQueue shall track high water marks for message queue depth | Unit Test
PMQ-011 | The PriorityMemQueue shall validate message sizes against priority-specific limits | Unit Test
PMQ-012 | The PriorityMemQueue shall use the F´ memory allocator registry for all dynamic allocations | Inspection
PMQ-013 | The PriorityMemQueue shall return appropriate error codes for queue full, empty, and invalid priority conditions | Unit Test
PMQ-014 | The PriorityMemQueue shall provide per-priority O(1) enqueue and dequeue operations | Inspection

> [!WARNING]
> Send/receive operations from ISR context must not use blocking behavior 

> [!NOTE]
> Os::PriorityMemQueue provides ISR safety and per-priority configuration at the cost of increased complexity and memory usage compared to Os::PriorityQueue.

### VxWorks msgQ-Based Architecture

#### Overview

`PriorityMemQueue` uses VxWorks message queues (`msgQLib`) for message storage and synchronization. This approach leverages Wind River's platform-optimized queue implementation that provides:
- **ISR Safety**: `msgQSend` can be called from interrupt context
- **SMP Safety**: Atomic operations prevent multi-core race conditions
- **No Interrupt Starvation**: msgQ uses task-level synchronization internally, not global interrupt disable
- **Platform Optimization**: Wind River has tuned msgQ performance for VxWorks SMP systems

#### Per-Priority Message Queues

Each priority level has its own dedicated VxWorks message queue created with `msgQCreate()`:

```cpp
MSG_Q_ID msgQ = msgQCreate(numMsgs, maxMsgSize, MSG_Q_FIFO);
```

**Parameters**:
- `numMsgs`: Maximum number of messages for this priority
- `maxMsgSize`: Maximum message size in bytes
- `MSG_Q_FIFO`: FIFO ordering within the priority level

**Storage** (Dynamically Allocated):

PriorityMemQueue dynamically allocates arrays for tracking per-queue max msg size, number of message buffers, and msgQ state. 

#### Priority Tracking with Atomics

To implement priority-based dequeue ordering, `PriorityMemQueue` maintains lock-free atomic bitmasks:

```cpp
std::atomic<U32> m_priorityMask;       // Which priorities are enabled (bit per priority)
std::atomic<U32> m_prioritiesWithMsg;  // Which priorities have messages
```

**Send Flow**:
1. Call `msgQSend()` - VxWorks handles synchronization
2. Atomically set bit: `m_prioritiesWithMsg.fetch_or(1 << priority)`
3. Post semaphore to wake receiver: `m_notEmpty.post()`

**Receive Flow**:
1. Read bitmask (lock-free): `priorities = m_prioritiesWithMsg & m_priorityMask`
2. Find highest priority bit set
3. Call `msgQReceive(msgQ[priority], ..., NO_WAIT)`
4. If queue was empty, block on semaphore: `m_notEmpty.wait()`
5. If queue now empty, atomically clear bit: `m_prioritiesWithMsg.fetch_and(~(1 << priority))`
6. Return dequeued message

This design minimizes synchronization overhead - only atomic operations on small integers, no spinlocks needed.

### PriorityMemQueue Overview

`PriorityMemQueue` is the main class that implements `Os::QueueInterface`. It provides a multi-priority message queue with configurable ISR-safe operations and flexible per-priority configuration.

**Priority Ordering**: Larger priority numbers have higher priority than lower priority numbers (i.e., priority 0 is the lowest priority, priority 15 is the highest).

**Key Features**:
- **Multi-Priority Support**: Up to 16 independent priority levels (0-15)
- **Per-Priority Memory Pools**: Each priority has dedicated buffer allocation 
- **ISR-Safe Operations**: Non-blocking send/receive can be called from interrupt context (platform-dependent)
- **Flexible Configuration**: Static configuration allows per-component, per-priority sizing
- **Priority Management**: Runtime enable/disable of individual priority levels
- **Blocking Support**: Optional blocking behavior for send and receive operations using counting semaphores
- **High Water Mark Tracking**: Monitors peak queue usage for system analysis
- **Dynamic Memory Allocation**: Arrays allocated only when needed, reducing memory footprint

#### Synchronization

**PriorityMemQueue** uses a counting semaphore for blocking receive operations:
- **m_notEmpty**: Semaphore count represents number of messages available across all priorities
  - Initialized to 0 (queue starts empty)
  - `post()` called after successful enqueue to signal message availability
  - `wait()` blocks receiver when count reaches 0 (queue empty)
  - After `wait()` returns, receiver dequeues from highest priority with messages

A semaphore is used (as opposed to a condition variable & mutex) because:
- Semaphores are ISR safe 
- Simplifies synchronization 
    - No need for separate mutex acquisition before checking queue state
    - No double-check pattern required to prevent lost notifications
    - Semaphore count intrinsically tracks message availability

### Configuration 

The static configuration system allows per-component, per-priority queue sizing. This enables fine-grained memory allocation tailored to each component's messaging patterns.

If a configuration is not provided for a given queue instance ID, then create() will use the message max size and depth arguments for priority DEFAULT_PRIORITY (0). 

### ISR Safety

The VxWorks msgQ-based implementation provides ISR safety through:

1. **msgQSend()**: ISR-safe by design in VxWorks
   - Can be called with `NO_WAIT` timeout from interrupt context
   - Uses atomic operations internally for SMP safety
   
2. **Atomic Bitmasks**: Lock-free priority tracking
   - `std::atomic<U32>` operations are compiler-intrinsic atomics
   - No interrupt disable or mutex needed
   
3. **msgQReceive()**: ISR-safe with `NO_WAIT` timeout
   - Non-blocking variant safe from interrupt context

**ISR Usage Pattern**:
```cpp
// From ISR context - use NONBLOCKING
queue.send(data, size, priority, QueueInterface::BlockingType::NONBLOCKING);
queue.receive(dest, capacity, QueueInterface::BlockingType::NONBLOCKING, size, pri);
```


