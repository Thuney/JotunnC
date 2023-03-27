#include <time.h>

struct timestep_t
{
    float current_time_seconds;
    // Time from last step
    float delta_time_seconds;
    // Underlying clock cycle count
    clock_t current_clock;
};

void timestep_init(struct timestep_t* timestep);
void timestep_reset(struct timestep_t* timestep);
void timestep_step(struct timestep_t* timestep);
void timestep_cleanup();