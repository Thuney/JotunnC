#pragma once

// The multi-window example's controls window, with two layers. The swatch
//  layer draws a strip in the circles' color; the UI layer above it makes
//  ImGui controls that change the shared state the scene window draws, and
//  shows what the scene window's overlay has recorded

#include "application.h"
#include "multi_window_app_state.h"
#include "ui_layer.h"
#include "window_layer_2d.h"

#include <stdint.h>

#define MULTI_WINDOW_APP_CONTROLS_WINDOW_WIDTH_PIXELS 420
#define MULTI_WINDOW_APP_CONTROLS_WINDOW_HEIGHT_PIXELS 360

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// The window and its layers. Its callbacks get back to it from a layer with
//  container_of()
struct multi_window_app_controls_window_t
{
  struct window_t window;
  //
  // Its layers, from the bottom up
  struct window_layer_2d_t swatch_layer;
  struct ui_layer_t ui_layer;
  //
  // Owned by the application
  struct multi_window_app_state_t* state;
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Opens the window and sets up its layers in its graphics context, which is
//  left current. The window isn't added to in_application; it keeps
//  inout_state, which must outlive it
//
//  in:     in_application
//  in/out: inout_state
//  out:    out_controls_window
//  ret:    0 on success; non-zero when the window or its layers can't be set
//            up
uint8_t multi_window_app_controls_window_init(
  struct multi_window_app_controls_window_t* out_controls_window,
  struct application_t* in_application,
  struct multi_window_app_state_t* inout_state);

// Releases what multi_window_app_controls_window_init() set up, in the
//  window's graphics context. The window itself is closed by
//  application_cleanup()
//
//  in/out: inout_controls_window
void multi_window_app_controls_window_cleanup(
  struct multi_window_app_controls_window_t* inout_controls_window);
