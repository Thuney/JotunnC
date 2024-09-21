#pragma once

#include "framebuffer.h"
#include "renderer.h"
#include "window.h"

struct window_layer_t
{
    struct window_t* parent_window;
    struct framebuffer_t* framebuffer;
    struct camera_base_t* camera;
    struct renderer_base_t* renderer;
    // Does the rendering for this layer, whatever that may be
    void (*function_custom_window_layer_run)(struct window_layer_t* window_layer);
    // Acts on events propagated through to this layer
    void (*function_event_react)(struct window_t* parent_window, struct window_layer_t* window_layer, struct event_base_t* event);
};

void window_layer_init(struct window_t* parent_window, struct window_layer_t* window_layer, struct framebuffer_t* framebuffer, struct camera_base_t* camera, struct renderer_base_t* renderer);

void window_layer_set_custom_layer_run(struct window_layer_t* window_layer, void (*function_custom_window_layer_run)(struct window_layer_t* window_layer) );

void window_layer_set_event_react(struct window_layer_t* window_layer, void (*function_event_react)(struct window_t* parent_window, struct window_layer_t* window_layer, struct event_base_t* event) );