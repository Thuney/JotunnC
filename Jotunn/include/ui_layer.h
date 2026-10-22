#pragma once

#include "event.h"
#include "renderer_2d.h"
#include "ui_element.h"
#include "window_layer.h"

struct ui_layer_t
{
  struct window_layer_t ui_window_layer;
  struct framebuffer_t ui_framebuffer;

  struct camera_ortho_t ui_camera_ortho;
  struct renderer_2d_t ui_renderer_2d;

  // Managed by implementation
  void* ui_layer_data;
  void (*function_custom_ui_layer_render)(struct ui_layer_t* ui_layer);
  void (*function_custom_ui_layer_event_react)(
    struct ui_layer_t* inout_ui_layer,
    struct event_base_t* inout_event);
};

//
void ui_layer_init(
  struct window_t* parent_window,
  struct ui_layer_t* ui_layer,
  uint32_t width,
  uint32_t height);

void ui_layer_cleanup(struct ui_layer_t* ui_layer);

void ui_layer_set_custom_render(
  struct ui_layer_t* ui_layer,
  void (*function_custom_ui_layer_render)(struct ui_layer_t* ui_layer));

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
