#include "application.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int application_init(struct application_t* app, char* app_name)
{
    #ifdef DEBUG
        fprintf(stdout, "Initializing application\n");
    #endif

    memset(app, 0, sizeof(struct application_t));

    int name_length = strlen(app_name);
    app->name = (char*) malloc(name_length*sizeof(char));
    strcpy(app->name, app_name);

    return window_init(&app->window, 800, 600, "JotunnWindow", app);
}

int application_start(struct application_t* app)
{
    #ifdef DEBUG
        fprintf(stdout, "Starting application\n");
    #endif

    app->running = 1;

    return 0;
}

void application_run(struct application_t* app)
{
    int signaled_close = window_run(&app->window);
    if(signaled_close)
    {
        app->running = 0;

        #ifdef DEBUG
            fprintf(stdout, "Window signaled to close\n");
        #endif
    }
}

int application_stop(struct application_t* app)
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

    window_cleanup(&app->window);
}

void application_on_event(struct application_t* app, struct event_base_t* event)
{
    switch (event->event_type)
    {
        case EVENT_WINDOW_RESIZE:
        {
            // Should be safe cast from base to derived form with base as first element
            struct event_window_resize_t* window_resize_event = (struct event_window_resize_t*)event;

            int new_width, new_height;
            new_width  = window_resize_event->width;
            new_height = window_resize_event->height;

            fprintf(stdout, "Custom Window Resize Event Received: new_width = %d, new_height = %d\n", new_width, new_height);
        }
        break;

        default:
        {

        }
        break;
    }
}