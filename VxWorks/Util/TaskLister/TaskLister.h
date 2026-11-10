#ifndef TASK_LISTER_H
#define TASK_LISTER_H

#ifdef __cplusplus
extern "C" {
#endif

    // dump current VxWorks task names with pririty and CPU affinity to stdout
    void listActiveTasks(void);

#ifdef __cplusplus
}
#endif

#endif  // TASK_LISTER_H
