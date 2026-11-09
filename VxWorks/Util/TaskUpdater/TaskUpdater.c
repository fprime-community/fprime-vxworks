/**
 * @file TaskUpdater.c
 * @brief Implementation of the VxWorks task priority / CPU affinity updater.
 */

#include "TaskUpdater.h"

#include <cpuset.h>
#include <stdio.h>
#include <string.h>
#include <taskLib.h>
#include <vxWorks.h>

#define TASK_UPDATER_MIN_PRIORITY 0
#define TASK_UPDATER_MAX_PRIORITY 255

static int updateEntry(const TaskUpdaterEntry* entry) {
    int failed = 0;

    if (entry->name == NULL) {
        (void)printf("TaskUpdater: NULL task name\n");
        return 1;
    }

    // taskNameToId takes a non-const char*, but does not modify it
    TASK_ID tid = taskNameToId((char*)entry->name);
    if (tid == TASK_ID_NULL) {
        (void)printf("TaskUpdater: task \"%s\" not found\n", entry->name);
        return 1;
    }

    if (entry->priority != TASK_UPDATER_NO_CHANGE) {
        if ((entry->priority < TASK_UPDATER_MIN_PRIORITY) || (entry->priority > TASK_UPDATER_MAX_PRIORITY)) {
            (void)printf("TaskUpdater: task \"%s\" priority %d out of range\n", entry->name, entry->priority);
            failed = 1;
        } else if (taskPrioritySet(tid, entry->priority) != OK) {
            (void)printf("TaskUpdater: task \"%s\" priority set to %d failed\n", entry->name, entry->priority);
            failed = 1;
        }
    }

    if (entry->cpu != TASK_UPDATER_NO_CHANGE) {
        if (entry->cpu < 0) {
            (void)printf("TaskUpdater: task \"%s\" invalid cpu %d\n", entry->name, entry->cpu);
            failed = 1;
        } else {
            cpuset_t affinity;
            CPUSET_ZERO(affinity);
            CPUSET_SET(affinity, entry->cpu);
            if (taskCpuAffinitySet(tid, affinity) != OK) {
                (void)printf("TaskUpdater: task \"%s\" cpu affinity set to %d failed\n", entry->name, entry->cpu);
                failed = 1;
            }
        }
    }

    return failed;
}

// Returns 1 and prints a message if any two entries share a task name. NULL names are
// skipped here; they are reported per-entry when the table is applied.
static int hasDuplicateNames(const TaskUpdaterEntry* table, int count) {
    for (int i = 0; i < count; i++) {
        if (table[i].name == NULL) {
            continue;
        }
        for (int j = i + 1; j < count; j++) {
            if ((table[j].name != NULL) && (strcmp(table[i].name, table[j].name) == 0)) {
                (void)printf("TaskUpdater: duplicate task name \"%s\" (entries %d and %d)\n", table[i].name, i, j);
                return 1;
            }
        }
    }
    return 0;
}

int TaskUpdater_apply(const TaskUpdaterEntry* table, int count) {
    if ((table == NULL) || (count < 1) || (count > TASK_UPDATER_MAX_ENTRIES)) {
        return -1;
    }

    // Validate the whole table before changing anything
    if (hasDuplicateNames(table, count) != 0) {
        return -1;
    }

    int failures = 0;
    for (int i = 0; i < count; i++) {
        failures += updateEntry(&table[i]);
    }
    return failures;
}
