#include "application.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

int application_init(struct application_t* app, char* app_name)
{
    memset(app, 0, sizeof(struct application_t));   

    int name_length = strlen(app_name);
    app->name = (char*) malloc(name_length*sizeof(char));    
    strcpy(app->name, app_name);

    window_init(&app->window, 800, 600, "Window");

    return 0;
}

int application_start(struct application_t* app)
{
    app->running = 1;

    return 0;
}

int application_run(struct application_t* app)
{
    static int counter = 0;

    while(counter++ < 1000)
    {
        for (int i = 0; i < 10000; i++);

        glfwSwapBuffers(app->window.glfw_window);
    }

    app->running = 0;

    return 0;
}

int application_stop(struct application_t* app)
{
    app->running = 0;

    return 0;
}

int application_cleanup(struct application_t* app)
{
    application_stop(app);

    free(app->name);
    app->name = 0;

    window_cleanup(&app->window);

    return 0;
}