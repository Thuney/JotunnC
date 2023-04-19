#include "window_layer.h"

void window_layer_init(struct window_layer_t* window_layer, struct framebuffer_t* framebuffer, struct camera_base_t* camera, struct renderer_base_t* renderer)
{
    window_layer->framebuffer = framebuffer;
    window_layer->camera      = camera;
    window_layer->renderer    = renderer;

    window_layer->function_custom_window_layer_run = 0;
    window_layer->function_custom_event = 0;
}

void window_layer_set_custom_layer_run(struct window_layer_t* window_layer, void (*function_custom_window_layer_run)(struct window_t* parent_window, struct window_layer_t* window_layer))
{
    window_layer->function_custom_window_layer_run = function_custom_window_layer_run;
}

// void window_layer_set_custom_event_run(struct window_layer_t* window_layer, void (*function_custom_event)(struct window_t* parent_window, struct window_layer_t* window_layer, struct event_base_t* event) )
// {
//     window_layer->function_custom_event = function_custom_event;
// }