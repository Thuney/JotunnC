#pragma once

// Cameras hold the view and projection matrices a renderer draws through. An
//  orthographic or perspective camera embeds camera_base_t as base, and is
//  got back from it with container_of(); an unprojected camera is a bare
//  camera_base_t, with identity matrices

#include "fmatrix.h"
#include "fquaternion.h"
#include "fvector.h"

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// The kind of camera, and so the struct that embeds its camera_base_t
enum camera_projection_t
{
  CAMERA_PROJECTION_ORTHOGRAPHIC,
  CAMERA_PROJECTION_PERSPECTIVE,
  CAMERA_PROJECTION_UNPROJECTED
};

struct camera_base_t
{
  enum camera_projection_t projection_type;

  union fmatrix_4x4_t projection_matrix;
  union fmatrix_4x4_t view_matrix;
  union fmatrix_4x4_t view_projection_matrix;

  union fvector3_t position;
  union fvector3_t up;
  union fvector3_t front;

  float near_plane;
  float far_plane;
  // Fits the projection to a window of the given size, with the given clip
  //  planes: an orthographic camera maps the window with its origin at the
  //  bottom-left, and a perspective camera takes its aspect ratio; an
  //  unprojected camera is left as it is. Set by the init functions
  void (*function_reproject)(
    struct camera_base_t* inout_camera,
    float in_window_width,
    float in_window_height,
    float in_near_plane,
    float in_far_plane);
};

struct camera_orthographic_t
{
  struct camera_base_t base;
  //
  float left;
  float right;
  float top;
  float bottom;
};

struct camera_perspective_t
{
  struct camera_base_t base;
  //
  // The vertical angle the camera sees across
  float field_of_view_radians;
  float aspect_ratio;
};

/*******************************************************************************
 *
 *   INITIALIZATION
 *
 ******************************************************************************/

// Sets up a camera with no projection, whose matrices are all identity. Its
//  position, orientation and clip planes are left unset
//
//  out:    out_camera
void camera_unprojected_init(
  struct camera_base_t* out_camera);

// Sets up an orthographic camera's view, at in_position looking along
//  in_front with in_up as up. Its projection is identity until
//  camera_orthographic_set_projection(), which must be called before the
//  camera is reprojected
//
//  in:     in_position, in_up, in_front
//  out:    out_camera
void camera_orthographic_init(
  struct camera_orthographic_t* out_camera,
  union fvector3_t in_position,
  union fvector3_t in_up,
  union fvector3_t in_front);

// Sets up a perspective camera's view, at in_position looking along in_front
//  with in_up as up. Its projection is identity until
//  camera_perspective_set_projection(), which must be called before the
//  camera is reprojected
//
//  in:     in_position, in_up, in_front
//  out:    out_camera
void camera_perspective_init(
  struct camera_perspective_t* out_camera,
  union fvector3_t in_position,
  union fvector3_t in_up,
  union fvector3_t in_front);

/*******************************************************************************
 *
 *   PROJECTION
 *
 ******************************************************************************/

// Gives an orthographic camera a projection mapping the box between the given
//  edges and clip planes to clip space, and updates its view-projection
//  matrix
//
//  in:     in_left, in_right, in_top, in_bottom, in_near_plane, in_far_plane
//  in/out: inout_camera
void camera_orthographic_set_projection(
  struct camera_orthographic_t* inout_camera,
  float in_left,
  float in_right,
  float in_top,
  float in_bottom,
  float in_near_plane,
  float in_far_plane);

// Sets a perspective camera's projection from its vertical field of view,
//  aspect ratio (width over height) and clip planes, and updates its
//  view-projection matrix. The field of view is kept for when the camera is
//  reprojected to a new window size
//
//  in:     in_field_of_view_radians, in_aspect_ratio, in_near_plane,
//            in_far_plane
//  in/out: inout_camera
void camera_perspective_set_projection(
  struct camera_perspective_t* inout_camera,
  float in_field_of_view_radians,
  float in_aspect_ratio,
  float in_near_plane,
  float in_far_plane);

// Recomputes a camera's view-projection matrix from its view and projection
//  matrices, after either has changed
//
//  in/out: inout_camera
void camera_recalculate_view_projection_matrix(
  struct camera_base_t* inout_camera);

/*******************************************************************************
 *
 *   ORIENTATION
 *
 ******************************************************************************/

// Moves a camera to in_position, looking along in_front with in_up as up,
//  and updates its view and view-projection matrices. in_up needn't be
//  perpendicular to in_front, so a fixed world up suits a camera looking up
//  or down, short of straight along it
//
//  in:     in_position, in_up, in_front
//  in/out: inout_camera
void camera_set_view(
  struct camera_base_t* inout_camera,
  union fvector3_t in_position,
  union fvector3_t in_up,
  union fvector3_t in_front);

// Gets the direction a camera faces
//
//  in:     in_camera
//  ret:    The zero vector
union fvector3_t camera_get_front_vector(
  const struct camera_base_t* in_camera);

// Gets the direction to a camera's left
//
//  in:     in_camera
//  ret:    The zero vector
union fvector3_t camera_get_left_vector(
  const struct camera_base_t* in_camera);

// Gets the direction above a camera
//
//  in:     in_camera
//  ret:    The zero vector
union fvector3_t camera_get_up_vector(
  const struct camera_base_t* in_camera);
