#pragma once

#include "font.h"
#include "renderer_2d.h"

struct application_t;

struct window_data_t
{
    char* tag;
    //
    uint32_t width;
    uint32_t height;
    //
    uint8_t signaled_close;
    uint8_t visible;
    //
    double window_time_sec;
    //
    struct application_t* parent_application;
    void (*function_event_notify)(struct application_t* application, struct event_base_t* event);
};

struct window_context_data_t
{
    void* window_handle;
};

struct window_t
{
    struct window_data_t metadata;
    struct window_context_data_t context_data;
    // 
    struct renderer_2d_t renderer;
    //
    struct typeface_t typeface;
    //
    struct texture_2d_t test_texture;
};

uint8_t window_init(struct window_t* window, uint32_t width, uint32_t height, const char* tag, struct application_t* app_parent);

void window_show(struct window_t* window);
void window_hide(struct window_t* window);

uint8_t window_run(struct window_t* window);

void window_cleanup(struct window_t* window);

void window_set_context(struct window_t* window);

void window_set_background_color(struct window_t* window, const fvector4 color);