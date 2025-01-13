#pragma once

#include <stdint.h>

#include "ui_layer_clay.h"

struct ball_ui_layer_clay_t
{
  struct ui_layer_clay_t ui_layer;
};

void ball_ui_layer_clay_init(struct ball_ui_layer_clay_t* ball_layer, uint16_t width, uint16_t height, struct window_t* parent_window);

void ball_ui_layer_clay_cleanup(struct ball_ui_layer_clay_t* ball_layer);