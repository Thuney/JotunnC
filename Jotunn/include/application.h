#pragma once

#include "window.h"

struct application_t
{
    char* name;
    struct window_t window;

    int running;
};

int application_init(struct application_t* app, char* app_name);

int application_start(struct application_t* app);
int application_run(struct application_t* app);
int application_stop(struct application_t* app);

int application_cleanup(struct application_t* app);