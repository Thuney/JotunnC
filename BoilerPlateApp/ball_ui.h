#pragma once

#include <stdint.h>

#include "ui_layer.h"

struct ball_ui_layer_t
{
  struct ui_layer_t ui_layer;
};

void ball_ui_layer_init(
  struct window_t *parent_window,
  struct ball_ui_layer_t *ball_layer,
  uint16_t width,
  uint16_t height);

void ball_ui_layer_cleanup(
  struct ball_ui_layer_t *ball_layer);
