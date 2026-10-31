#pragma once

// The ball app's window: a ball that falls under gravity and bounces off the
//  window's edges, which can be picked up with the left mouse button and
//  thrown. Distances are in the window's pixels and times in seconds

#include "application.h"
#include "ball_ui.h"
#include "camera.h"
#include "event.h"
#include "fmatrix.h"
#include "fvector.h"
#include "renderer_2d.h"

#include <stdbool.h>

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// The ball's size, motion and how it's being handled
struct ball_t
{
  float    radius_pixels;
  union fvector2_t position_pixels;
  union fvector2_t velocity_pixels_per_second;
  union fvector2_t acceleration_pixels_per_second_squared;
  //
  union fvector4_t color;
  //
  bool is_held;
  bool is_grounded;
};

// The window and layer callbacks get back to the ball_window_t from the
//  struct window_t they're given with container_of()
struct ball_window_t
{
  // Base window structure
  struct window_t window;
  //
  struct window_layer_t window_layer;
  struct framebuffer_t framebuffer;
  //
  struct ball_ui_layer_t ball_ui_layer;
  //
  struct camera_orthographic_t camera_ortho;
  struct renderer_2d_t renderer_2d;
  //
  struct ball_t ball;
  //
  float clicked_indicator_radius_pixels;
  union fvector2_t last_click_position_pixels;
  union fvector4_t clicked_color;
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Creates the window with its scene layer and UI layer, its camera and
//  renderer, and puts the ball in its starting place. The caller still has to
//  set ball_window_run() as the custom run of out_ball_window->window_layer
//  and add the window to the application
//
//  in/out: inout_app_parent
//  out:    out_ball_window
//  ret:    0 on success; non-zero when the window can't be created, in which
//            case nothing else is set up
uint8_t ball_window_init(
  struct application_t* inout_app_parent,
  struct ball_window_t* out_ball_window);

// Releases the framebuffer, renderer and UI layer set up by ball_window_init().
//  The window itself is released by application_cleanup()
//
//  in/out: inout_ball_window
void ball_window_cleanup(
  struct ball_window_t* inout_ball_window);

/*******************************************************************************
 *
 *   DRAWING
 *
 ******************************************************************************/

// Draws the ball and a marker where the mouse was last pressed. Set it as the
//  custom run of the ball window's window_layer, whose parent window must be
//  a ball_window_t
//
//  in/out: inout_window_layer
void ball_window_run(
  struct window_layer_t* inout_window_layer);

// Draws a fixed square in the click marker's color. Nothing uses it
//
//  in:     in_window_layer
//  in/out: inout_parent_window
void ball_ui_window_run(
  struct window_t* inout_parent_window,
  const struct window_layer_t* in_window_layer);
