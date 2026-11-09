# VxWorks::Util::TaskUpdater

## Introduction

TaskUpdater is a plain C utility (not an F Prime component) for VxWorks 7 SMP targets. Given a table of task names, it changes the priority and the CPU affinity of each named task. It is intended to be called once after the topology has started its tasks, so that task scheduling and core placement can be tuned from a single table instead of being scattered through component configuration.

The utility has no ports, commands, events, telemetry or parameters. Results are reported by return value and by `printf` to the console.

## Requirements

| Name | Description | Validation |
|---|---|---|
| TU-001 | The utility shall accept a table of entries, each holding a task name, a priority and a CPU index. | Inspection, target test |
| TU-002 | The utility shall look up each task by name and set its priority to the specified value. | Target test |
| TU-003 | The utility shall pin each task to the single CPU core specified in its entry. | Target test |
| TU-004 | The utility shall allow either the priority or the CPU of an entry to be left unchanged by using `TASK_UPDATER_NO_CHANGE`. | Target test |
| TU-005 | The utility shall continue processing the remaining entries when an entry fails, and shall report the number of failed entries. | Target test |
| TU-006 | The utility shall reject a NULL table, a count below 1, or a count above `TASK_UPDATER_MAX_ENTRIES` (set in `taskupdater_config/TaskUpdaterCfg.h`). | Inspection, target test |
| TU-007 | The utility shall reject priorities outside 0 - 255 and negative CPU indices (other than `TASK_UPDATER_NO_CHANGE`). | Inspection, target test |
| TU-008 | The utility shall reject a table containing duplicate task names, print the duplicate name, and change no tasks. | Inspection, target test |

## Usage Examples

### Filling in the TaskUpdaterEntry Table

The table is an array of `TaskUpdaterEntry`, one row per task. Each row has three fields, in this order:

| Field | Type | Meaning | Valid values |
|---|---|---|---|
| `name` | `const char*` | VxWorks task name, exactly as shown by `i` or `taskNameToId`. Matching is case-sensitive. | Non-NULL, NUL-terminated string naming an existing, uniquely named task |
| `priority` | `int` | New task priority. Lower numbers are higher priority in VxWorks. | 0 - 255, or `TASK_UPDATER_NO_CHANGE` |
| `cpu` | `int` | Index of the single CPU core the task is pinned to. | 0 to (number of cores - 1), or `TASK_UPDATER_NO_CHANGE` |

Guidelines:

- Declare the table `static const` and pass its length with `sizeof(table) / sizeof(table[0])` so the count cannot drift out of sync with the contents. The count must be between 1 and `TASK_UPDATER_MAX_ENTRIES` (256 by default; see Configuration).
- Use `TASK_UPDATER_NO_CHANGE` in `priority` or `cpu` to leave that attribute as it is. An entry with both set to `TASK_UPDATER_NO_CHANGE` does nothing except confirm the task exists.
- Each entry pins to exactly one core. There is no way to give a task a set of allowed cores.
- List each task name once. If a name appears twice, `TaskUpdater_apply` prints the duplicate name and the two entry indices, returns -1, and changes no tasks.
- Task names must match the names the tasks were created with. For F Prime active components this is the name given to the task at start-up, so confirm it on the target (for example with the `i` shell command) before adding it to the table.
- The tasks must already exist when `TaskUpdater_apply` is called. A name that is not found is reported and counted as a failure.
- A CPU index at or above the number of cores on the target is passed to VxWorks as is, and the call fails at run time. The utility only rejects negative indices.

Example rows:

```c
static const TaskUpdaterEntry table[] = {
    //  name            priority                  cpu
    {"tRateGroup1",     100,                      1},                       // change both
    {"tLogger",         150,                      TASK_UPDATER_NO_CHANGE},  // priority only
    {"tCmdDisp",        TASK_UPDATER_NO_CHANGE,   0},                       // affinity only
};
```

### Typical Usage

```c
#include "TaskUpdater.h"

static const TaskUpdaterEntry table[] = {
    {"tRateGroup1", 100, 1},
    {"tLogger", 150, TASK_UPDATER_NO_CHANGE},
    {"tCmdDisp", TASK_UPDATER_NO_CHANGE, 0},
};

int failures = TaskUpdater_apply(table, sizeof(table) / sizeof(table[0]));
```

Call this after the tasks named in the table have been created; tasks that do not exist yet are reported as not found.

## Functional Behaviors

`TaskUpdater_apply(table, count)` validates its arguments, then scans the table for duplicate task names (NULL names are ignored by the scan). If two entries share a name, it prints the name and both entry indices and returns -1 before changing any task. Otherwise it processes entries in order. For each entry:

1. If `name` is NULL, the entry fails.
2. The task ID is found with `taskNameToId`. If no task has that name, the entry fails.
3. If `priority` is not `TASK_UPDATER_NO_CHANGE`, it must be in 0 - 255 and is applied with `taskPrioritySet`.
4. If `cpu` is not `TASK_UPDATER_NO_CHANGE`, it must be non-negative. A `cpuset_t` containing only that core is built with `CPUSET_ZERO`/`CPUSET_SET` and applied with `taskCpuAffinitySet`. Affinity is always a single core; multi-core masks are not supported.

A failure in step 3 does not prevent step 4 for the same entry. Each failure prints a message naming the task. An entry counts as failed if any of its steps failed.

The function returns the number of failed entries (0 means all succeeded), or -1 if the arguments are invalid or the table has duplicate names. All loops are bounded by `TASK_UPDATER_MAX_ENTRIES`. The function does no dynamic allocation. The table is read-only; the task name strings are not modified.

### Port Descriptions

None. TaskUpdater is not an F Prime component.

### Component States

None. TaskUpdater is stateless.

### Sequence Diagrams

```
Caller            TaskUpdater            VxWorks
  | apply(table,n)    |                      |
  |------------------>|                      |
  |                   | check duplicate names|
  |                   | (-1 if any, no change)|
  |                   | taskNameToId(name)   |
  |                   |--------------------->|
  |                   | taskPrioritySet      |
  |                   |--------------------->|
  |                   | taskCpuAffinitySet   |
  |                   |--------------------->|
  |                   |   (repeat per entry) |
  | failure count     |                      |
  |<------------------|                      |
```

## Parameters

None.

## Commands

None.

## Events

None. Failures are printed to the console with `printf`.

## Telemetry

None.

## Configuration

Compile-time settings are in `taskupdater_config/TaskUpdaterCfg.h`, registered with `register_fprime_config` as `TaskUpdater-config`. The module depends on it.

| Name | Default | Description |
|---|---|---|
| `TASK_UPDATER_MAX_ENTRIES` | 256 | Maximum number of table entries accepted by `TaskUpdater_apply`; bounds the update loop and the duplicate-name scan. |

## Limitations and Open Items

- The code builds only for VxWorks (`restrict_platforms(vxworks)`), so it has no native unit tests; validation is on target.
- The VxWorks affinity API names (`<cpuset.h>`, `CPUSET_ZERO`, `CPUSET_SET`, `taskCpuAffinitySet`) should be confirmed against the SDK headers on first build.
- Duplicate names within the table are rejected, but the utility cannot detect two running tasks that share a name. `taskNameToId` returns only one of them.
