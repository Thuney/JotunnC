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

    window_init(&app->window, 800, 600, "Window");

    return 0;
}

int application_start(struct application_t* app)
{
    #ifdef DEBUG
        fprintf(stdout, "Starting application\n");
    #endif

    app->running = 1;

    return 0;
}

int application_run(struct application_t* app)
{
    #ifdef DEBUG
        fprintf(stdout, "Running application\n");
    #endif

    static int counter = 0;

    while(counter++ < 1000)
    {
        for (int i = 0; i < 10000; i++);

        // Run as long as window doesn't signal to close
        if(window_run(&app->window))
        {
            #ifdef DEBUG
                fprintf(stdout, "Window signaled to close\n");
            #endif
            
            break;
        }
    }

    app->running = 0;

    return 0;
}

int application_stop(struct application_t* app)
{
    #ifdef DEBUG
        fprintf(stdout, "Stopping application\n");
    #endif

    app->running = 0;

    return 0;
}

int application_cleanup(struct application_t* app)
{
    #ifdef DEBUG
        fprintf(stdout, "Cleaning up application\n");
    #endif

    if(app->running) application_stop(app);

    free(app->name);
    app->name = 0;

    window_cleanup(&app->window);

    return 0;
}