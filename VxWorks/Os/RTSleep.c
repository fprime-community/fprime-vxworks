#include <Fw/Types/BasicTypes.hpp>

#include "RTSleep.h"
#include <Fw/Types/CAssert.hpp>
#include <timers.h>

#define MAX_ITERATIONS 1024

void rt_sleep_ms(const long ms)
{
  rt_sleep_ns(ms * 1000000);
}

void rt_sleep_us(const long usec)
{
  rt_sleep_ns(usec * 1000);
}

void rt_sleep_ns(const long nsec)
{
  struct timespec ts;
  int status = 0;
  ts.tv_sec = nsec / 1000000000;
  ts.tv_nsec = (nsec % 1000000000);

  for (U32 i = 0; i < MAX_ITERATIONS; ++i) {
    if( (status = nanosleep(&ts, &ts)) == -1 ){
      status = errno;
    }
    /* Continue sleeping if interupted by signal handler */
    if (status != EINTR) {
      break;
    }
  }
  
  // If status is something other than zero here then something
  // has gone horribly wrong...
  FW_CASSERT(0 == status);
}

