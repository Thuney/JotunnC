#ifndef APPLICATION_H
#define APPLICATION_H

#include "event.h"
#include "window.h"

struct application_t
{
    char* name;
    struct window_t window;

    int running;
};

int application_init(struct application_t* app, char* app_name);

int application_start(struct application_t* app);
void application_run(struct application_t* app);
int application_stop(struct application_t* app);

void application_cleanup(struct application_t* app);

void application_on_event(struct application_t* app, struct event_base_t* event);

#endif