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

    return window_init(&app->window, 800, 600, "JotunnWindow");
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