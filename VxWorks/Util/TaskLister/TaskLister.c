#include <vxWorks.h>
#include <taskLib.h>
#include <cpuset.h>
#include <vxCpuLib.h>
#include <stdio.h>
#include <tasklister_config/TaskListerCfg.h>

void listActiveTasks(void)
    {
    TASK_ID idList[TASK_LISTER_MAX_ENTRIES];
    int numTasks;

    // Populate the array with active task IDs
    numTasks = taskIdListGet(idList, TASK_LISTER_MAX_ENTRIES);

    if (numTasks == ERROR)
        {
        printf("Failed to retrieve task list.\n");
        return;
        }

    printf("--- Active VxWorks Tasks (%d total) ---\n", numTasks);
    for (int i = 0; i < numTasks; i++)
        {
        char *name = taskName(idList[i]);
        const char *displayName = (name != NULL && name[0] != '\0') ? name : "<unnamed>";

        // Build a string of CPU indices in the task's affinity set (e.g. "0,2")
        char cpuStr[64];
        size_t len = 0;
        cpuStr[0] = '\0';
        cpuset_t affinity;
        CPUSET_ZERO(affinity);
        if (taskCpuAffinityGet(idList[i], &affinity) == OK)
            {
            unsigned int numCpus = vxCpuConfiguredGet();
            for (unsigned int cpu = 0; cpu < numCpus; cpu++)
                {
                if (CPUSET_ISSET(affinity, cpu) && (len + 4 < sizeof(cpuStr)))
                    {
                    len += (size_t)snprintf(&cpuStr[len], sizeof(cpuStr) - len, "%s%u",
                                            (len > 0) ? "," : "", cpu);
                    }
                }
            if (len == 0)
                {
                snprintf(cpuStr, sizeof(cpuStr), "none");
                }
            }
        else
            {
            snprintf(cpuStr, sizeof(cpuStr), "<error>");
            }

        printf("Task ID: %p | Name: %s | CPU Affinity: %s\n", (void *)idList[i], displayName, cpuStr);
        }
    }
