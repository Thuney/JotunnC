#pragma once

#include "renderer_2d.h"

struct application_t;
struct camera_base_t;
struct renderer_base_t;
struct window_t;

struct window_data_t
{
    char* tag;
    //
    uint32_t width;
    uint32_t height;
    //
    uint8_t resized;
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
    struct camera_base_t* camera;
    struct renderer_base_t* renderer;
    //
    void (*function_custom_window_run)(struct window_t* window);
};

uint8_t window_init(struct window_t* window, const uint32_t width, const uint32_t height, const char* tag, struct application_t* app_parent);
void window_set_renderer(struct window_t* window, struct renderer_base_t* renderer, struct camera_base_t* camera);

void window_show(struct window_t* window);
void window_hide(struct window_t* window);

uint8_t window_run(struct window_t* window);

void window_cleanup(struct window_t* window);

void window_set_context(struct window_t* window);
void window_release_context();

void window_set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

void window_set_background_color(struct window_t* window, const fvector4 color);

void window_set_function_custom_window_run(struct window_t* window, void (*function_custom_window_run)(struct window_t* window));