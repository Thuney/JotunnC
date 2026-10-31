#include "camera_orbit.h"

#include "fmath.h"

#include <math.h>

/*******************************************************************************
 *
 *   Function: camera_orbit_init()
 *
 ******************************************************************************/
void camera_orbit_init(
  struct camera_orbit_t* out_orbit,
  union fvector3_t in_target,
  float in_distance,
  float in_yaw_radians,
  float in_pitch_radians)
{
  out_orbit->target = in_target;
  out_orbit->distance = in_distance;
  out_orbit->yaw_radians = in_yaw_radians;
  out_orbit->pitch_radians = in_pitch_radians;

  out_orbit->auto_turn_radians_per_second = 0.0f;
  out_orbit->drag_radians_per_pixel = 0.01f;
  out_orbit->zoom_fraction_per_step = 0.1f;
  // Short of straight up or down, where the camera's up would be undefined
  out_orbit->min_pitch_radians = -1.4f;
  out_orbit->max_pitch_radians = 1.4f;
  out_orbit->min_distance = (in_distance / 4.0f);
  out_orbit->max_distance = (in_distance * 4.0f);

  out_orbit->is_dragging = false;
  out_orbit->last_mouse_position_pixels = (union fvector2_t){{0.0f, 0.0f}};
}

/*******************************************************************************
 *
 *   Function: camera_orbit_calculate_position()
 *
 ******************************************************************************/
union fvector3_t camera_orbit_calculate_position(
  const struct camera_orbit_t* in_orbit)
{
  const float horizontal_distance = (in_orbit->distance * cosf(in_orbit->pitch_radians));

  const union fvector3_t position =
    {{(in_orbit->target.comp.x + (horizontal_distance * sinf(in_orbit->yaw_radians))),
      (in_orbit->target.comp.y + (in_orbit->distance * sinf(in_orbit->pitch_radians))),
      (in_orbit->target.comp.z + (horizontal_distance * cosf(in_orbit->yaw_radians)))}};

  return position;
}

/*******************************************************************************
 *
 *   Function: camera_orbit_apply_to_camera()
 *
 ******************************************************************************/
void camera_orbit_apply_to_camera(
  const struct camera_orbit_t* in_orbit,
  struct camera_base_t* inout_camera)
{
  const union fvector3_t world_up = {{0.0f, 1.0f, 0.0f}};

  const union fvector3_t position = camera_orbit_calculate_position(in_orbit);
  const union fvector3_t to_target =
    fvector3_subtract(
      &(in_orbit->target),
      &(position));

  // The camera works its own up out from world_up, which needn't be
  //  perpendicular to the way it looks
  camera_set_view(
    inout_camera,
    position,
    world_up,
    fvector3_normalize(&(to_target)));
}

/*******************************************************************************
 *
 *   Function: camera_orbit_advance()
 *
 ******************************************************************************/
void camera_orbit_advance(
  struct camera_orbit_t* inout_orbit,
  float in_delta_time_seconds)
{
  if (!inout_orbit->is_dragging) {
    inout_orbit->yaw_radians +=
      (inout_orbit->auto_turn_radians_per_second * in_delta_time_seconds);
  }
}

/*******************************************************************************
 *
 *   Function: camera_orbit_set_dragging()
 *
 ******************************************************************************/
void camera_orbit_set_dragging(
  struct camera_orbit_t* inout_orbit,
  bool in_is_dragging)
{
  inout_orbit->is_dragging = in_is_dragging;
}

/*******************************************************************************
 *
 *   Function: camera_orbit_move_mouse()
 *
 ******************************************************************************/
void camera_orbit_move_mouse(
  struct camera_orbit_t* inout_orbit,
  union fvector2_t in_mouse_position_pixels)
{
  if (inout_orbit->is_dragging) {
    const float delta_x_pixels =
      (in_mouse_position_pixels.comp.x - inout_orbit->last_mouse_position_pixels.comp.x);
    const float delta_y_pixels =
      (in_mouse_position_pixels.comp.y - inout_orbit->last_mouse_position_pixels.comp.y);

    inout_orbit->yaw_radians -= (delta_x_pixels * inout_orbit->drag_radians_per_pixel);
    inout_orbit->pitch_radians =
      fmath_clamp(
        (inout_orbit->pitch_radians - (delta_y_pixels * inout_orbit->drag_radians_per_pixel)),
        inout_orbit->min_pitch_radians,
        inout_orbit->max_pitch_radians);
  }

  inout_orbit->last_mouse_position_pixels = in_mouse_position_pixels;
}

/*******************************************************************************
 *
 *   Function: camera_orbit_zoom()
 *
 ******************************************************************************/
void camera_orbit_zoom(
  struct camera_orbit_t* inout_orbit,
  float in_scroll_steps)
{
  inout_orbit->distance =
    fmath_clamp(
      (inout_orbit->distance * (1.0f - (inout_orbit->zoom_fraction_per_step * in_scroll_steps))),
      inout_orbit->min_distance,
      inout_orbit->max_distance);
}
