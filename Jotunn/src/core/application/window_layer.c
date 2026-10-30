#include "window_layer.h"

/*******************************************************************************
 *
 *   Function: window_layer_init()
 *
 ******************************************************************************/
void window_layer_init(
  struct window_t* in_parent_window,
  struct window_layer_t* out_window_layer,
  struct framebuffer_t* in_framebuffer,
  struct camera_base_t* in_camera,
  struct renderer_base_t* in_renderer)
{
  out_window_layer->parent_window = in_parent_window;
  out_window_layer->framebuffer = in_framebuffer;
  out_window_layer->camera = in_camera;
  out_window_layer->renderer = in_renderer;

  out_window_layer->function_custom_window_layer_run = 0;
  out_window_layer->function_event_react = 0;
}

/*******************************************************************************
 *
 *   Function: window_layer_set_custom_layer_run()
 *
 ******************************************************************************/
void window_layer_set_custom_layer_run(
  struct window_layer_t* inout_window_layer,
  void (*in_function_custom_window_layer_run)(struct window_layer_t* inout_window_layer))
{
  inout_window_layer->function_custom_window_layer_run =
    in_function_custom_window_layer_run;
}

/*******************************************************************************
 *
 *   Function: window_layer_set_event_react()
 *
 ******************************************************************************/
void window_layer_set_event_react(
  struct window_layer_t* inout_window_layer,
  void (*in_function_event_react)(
    struct window_t* inout_parent_window,
    struct window_layer_t* inout_window_layer,
    struct event_base_t* inout_event))
{
  inout_window_layer->function_event_react = in_function_event_react;
}
