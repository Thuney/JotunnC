#include <stdio.h>

#include "application.h"
#include "ball_window.h"

static struct application_t jotunn_app;
static struct ball_window_t ball_window;
//

int main(int argc, char** argv)
{
    const uint8_t max_windows = 1;

    struct application_t* app_ptr = &(jotunn_app);
    struct ball_window_t* ball_window_ptr = &(ball_window);

    struct window_t *base_window_ptr = &(ball_window.window);

    uint8_t error = 0U;

    error |= application_init(app_ptr, "JotunnTestApp", max_windows);

    #ifdef DEBUG
        fprintf(stdout, "Initializing Ball Window\n");
    #endif

    ball_window_init(app_ptr, &ball_window);

    #ifdef DEBUG
        fprintf(stdout, "Setting Window Function\n");
    #endif

    window_set_function_custom_window_run(base_window_ptr, &ball_window_run);

    error |= application_add_window(app_ptr, base_window_ptr);

    if (!error)
    {
        application_start(app_ptr);

        #ifdef DEBUG
            fprintf(stdout, "Running application\n");
        #endif

        while (app_ptr->running)
        {
            application_run(app_ptr);
        }

        application_stop(app_ptr);
        application_cleanup(app_ptr);

        ball_window_cleanup(&ball_window);
    }
#ifdef DEBUG
    else
    {
        fprintf(stdout, "Error initializing application\n");
    }
    fprintf(stdout, "Jobs done from main.c\n");
    fflush(stdout);
#endif  
}