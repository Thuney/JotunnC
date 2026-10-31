#pragma once

// A window layer is one of the images a window composites into each frame:
//  it renders to its own framebuffer, through its own camera and renderer,
//  and gets a say in the window's events. A window draws its layers in the
//  order they were added, so later layers go over earlier ones, and offers
//  them events in the reverse order

#include "framebuffer.h"
#include "renderer.h"
#include "window.h"

/*******************************************************************************
 *
 *   WINDOW LAYER
 *
 ******************************************************************************/

// A layer of a window. The window resizes the framebuffer and reprojects the
//  camera when the window's size changes, and begins and ends the renderer's
//  scene around function_custom_window_layer_run each frame
struct window_layer_t
{
  struct window_t* parent_window;
  struct framebuffer_t* framebuffer;
  struct camera_base_t* camera;
  struct renderer_base_t* renderer;
  // Does the rendering for this layer, whatever that may be
  void (*function_custom_window_layer_run)(
    struct window_layer_t* inout_window_layer);
  // Acts on events propagated through to this layer; marking an event
  //  handled keeps it from the layers underneath and the window
  void (*function_event_react)(
    struct window_t* inout_parent_window,
    struct window_layer_t* inout_window_layer,
    struct event_base_t* inout_event);
};

/*******************************************************************************
 *
 *   SETUP
 *
 ******************************************************************************/

// Sets up a layer to draw into in_framebuffer with in_renderer, viewed
//  through in_camera, with nothing to run and no event handler yet. The layer
//  keeps the pointers but owns none of them, and must still be added to
//  in_parent_window with window_add_layer()
//
//  in:     in_parent_window, in_framebuffer, in_camera, in_renderer
//  out:    out_window_layer
void window_layer_init(
  struct window_t* in_parent_window,
  struct window_layer_t* out_window_layer,
  struct framebuffer_t* in_framebuffer,
  struct camera_base_t* in_camera,
  struct renderer_base_t* in_renderer);

// Binds the function that does the layer's drawing each frame, between the
//  beginning and end of its renderer's scene; NULL unbinds it
//
//  in:     in_function_custom_window_layer_run
//  in/out: inout_window_layer
void window_layer_set_custom_layer_run(
  struct window_layer_t* inout_window_layer,
  void (*in_function_custom_window_layer_run)(
    struct window_layer_t* inout_window_layer));

// Binds the function that reacts to events reaching the layer; NULL unbinds
//  it
//
//  in:     in_function_event_react
//  in/out: inout_window_layer
void window_layer_set_event_react(
  struct window_layer_t* inout_window_layer,
  void (*in_function_event_react)(
    struct window_t* inout_parent_window,
    struct window_layer_t* inout_window_layer,
    struct event_base_t* inout_event));
