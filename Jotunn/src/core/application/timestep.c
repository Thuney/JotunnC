#define _POSIX_C_SOURCE 199309L
#include "timestep.h"

#include <stdlib.h>

void timestep_init(struct timestep_t* timestep)
{
    timestep->current_time_seconds = 0.0f;
    timestep->delta_time_seconds = 0.0f;
    
    clock_gettime(CLOCK_MONOTONIC, &timestep->start_time);
    clock_gettime(CLOCK_MONOTONIC, &timestep->last_time);
    clock_gettime(CLOCK_MONOTONIC, &timestep->current_time);
}

void timestep_reset(struct timestep_t* timestep)
{
    timestep_init(timestep);
}

static double diff_timespec(const struct timespec *time1, const struct timespec *time0) {
  return (time1->tv_sec - time0->tv_sec)
      + (time1->tv_nsec - time0->tv_nsec) / 1000000000.0;
}

void timestep_step(struct timestep_t* timestep)
{
    timestep->last_time = timestep->current_time;
    clock_gettime(CLOCK_MONOTONIC, &timestep->current_time);

    timestep->current_time_seconds = diff_timespec(&timestep->current_time, &timestep->start_time);
    timestep->delta_time_seconds = diff_timespec(&timestep->current_time, &timestep->last_time);
}

void timestep_cleanup()
{
    
}