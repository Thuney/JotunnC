#pragma once

// What the multi-window example's two windows share. The controls window
//  sets how the scene window's circles look and move; the scene window's
//  overlay records where the mouse is and where it's been clicked, for the
//  controls window to show. The application owns it, and each window keeps a
//  pointer to it

#include "fvector.h"

#include <stdbool.h>

#define MULTI_WINDOW_APP_STATE_MAX_CIRCLES 24
// Older markers make way for new ones past this many
#define MULTI_WINDOW_APP_STATE_MAX_MARKERS 32

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

struct multi_window_app_state_t
{
  // Set from the controls window, and drawn in the scene window
  int circle_count;
  float orbit_speed_radians_per_second;
  // Red, green and blue, edited through vec
  union fvector3_t circle_color;
  //
  // Set from the scene window's overlay, and shown in the controls window
  bool is_mouse_over_scene;
  union fvector2_t mouse_position_pixels;
  union fvector2_t marker_positions_pixels[MULTI_WINDOW_APP_STATE_MAX_MARKERS];
  unsigned int marker_count;
  unsigned int next_marker_index;
};

/*******************************************************************************
 *
 *   SETUP
 *
 ******************************************************************************/

// Gives the state its starting values, with no markers
//
//  out:    out_state
static inline void multi_window_app_state_init(
  struct multi_window_app_state_t* out_state);

/*******************************************************************************
 *
 *   MARKERS
 *
 ******************************************************************************/

// Drops a marker, replacing the oldest once there are
//  MULTI_WINDOW_APP_STATE_MAX_MARKERS
//
//  in:     in_position_pixels
//  in/out: inout_state
static inline void multi_window_app_state_add_marker(
  struct multi_window_app_state_t* inout_state,
  union fvector2_t in_position_pixels);

// Removes every marker
//
//  in/out: inout_state
static inline void multi_window_app_state_clear_markers(
  struct multi_window_app_state_t* inout_state);

/*******************************************************************************
 *
 *   INLINE DEFINITIONS
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *   Function: multi_window_app_state_init()
 *
 ******************************************************************************/
static inline void multi_window_app_state_init(
  struct multi_window_app_state_t* out_state)
{
  out_state->circle_count = 12;
  out_state->orbit_speed_radians_per_second = 0.8f;
  out_state->circle_color = (union fvector3_t){{0.35f, 0.75f, 0.95f}};

  out_state->is_mouse_over_scene = false;
  out_state->mouse_position_pixels = (union fvector2_t){{0.0f, 0.0f}};
  multi_window_app_state_clear_markers(out_state);
}

/*******************************************************************************
 *
 *   Function: multi_window_app_state_add_marker()
 *
 ******************************************************************************/
static inline void multi_window_app_state_add_marker(
  struct multi_window_app_state_t* inout_state,
  union fvector2_t in_position_pixels)
{
  inout_state->marker_positions_pixels[inout_state->next_marker_index] = in_position_pixels;
  inout_state->next_marker_index =
    ((inout_state->next_marker_index + 1) % MULTI_WINDOW_APP_STATE_MAX_MARKERS);

  if (inout_state->marker_count < MULTI_WINDOW_APP_STATE_MAX_MARKERS) {
    inout_state->marker_count++;
  }
}

/*******************************************************************************
 *
 *   Function: multi_window_app_state_clear_markers()
 *
 ******************************************************************************/
static inline void multi_window_app_state_clear_markers(
  struct multi_window_app_state_t* inout_state)
{
  inout_state->marker_count = 0;
  inout_state->next_marker_index = 0;
}
