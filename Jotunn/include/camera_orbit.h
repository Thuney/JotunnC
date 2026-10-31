#pragma once

// An orbiting camera controller: it keeps a camera on a sphere about a
//  target, looking at it. Dragging turns it around the target and up and
//  down, scrolling moves it in and out, and it can turn slowly by itself
//  while it isn't being dragged. It knows nothing of events; the application
//  feeds it the mouse's input and the time passing, and applies it to a
//  camera before the camera's scene is drawn

#include "camera.h"
#include "fvector.h"

#include <stdbool.h>

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// Where the camera sits about its target, how the orbit responds to input,
//  and the drag in progress
struct camera_orbit_t
{
  union fvector3_t target;
  float distance;
  // Around the vertical axis, and up from the ground
  float yaw_radians;
  float pitch_radians;
  //
  // How the orbit responds, given defaults by camera_orbit_init() and free
  //  to change after
  float auto_turn_radians_per_second;
  float drag_radians_per_pixel;
  // The share of the distance each step scrolled moves the camera in or out
  float zoom_fraction_per_step;
  float min_pitch_radians;
  float max_pitch_radians;
  float min_distance;
  float max_distance;
  //
  bool is_dragging;
  // Where the mouse last was over the window, from its bottom-left corner
  union fvector2_t last_mouse_position_pixels;
};

/*******************************************************************************
 *
 *   SETUP
 *
 ******************************************************************************/

// Places an orbit in_distance from in_target, turned in_yaw_radians around
//  it and raised in_pitch_radians, with no drag in progress. It doesn't turn
//  by itself; dragging turns it 0.01 radians a pixel, short of looking
//  straight up or down (1.4 radians either way); and each step scrolled moves
//  it a tenth of its distance, kept between a quarter and four times
//  in_distance
//
//  in:     in_target, in_distance, in_yaw_radians, in_pitch_radians
//  out:    out_orbit
void camera_orbit_init(
  struct camera_orbit_t* out_orbit,
  union fvector3_t in_target,
  float in_distance,
  float in_yaw_radians,
  float in_pitch_radians);

/*******************************************************************************
 *
 *   CAMERA
 *
 ******************************************************************************/

// Finds where the orbit puts the camera
//
//  in:     in_orbit
//  ret:    The camera's position
union fvector3_t camera_orbit_calculate_position(
  const struct camera_orbit_t* in_orbit);

// Moves a camera to where the orbit puts it, looking at the target, with the
//  world's +Y as up. A renderer draws its batches through the camera as it
//  is when they're drawn, so apply the orbit before a scene's draws are made
//
//  in:     in_orbit
//  in/out: inout_camera
void camera_orbit_apply_to_camera(
  const struct camera_orbit_t* in_orbit,
  struct camera_base_t* inout_camera);

/*******************************************************************************
 *
 *   INPUT
 *
 ******************************************************************************/

// Turns the orbit around its target by auto_turn_radians_per_second as time
//  passes, unless it's being dragged
//
//  in:     in_delta_time_seconds
//  in/out: inout_orbit
void camera_orbit_advance(
  struct camera_orbit_t* inout_orbit,
  float in_delta_time_seconds);

// Starts or ends a drag
//
//  in:     in_is_dragging
//  in/out: inout_orbit
void camera_orbit_set_dragging(
  struct camera_orbit_t* inout_orbit,
  bool in_is_dragging);

// Follows the mouse to a new position over the window, turning the orbit by
//  how far it moved while a drag is in progress, its pitch kept within
//  min_pitch_radians and max_pitch_radians
//
//  in:     in_mouse_position_pixels
//  in/out: inout_orbit
void camera_orbit_move_mouse(
  struct camera_orbit_t* inout_orbit,
  union fvector2_t in_mouse_position_pixels);

// Moves the orbit in by zoom_fraction_per_step of its distance for each step
//  scrolled forward, and out for each step back, its distance kept within
//  min_distance and max_distance
//
//  in:     in_scroll_steps
//  in/out: inout_orbit
void camera_orbit_zoom(
  struct camera_orbit_t* inout_orbit,
  float in_scroll_steps);
