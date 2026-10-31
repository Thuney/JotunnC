#include "camera.h"

#include "util.h"

#include <math.h>

#ifdef DEBUG
  #include <stdio.h>
#endif

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Fits a camera's projection to a window of the given size, with the given
//  clip planes. Orthographic cameras map the window with its origin at the
//  bottom-left, perspective cameras take its aspect ratio, and unprojected
//  cameras are left as they are. Assigned to camera_base_t's
//  function_reproject
//
//  in:     in_window_width, in_window_height, in_near_plane, in_far_plane
//  in/out: inout_camera
static void camera_reproject(
  struct camera_base_t* inout_camera,
  float in_window_width,
  float in_window_height,
  float in_near_plane,
  float in_far_plane);

// Builds the matrix translating by -in_position, moving a camera at
//  in_position to the origin
//
//  in:     in_position
//  ret:    The translation matrix
static union fmatrix_4x4_t camera_position_to_matrix(
  union fvector3_t in_position);

// Builds the view matrix of a camera at in_position looking at in_target,
//  with up taken from in_up made perpendicular to the view direction
//
//  in:     in_position, in_target, in_up
//  ret:    The view matrix
static union fmatrix_4x4_t camera_calculate_look_at_matrix(
  union fvector3_t in_position,
  union fvector3_t in_target,
  union fvector3_t in_up);

// Sets up a camera's view, at in_position looking along in_front with in_up
//  as up, with an identity projection, and assigns its reproject function.
//  The projection type and clip planes are left for the set_projection
//  functions
//
//  in:     in_position, in_up, in_front
//  out:    out_camera
static void camera_init(
  struct camera_base_t* out_camera,
  union fvector3_t in_position,
  union fvector3_t in_up,
  union fvector3_t in_front);

/*******************************************************************************
 *
 *   Function: camera_unprojected_init()
 *
 ******************************************************************************/
void camera_unprojected_init(
  struct camera_base_t* out_camera)
{
  out_camera->projection_type = CAMERA_PROJECTION_UNPROJECTED;

  fmatrix_4x4_init(&(out_camera->projection_matrix));
  fmatrix_4x4_init(&(out_camera->view_matrix));
  fmatrix_4x4_init(&(out_camera->view_projection_matrix));

  out_camera->function_reproject = &(camera_reproject);
}

/*******************************************************************************
 *
 *   Function: camera_orthographic_init()
 *
 ******************************************************************************/
void camera_orthographic_init(
  struct camera_orthographic_t* out_camera,
  union fvector3_t in_position,
  union fvector3_t in_up,
  union fvector3_t in_front)
{
  camera_init(
    &(out_camera->base),
    in_position,
    in_up,
    in_front);
}

/*******************************************************************************
 *
 *   Function: camera_orthographic_init_pixel_space()
 *
 ******************************************************************************/
void camera_orthographic_init_pixel_space(
  struct camera_orthographic_t* out_camera,
  uint32_t in_width_pixels,
  uint32_t in_height_pixels)
{
  const float near_plane = -3.0f;
  const float far_plane = 100.0f;

  camera_orthographic_init(
    out_camera,
    (union fvector3_t){{0.0f, 0.0f, 2.0f}},
    (union fvector3_t){{0.0f, 1.0f, 0.0f}},
    (union fvector3_t){{0.0f, 0.0f, -1.0f}});

  // Left, right, top and bottom edges, in pixels
  camera_orthographic_set_projection(
    out_camera,
    0.0f,
    (float)in_width_pixels,
    (float)in_height_pixels,
    0.0f,
    near_plane,
    far_plane);
}

/*******************************************************************************
 *
 *   Function: camera_perspective_init()
 *
 ******************************************************************************/
void camera_perspective_init(
  struct camera_perspective_t* out_camera,
  union fvector3_t in_position,
  union fvector3_t in_up,
  union fvector3_t in_front)
{
  camera_init(
    &(out_camera->base),
    in_position,
    in_up,
    in_front);
}

/*******************************************************************************
 *
 *   Function: camera_orthographic_set_projection()
 *
 ******************************************************************************/
