#include <time.h>

struct timestep_t
{
    double current_time_seconds;
    // Time from last step
    double delta_time_seconds;
    // Underlying time structs
    struct timespec current_time;
    struct timespec last_time;
    struct timespec start_time;
};

void timestep_init(struct timestep_t* timestep);
void timestep_reset(struct timestep_t* timestep);
void timestep_step(struct timestep_t* timestep);
void timestep_cleanup();