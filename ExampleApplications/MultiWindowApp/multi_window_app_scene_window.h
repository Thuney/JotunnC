#pragma once

// The multi-window example's scene window, with two layers. The scene layer
//  draws a ring of circles as the shared state says; the overlay layer above
//  it follows the mouse, and takes left clicks for itself, dropping a marker
//  where each lands. Being the top layer, the overlay is offered the window's
//  events before the scene layer and the window itself are

#include "application.h"
#include "font.h"
#include "multi_window_app_state.h"
#include "window_layer_2d.h"

#include <stdint.h>

#define MULTI_WINDOW_APP_SCENE_WINDOW_WIDTH_PIXELS 800
#define MULTI_WINDOW_APP_SCENE_WINDOW_HEIGHT_PIXELS 600

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// The window, its layers and what they draw with. Its callbacks get back to
//  it from a layer with container_of()
struct multi_window_app_scene_window_t
{
  struct window_t window;
  //
  // Its layers, from the bottom up
  struct window_layer_2d_t scene_layer;
  struct window_layer_2d_t overlay_layer;
  // Made in this window's graphics context, so drawn only in this window
  struct font_typeface_t overlay_typeface;
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
//  out:    out_scene_window
//  ret:    0 on success; non-zero when the window or its layers can't be set
//            up
uint8_t multi_window_app_scene_window_init(
  struct multi_window_app_scene_window_t* out_scene_window,
  struct application_t* in_application,
  struct multi_window_app_state_t* inout_state);

// Releases what multi_window_app_scene_window_init() set up, in the window's
//  graphics context. The window itself is closed by application_cleanup()
//
//  in/out: inout_scene_window
void multi_window_app_scene_window_cleanup(
  struct multi_window_app_scene_window_t* inout_scene_window);
