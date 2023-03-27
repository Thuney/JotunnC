#include "timestep.h"

void timestep_init(struct timestep_t* timestep)
{
    // Resets time to 0, at the current application clock count. Will measure delta from this point.
    timestep->current_clock = clock();
    timestep->current_time_seconds = 0.0f;
}

void timestep_reset(struct timestep_t* timestep)
{
    timestep_init(timestep);
}

void timestep_step(struct timestep_t* timestep)
{
    clock_t new_clock = clock();
    timestep->current_clock += (new_clock - timestep->current_clock);

    float last_time_seconds = timestep->current_time_seconds;
    timestep->current_time_seconds = ((float)timestep->current_clock / CLOCKS_PER_SEC);
    timestep->delta_time_seconds = (timestep->current_time_seconds - last_time_seconds);
}

void timestep_cleanup()
{

}