void camera_orthographic_set_projection(
  struct camera_orthographic_t* inout_camera,
  float in_left,
  float in_right,
  float in_top,
  float in_bottom,
  float in_near_plane,
  float in_far_plane)
{
  struct camera_base_t* camera_base = &(inout_camera->base);

  camera_base->near_plane = in_near_plane;
  camera_base->far_plane = in_far_plane;

  inout_camera->left = in_left;
  inout_camera->right = in_right;
  inout_camera->top = in_top;
  inout_camera->bottom = in_bottom;

  camera_base->projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;

  const float ortho_matrix[4][4] = {
    {(2.0f / (in_right - in_left)), 0.0f, 0.0f, -((in_right + in_left) / (in_right - in_left))},
    {0.0f, 2.0f / (in_top - in_bottom), 0.0f, -((in_top + in_bottom) / (in_top - in_bottom))},
    {0.0f,
     0.0f,
     -2.0f / (in_far_plane - in_near_plane),
     -((in_far_plane + in_near_plane) / (in_far_plane - in_near_plane))},
    {0.0f, 0.0f, 0.0f, 1.0f}};

  fmatrix_4x4_set(
    &(camera_base->projection_matrix),
    ortho_matrix);

  fmatrix_4x4_transpose(&(camera_base->projection_matrix));

  // #ifdef DEBUG
  //    fmatrix_4x4_print(&camera_base->projection_matrix);
  // #endif

  camera_recalculate_view_projection_matrix(camera_base);
}

/*******************************************************************************
 *
 *   Function: camera_perspective_set_projection()
 *
 ******************************************************************************/
void camera_perspective_set_projection(
  struct camera_perspective_t* inout_camera,
  float in_field_of_view_radians,
  float in_aspect_ratio,
  float in_near_plane,
  float in_far_plane)
{
  struct camera_base_t* camera_base = &(inout_camera->base);

  camera_base->near_plane = in_near_plane;
  camera_base->far_plane = in_far_plane;

  inout_camera->field_of_view_radians = in_field_of_view_radians;
  inout_camera->aspect_ratio = in_aspect_ratio;

  camera_base->projection_type = CAMERA_PROJECTION_PERSPECTIVE;

  // The distance at which the field of view spans two units, the height of
  //  clip space
  const float focal_length = (1.0f / tanf(in_field_of_view_radians / 2.0f));
  const float depth_range = (in_near_plane - in_far_plane);

  const float persp_matrix[4][4] = {
    {(focal_length / in_aspect_ratio), 0.0f, 0.0f, 0.0f},
    {0.0f, focal_length, 0.0f, 0.0f},
    {0.0f,
     0.0f,
     ((in_far_plane + in_near_plane) / depth_range),
     ((2.0f * in_far_plane * in_near_plane) / depth_range)},
    {0.0f, 0.0f, -1.0f, 0.0f}};

  fmatrix_4x4_set(
    &(camera_base->projection_matrix),
    persp_matrix);
  fmatrix_4x4_transpose(&(camera_base->projection_matrix));

  camera_recalculate_view_projection_matrix(camera_base);
}

/*******************************************************************************
 *
 *   Function: camera_set_view()
 *
 ******************************************************************************/
void camera_set_view(
  struct camera_base_t* inout_camera,
  union fvector3_t in_position,
  union fvector3_t in_up,
  union fvector3_t in_front)
{
  fvector3_setv(
    &(inout_camera->position),
    in_position);
  fvector3_setv(
    &(inout_camera->up),
    in_up);
  fvector3_setv(
    &(inout_camera->front),
    in_front);

  const union fvector3_t target =
    fvector3_add(
      &(in_position),
      &(in_front));

  inout_camera->view_matrix =
    camera_calculate_look_at_matrix(
      in_position,
      target,
      in_up);
  camera_recalculate_view_projection_matrix(inout_camera);
}

/*******************************************************************************
 *
 *   Function: camera_recalculate_view_projection_matrix()
 *
 ******************************************************************************/
void camera_recalculate_view_projection_matrix(
  struct camera_base_t* inout_camera)
{
  inout_camera->view_projection_matrix =
    fmatrix_4x4_multiply(
      &(inout_camera->view_matrix),
      &(inout_camera->projection_matrix));
}

/*******************************************************************************
 *
 *   Function: camera_get_front_vector()
 *
 ******************************************************************************/
union fvector3_t camera_get_front_vector(
  const struct camera_base_t* in_camera)
{
  union fvector3_t front;
  fvector3_init(&(front));

  return front;
}

/*******************************************************************************
 *
 *   Function: camera_get_left_vector()
 *
 ******************************************************************************/
union fvector3_t camera_get_left_vector(
  const struct camera_base_t* in_camera)
{
  union fvector3_t left;
  fvector3_init(&(left));

  return left;
}

/*******************************************************************************
 *
 *   Function: camera_get_up_vector()
 *
 ******************************************************************************/
