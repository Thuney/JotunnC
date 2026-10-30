#define _POSIX_C_SOURCE 199309L
#include "timestep.h"

#include <stdlib.h>

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Subtracts one time from another
//
//  in:     in_time1, in_time0
//  ret:    in_time1 - in_time0, in seconds
static double timestep_subtract_timespecs(
  const struct timespec* in_time1,
  const struct timespec* in_time0);

/*******************************************************************************
 *
 *   Function: timestep_init()
 *
 ******************************************************************************/
void timestep_init(
  struct timestep_t* out_timestep)
{
  out_timestep->current_time_seconds = 0.0f;
  out_timestep->delta_time_seconds = 0.0f;

  clock_gettime(CLOCK_MONOTONIC, &out_timestep->start_time);
  clock_gettime(CLOCK_MONOTONIC, &out_timestep->last_time);
  clock_gettime(CLOCK_MONOTONIC, &out_timestep->current_time);
}

/*******************************************************************************
 *
 *   Function: timestep_reset()
 *
 ******************************************************************************/
void timestep_reset(
  struct timestep_t* out_timestep)
{
  timestep_init(out_timestep);
}

/*******************************************************************************
 *
 *   Function: timestep_subtract_timespecs()
 *
 ******************************************************************************/
static double timestep_subtract_timespecs(
  const struct timespec* in_time1,
  const struct timespec* in_time0)
{
  return (in_time1->tv_sec - in_time0->tv_sec) +
    (in_time1->tv_nsec - in_time0->tv_nsec) / 1000000000.0;
}

/*******************************************************************************
 *
 *   Function: timestep_step()
 *
 ******************************************************************************/
void timestep_step(
  struct timestep_t* inout_timestep)
{
  inout_timestep->last_time = inout_timestep->current_time;
  clock_gettime(CLOCK_MONOTONIC, &inout_timestep->current_time);

  inout_timestep->current_time_seconds =
    timestep_subtract_timespecs(&inout_timestep->current_time, &inout_timestep->start_time);
  inout_timestep->delta_time_seconds =
    timestep_subtract_timespecs(&inout_timestep->current_time, &inout_timestep->last_time);
}

/*******************************************************************************
 *
 *   Function: timestep_cleanup()
 *
 ******************************************************************************/
void timestep_cleanup(void)
{
}
