#include "application.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t application_init(struct application_t* app, char* app_name, const uint8_t num_windows)
{
    #ifdef DEBUG
        fprintf(stdout, "Initializing application\n");
    #endif

    memset(app, 0, sizeof(struct application_t));

    int name_length = strlen(app_name);
    app->name = (char*) malloc(name_length*sizeof(char));
    strcpy(app->name, app_name);

    app->num_windows = num_windows;
    app->windows = (struct window_t*) malloc(app->num_windows*sizeof(struct window_t));

    uint8_t error = 0;

    // Pointer to first window, to be iterated over
    struct window_t* cur_window = app->windows;
    for (uint8_t i = 0U; i < app->num_windows; i++)
    {
        error |= window_init(cur_window, 400, 600, "JotunnWindow", app);
        cur_window++;
    }

    return error;
}

uint8_t application_start(struct application_t* app)
{
    #ifdef DEBUG
        fprintf(stdout, "Starting application\n");
    #endif

    app->running = 1;

    return 0;
}

void application_run(struct application_t* app)
{
    // Pointer to first window, to be iterated over
    struct window_t* cur_window = app->windows;
    for (uint8_t i = 0U; i < app->num_windows; i++)
    {
        uint8_t signaled_close = window_run(cur_window);
        if(signaled_close)
        {
            app->running = 0;

            #ifdef DEBUG
                fprintf(stdout, "Window signaled to close\n");
            #endif
        }
        cur_window++;
    }

    uint8_t signaled_close = window_run(&app->window);
    if(signaled_close)
    {
        app->running = 0;

        #ifdef DEBUG
            fprintf(stdout, "Window signaled to close\n");
        #endif
    }
}

uint8_t application_stop(struct application_t* app)
{
    #ifdef DEBUG
        fprintf(stdout, "Stopping application\n");
    #endif

    app->running = 0;

    return 0;
}

void application_cleanup(struct application_t* app)
{
    #ifdef DEBUG
        fprintf(stdout, "Cleaning up application\n");
    #endif

    if (app->running) application_stop(app);

    free(app->name);
    app->name = 0;

    // Pointer to first window, to be iterated over
    struct window_t* cur_window = app->windows;
    for (uint8_t i = 0U; i < app->num_windows; i++)
    {
        window_cleanup(cur_window);
        cur_window++;
    }
}

void application_on_event(struct application_t* app, struct event_base_t* event)
{
    switch (event->event_type)
    {
        case EVENT_WINDOW_RESIZE:
        {
            // Should be safe cast from base to derived form with base as first element
            struct event_window_resize_t* window_resize_event = (struct event_window_resize_t*)event;

            uint32_t new_width, new_height;
            new_width  = window_resize_event->width;
            new_height = window_resize_event->height;
            
            // TODO: FIX THIS SHIT
            camera_set_projection_orthographic(&app->windows[0].renderer.camera, 0.0f, (float)new_width, (float)new_height, 0.0f, -1.0f, 100.0f);
        }
        break;

        case EVENT_MOUSE_MOVED:
        {
            struct event_mouse_moved_t* mouse_moved_event = (struct event_mouse_moved_t*)event;

            float x, y;
            x = mouse_moved_event->x;
            y = (app->windows[0].metadata.height - mouse_moved_event->y);
        }
        break;

        default:
        {

        }
        break;
    }
}