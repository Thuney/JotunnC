#pragma once

#include "event.h"
#include "window.h"

enum application_tick_rate_t
{
    APP_TICKRATE_1000_HZ = 0x00,
    APP_TICKRATE_500_HZ,
    APP_TICKRATE_200_HZ,
    APP_TICKRATE_100_HZ,
    APP_TICKRATE_20_HZ,
    APP_TICKRATE_10_HZ,
    APP_TICKRATE_1_HZ
};

struct application_t
{
    char* name;
    struct window_t window;
    //
    enum application_tick_rate_t tick_rate;
    //
    int running;
};

int application_init(struct application_t* app, char* app_name, const enum application_tick_rate_t app_tick_rate);

int application_start(struct application_t* app);
void application_run(struct application_t* app);
int application_stop(struct application_t* app);

void application_cleanup(struct application_t* app);

void application_on_event(struct application_t* app, struct event_base_t* event);