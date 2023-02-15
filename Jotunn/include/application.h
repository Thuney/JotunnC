#pragma once

#include "event.h"
#include "window.h"

struct application_t
{
    //
    char* name;
    //
    struct window_t* current_window;
    struct window_t* windows;
    uint8_t num_windows;
    uint8_t max_windows;
    //
    uint8_t running;
    //
    void (*custom_event_function)(struct event_base_t*);
    //
};

uint8_t application_init(struct application_t* app, const char* app_name, const uint8_t max_windows);

uint8_t application_start(struct application_t* app);
void application_run(struct application_t* app);
uint8_t application_stop(struct application_t* app);

void application_cleanup(struct application_t* app);

void application_on_event(struct application_t* app, struct event_base_t* event);
void application_bind_custom_events(struct application_t* app, void (*custom_event_function)(struct event_base_t*));

uint8_t application_add_window(struct application_t* app, struct window_t new_window);
// uint8_t application_remove_window(struct application_t* app, struct window_t* remove_window);