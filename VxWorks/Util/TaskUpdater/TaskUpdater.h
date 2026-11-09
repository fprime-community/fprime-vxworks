/**
 * @file TaskUpdater.h
 * @brief Utility to change VxWorks task priorities and CPU affinities from a table.
 *
 * Given a table of task names, each entry specifies a new priority and a new
 * CPU affinity for the named task. Entries whose task cannot be found, or whose
 * update fails, are reported and counted but do not stop processing of the
 * remaining entries.
 */

#ifndef TASK_UPDATER_H
#define TASK_UPDATER_H

/* Provides TASK_UPDATER_MAX_ENTRIES, the upper bound on the number of table entries. */
#include "taskupdater_config/TaskUpdaterCfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Value for the priority or cpu field meaning "leave this attribute unchanged". */
#define TASK_UPDATER_NO_CHANGE (-1)

/** One row of the update table. */
typedef struct {
    const char* name; /**< VxWorks task name (NUL terminated) */
    int priority;     /**< New priority (0 - 255), or TASK_UPDATER_NO_CHANGE */
    int cpu;          /**< CPU core index the task is pinned to, or TASK_UPDATER_NO_CHANGE */
} TaskUpdaterEntry;

/**
 * Apply a table of priority / CPU affinity changes.
 *
 * @param table  Array of entries
 * @param count  Number of entries in the table (1 - TASK_UPDATER_MAX_ENTRIES)
 * @return number of entries that failed (0 = all succeeded), or -1 if the
 *         arguments are invalid or two entries have the same task name (in
 *         which case no task is changed)
 */
int TaskUpdater_apply(const TaskUpdaterEntry* table, int count);

#ifdef __cplusplus
}
#endif

#endif  // TASK_UPDATER_H