union fvector3_t camera_get_up_vector(
  const struct camera_base_t* in_camera)
{
  union fvector3_t up;
  fvector3_init(&(up));

  return up;
}

/*******************************************************************************
 *
 *   Function: camera_reproject()
 *
 ******************************************************************************/
static void camera_reproject(
  struct camera_base_t* inout_camera,
  float in_window_width,
  float in_window_height,
  float in_near_plane,
  float in_far_plane)
{
  switch (inout_camera->projection_type)
  {
    case CAMERA_PROJECTION_ORTHOGRAPHIC:
    {
      struct camera_orthographic_t* camera_ortho =
        container_of(
          inout_camera,
          struct camera_orthographic_t,
          base);
      camera_orthographic_set_projection(
        camera_ortho,
        0.0f,
        in_window_width,
        in_window_height,
        0.0f,
        in_near_plane,
        in_far_plane);
    }
    break;
    case CAMERA_PROJECTION_PERSPECTIVE:
    {
      struct camera_perspective_t* camera_perspective =
        container_of(
          inout_camera,
          struct camera_perspective_t,
          base);
      camera_perspective_set_projection(
        camera_perspective,
        camera_perspective->field_of_view_radians,
        in_window_width / in_window_height,
        in_near_plane,
        in_far_plane);
    }
    break;
    case CAMERA_PROJECTION_UNPROJECTED:
    {
    }
    break;
  }
}

/*******************************************************************************
 *
 *   Function: camera_position_to_matrix()
 *
 ******************************************************************************/
static union fmatrix_4x4_t camera_position_to_matrix(
  union fvector3_t in_position)
{
  union fmatrix_4x4_t position_as_matrix;

  const float position_as_matrix_data[4][4] = {
    {1.0f, 0.0f, 0.0f, -in_position.comp.x},
    {0.0f, 1.0f, 0.0f, -in_position.comp.y},
    {0.0f, 0.0f, 1.0f, -in_position.comp.z},
    {0.0f, 0.0f, 0.0f, 1.0f}};

  fmatrix_4x4_set(
    &(position_as_matrix),
    position_as_matrix_data);

  return position_as_matrix;
}

/*******************************************************************************
 *
 *   Function: camera_calculate_look_at_matrix()
 *
 ******************************************************************************/
static union fmatrix_4x4_t camera_calculate_look_at_matrix(
  union fvector3_t in_position,
  union fvector3_t in_target,
  union fvector3_t in_up)
{
  union fvector3_t direction, right, camera_up;

  union fvector3_t pos_minus_target, up_cross_direction;

  pos_minus_target = fvector3_subtract(
    &(in_position),
    &(in_target));
  direction = fvector3_normalize(&(pos_minus_target));

  up_cross_direction = fvector3_cross(
    &(in_up),
    &(direction));
  right = fvector3_normalize(&(up_cross_direction));

  // in_up need not be perpendicular to the view direction, so the up used is
  //  derived from the other two, keeping the basis orthonormal
  camera_up = fvector3_cross(
    &(direction),
    &(right));

  union fmatrix_4x4_t coordinate_matrix;

  const float coordinate_matrix_data[4][4] = {
    {right.comp.x, right.comp.y, right.comp.z, 0.0f},
    {camera_up.comp.x, camera_up.comp.y, camera_up.comp.z, 0.0f},
    {direction.comp.x, direction.comp.y, direction.comp.z, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};

  fmatrix_4x4_set(
    &(coordinate_matrix),
    coordinate_matrix_data);

  union fmatrix_4x4_t pos_matrix = camera_position_to_matrix(in_position);

  union fmatrix_4x4_t look_at_matrix = fmatrix_4x4_multiply(
    &(coordinate_matrix),
    &(pos_matrix));
  fmatrix_4x4_transpose(&(look_at_matrix));

  return look_at_matrix;
}

/*******************************************************************************
 *
 *   Function: camera_init()
 *
 ******************************************************************************/
static void camera_init(
  struct camera_base_t* out_camera,
  union fvector3_t in_position,
  union fvector3_t in_up,
  union fvector3_t in_front)
{
  fmatrix_4x4_init(&(out_camera->projection_matrix));
  fmatrix_4x4_init(&(out_camera->view_matrix));
  fmatrix_4x4_init(&(out_camera->view_projection_matrix));

  camera_set_view(
    out_camera,
    in_position,
    in_up,
    in_front);

  out_camera->function_reproject = &(camera_reproject);
}
