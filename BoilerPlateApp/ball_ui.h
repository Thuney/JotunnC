#pragma once

// The ball app's UI: an ImGui layer drawn over the ball window, showing a test
//  window and the ImGui demo window

#include <stdint.h>

#include "ui_layer.h"

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

struct ball_ui_layer_t
{
  struct ui_layer_t ui_layer;
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Sets up the UI layer for a window, sized in_width by in_height, with its
//  renderer's batch storage allocated by the engine. It isn't added to the
//  window: pass its ui_layer.ui_window_layer to window_add_layer() for it to
//  be drawn
//
//  in:     in_width, in_height
//  in/out: inout_parent_window
//  out:    out_ball_layer
//  ret:    0 on success; 1 when the renderer's storage can't be allocated
uint8_t ball_ui_layer_init(
  struct window_t* inout_parent_window,
  struct ball_ui_layer_t* out_ball_layer,
  uint16_t in_width,
  uint16_t in_height);

// Releases what ball_ui_layer_init() set up
//
//  in/out: inout_ball_layer
void ball_ui_layer_cleanup(
  struct ball_ui_layer_t* inout_ball_layer);
