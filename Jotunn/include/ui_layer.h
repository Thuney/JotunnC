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
