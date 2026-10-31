#pragma once

// A window layer drawing a Dear ImGui interface, with ImPlot and imnodes,
//  into a framebuffer of its own through a renderer_2d. Each layer has its own
//  ImGui contexts and feeds them the events reaching its window; a custom
//  render function makes the layer's ig*, ImPlot_* and imnodes_* calls each
//  frame

#include "event.h"
#include "renderer_2d.h"
#include "ui_element.h"
#include "window_layer.h"

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

struct ui_layer_t
{
  struct window_layer_t ui_window_layer;
  struct framebuffer_t ui_framebuffer;

  struct camera_orthographic_t ui_camera_ortho;
  struct renderer_2d_t ui_renderer_2d;

  // Managed by implementation
  void* ui_layer_data;
  void (*function_custom_ui_layer_render)(struct ui_layer_t* inout_ui_layer);
  void (*function_custom_ui_layer_event_react)(
    struct ui_layer_t* inout_ui_layer,
    struct event_base_t* inout_event);
};

/*******************************************************************************
 *
 *   LIFECYCLE
 *
 ******************************************************************************/

// Sets up a UI layer for a window, drawing into a framebuffer of the given
//  size. The window doesn't run the layer until ui_window_layer is added to
//  it. The window's graphics context must be current; the layer's new ImGui,
//  ImPlot and imnodes contexts are left current. The layer's renderer keeps
//  its batches' vertices in inout_renderer_storage, which must outlive the
//  layer; when it's NULL, the storage is allocated instead
//
//  in:     in_parent_window, in_width_pixels, in_height_pixels
//  in/out: inout_renderer_storage
//  out:    out_ui_layer
//  ret:    0 on success; 1 when inout_renderer_storage is NULL and the
//            storage can't be allocated, leaving nothing to clean up
uint8_t ui_layer_init(
  struct window_t* in_parent_window,
  struct ui_layer_t* out_ui_layer,
  struct renderer_2d_storage_t* inout_renderer_storage,
  uint32_t in_width_pixels,
  uint32_t in_height_pixels);

// Releases everything a UI layer holds, making its parent window's graphics
//  context current to do so and releasing it after
//
//  in/out: inout_ui_layer
void ui_layer_cleanup(
  struct ui_layer_t* inout_ui_layer);

/*******************************************************************************
 *
 *   HOOKS
 *
 ******************************************************************************/

// Sets the function making the layer's ig*, ImPlot_* and imnodes_* calls.
//  It's called each time the layer runs, within the layer's ImGui frame
//
//  in:     in_function_custom_ui_layer_render
//  in/out: inout_ui_layer
void ui_layer_set_custom_render(
  struct ui_layer_t* inout_ui_layer,
  void (*in_function_custom_ui_layer_render)(struct ui_layer_t* inout_ui_layer));

// Sets a handler for the events reaching the UI layer. The layer owns its
//  window layer's event hook, through which it feeds input to the UI, so use
//  this rather than window_layer_set_event_react() on ui_window_layer. Every
//  event reaches the handler, including those the UI wants exclusive use of;
//  those are marked handled after it returns, keeping them from the layers
//  beneath
//
//  in:     in_function_custom_ui_layer_event_react
//  in/out: inout_ui_layer
void ui_layer_set_custom_event_react(
  struct ui_layer_t* inout_ui_layer,
  void (*in_function_custom_ui_layer_event_react)(
    struct ui_layer_t* inout_ui_layer,
    struct event_base_t* inout_event));
