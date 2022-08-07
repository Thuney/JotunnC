#pragma once

#include "font.h"
#include "renderer_2d.h"

struct application_t;

struct window_data_t
{
    char* tag;

    int width;
    int height;

    int signaled_close;

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

int window_init(struct window_t* window, int width, int height, char* tag, struct application_t* app_parent);

int window_run(struct window_t* window);

void window_cleanup(struct window_t* window);

void window_set_background_color(struct window_t* window, const fvector4 color);