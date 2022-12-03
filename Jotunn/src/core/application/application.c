#include "application.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t application_init(struct application_t* app, const char* app_name, const uint8_t max_windows)
{
    #ifdef DEBUG
        fprintf(stdout, "Initializing application\n");
    #endif

    uint8_t error = 0;

    memset(app, 0, sizeof(struct application_t));

    int name_length = strlen(app_name);
    app->name = (char*) malloc(name_length*sizeof(char));
    strcpy(app->name, app_name);

    app->max_windows = max_windows;
    app->num_windows = 0;
    app->windows = (struct window_t*) malloc(app->max_windows*sizeof(struct window_t));
    app->current_window = app->windows;

    app->custom_event_function = 0;

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
    if (app->num_windows && app->running)
    {
        // Pointer to first window, to be iterated over
        struct window_t* cur_window = app->windows;
        for (uint8_t i = 0U; i < app->num_windows; i++)
        {
            if (cur_window->metadata.visible)
            {
                uint8_t signaled_close = window_run(cur_window);
                if(signaled_close)
                {
                    app->running = 0;

                    #ifdef DEBUG
                        fprintf(stdout, "Window signaled to close\n");
                    #endif
                }
            }
            cur_window++;
        }
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

    if (app->num_windows)
    {
        // Pointer to first window, to be iterated over
        struct window_t* cur_window = app->windows;
        for (uint8_t i = 0U; i < app->num_windows; i++)
        {
            window_cleanup(cur_window);
            cur_window++;
        }
    }

    app->max_windows = 0;
    app->num_windows = 0;
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
            
            // camera_set_projection_orthographic(&app->current_window->renderer.camera, 0.0f, (float)new_width, (float)new_height, 0.0f, -1.0f, 100.0f);
        }
        break;

        case EVENT_WINDOW_FOCUS:
        {
            // Should be safe cast from base to derived form with base as first element
            struct event_window_focus_t* window_focus_event = (struct event_window_focus_t*)event;

            if (window_focus_event->focused)
            {

                app->current_window = window_focus_event->window_handle;
                window_set_context(window_focus_event->window_handle);
            }
        }
        break;

        case EVENT_MOUSE_MOVED:
        {
            // Should be safe cast from base to derived form with base as first element
            struct event_mouse_moved_t* mouse_moved_event = (struct event_mouse_moved_t*)event;

            float x, y;
            x = mouse_moved_event->x;
            y = (app->current_window->metadata.height - mouse_moved_event->y);
        }
        break;

        default:
        {
            
        }
        break;
    }

    // Custom events

    if (app->custom_event_function && !event->handled)
    {
        app->custom_event_function(event);
    }
}

void application_bind_custom_events(struct application_t* app, void (*custom_event_function)(struct event_base_t*))
{
    app->custom_event_function = custom_event_function;
}

uint8_t application_add_window(struct application_t* app, struct window_t new_window)
{
    #ifdef DEBUG
        fprintf(stdout, "Adding window to application\n");
    #endif  

    uint8_t error = 0U;

    if (app->num_windows < app->max_windows)
    {
        struct window_t* window_ptr;
        window_ptr = (app->windows + app->num_windows);
        *window_ptr = new_window;
        app->current_window = window_ptr;
        app->num_windows++;
    }
    else
    {
        error = 1U;
    }

    return error;
}