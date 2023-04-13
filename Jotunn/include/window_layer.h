#pragma once

#include "framebuffer.h"
#include "renderer.h"

struct window_layer_t
{
    struct framebuffer_t* framebuffer;
    struct camera_base_t* camera;
    struct renderer_base_t* renderer;
    //
    void (*function_custom_window_layer_run)(struct window_layer_t* window_layer);
};

void window_layer_init(struct window_layer_t* window_layer, struct framebuffer_t* framebuffer, struct camera_base_t* camera, struct renderer_base_t* renderer);
void window_layer_set_custom_layer_run(struct window_layer_t* window_layer, void (*function_custom_window_layer_run)(struct window_layer_t* window_layer));