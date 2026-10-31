#pragma once

// A monotonic clock that measures the time since it started and between
//  steps, as a main loop needs

/** System Includes **/
#include <time.h>

/*******************************************************************************
 *
 *   TIMESTEP
 *
 ******************************************************************************/

// Times as of the most recent step
struct timestep_t
{
  // Time from start to the last step
  double current_time_seconds;
  // Time from last step
  double delta_time_seconds;
  // Underlying time structs
  struct timespec current_time;
  struct timespec last_time;
  struct timespec start_time;
};

/*******************************************************************************
 *
 *   TIMING
 *
 ******************************************************************************/

// Starts the clock now, with no time elapsed
//
//  out:    out_timestep
void timestep_init(
  struct timestep_t* out_timestep);

// Restarts the clock now, as timestep_init() does
//
//  out:    out_timestep
void timestep_reset(
  struct timestep_t* out_timestep);

// Takes a step, measuring the time since the clock started and since the
//  last step (or the start, for the first step)
//
//  in/out: inout_timestep
void timestep_step(
  struct timestep_t* inout_timestep);

// Does nothing; there's nothing to release
void timestep_cleanup(void);
