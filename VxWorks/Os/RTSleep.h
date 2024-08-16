#ifndef OS_RTSLEEP_H
#define OS_RTSLEEP_H

#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Sleep for msec milliseconds */
void rt_sleep_ms(const long msec);
/* Sleep for usec microseconds */
void rt_sleep_us(const long usec);
/* Sleep for nsec nanoseconds */
void rt_sleep_ns(const long nsec);

#ifdef __cplusplus
}
#endif

#endif
