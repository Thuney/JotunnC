#include <memory.h>
#include <stdio.h>

#include "application.h"
#include "fvector.h"

struct application_t jotunn_app;
struct window_t window1, window2;

uint8_t init_window1(struct application_t* app, struct window_t* window1)
{
    uint8_t error = 0U;

    error |= window_init(window1, 600, 400, "JotunnWindow1", app);

    fvector4 window1_background_color;
    fvector4_set(&window1_background_color, 1.0f, 0.0f, 0.0f, 1.0f);

    window_set_context(window1);
    window_set_background_color(window1, window1_background_color);

    return error;
}

uint8_t init_window2(struct application_t* app, struct window_t* window2)
{
    uint8_t error = 0U;

    error |= window_init(window2, 200, 200, "JotunnWindow2", app);

    fvector4 window2_background_color;
    fvector4_set(&window2_background_color, 0.0f, 1.0f, 0.0f, 1.0f);

    window_set_context(window2);    
    window_set_background_color(window2, window2_background_color);

    return error;
}

int main(int argc, char** argv)
{
    struct application_t* app_ptr = &jotunn_app;
    struct window_t *window1_ptr = &window1, *window2_ptr = &window2;

    uint8_t error = 0U;

    error |= application_init(app_ptr, "JotunnTestApp", 2);

    init_window1(app_ptr, window1_ptr);
    init_window2(app_ptr, window2_ptr);

    error |= application_add_window(app_ptr, window1);
    error |= application_add_window(app_ptr, window2);

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