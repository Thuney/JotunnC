#include "renderer_3d_app_scene.h"
#include "renderer_3d_app.h"

#include "color.h"
#include "fmath.h"
#include "util.h"

#include <math.h>

// How far the grid reaches from the origin each way, in units
#define RENDERER_3D_APP_SCENE_GRID_HALF_EXTENT 8
// How far the Y axis rises from the grid
#define RENDERER_3D_APP_SCENE_AXIS_LENGTH 3.5f

// How many shapes ride around the ring, and how far out
#define RENDERER_3D_APP_SCENE_RING_SHAPE_COUNT 9
#define RENDERER_3D_APP_SCENE_RING_RADIUS 4.5f

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Draws a cube tumbling about a diagonal, textured with
//  renderer_3d_draw_textured_cube(), inside a wireframe cage turning the
//  other way, drawn with renderer_3d_draw_unfilled_cube()
//
//  in:     in_elapsed_seconds
//  in/out: inout_app
static void renderer_3d_app_scene_draw_centerpiece(
  struct renderer_3d_app_t* inout_app,
  float in_elapsed_seconds);

// Draws the ring of shapes, each spinning and bobbing as the ring turns
//
//  in:     in_elapsed_seconds
//  in/out: inout_renderer
static void renderer_3d_app_scene_draw_ring(
  struct renderer_3d_t* inout_renderer,
  float in_elapsed_seconds);

// Draws one of the ring's shapes, choosing between
//  renderer_3d_draw_cube(), renderer_3d_draw_pyramid() and
//  renderer_3d_draw_sphere() in turn by its place in the ring
//
//  in:     in_index, in_position, in_spin_radians, in_color
//  in/out: inout_renderer
static void renderer_3d_app_scene_draw_ring_shape(
  struct renderer_3d_t* inout_renderer,
  unsigned int in_index,
  union fvector3_t in_position,
  float in_spin_radians,
  union fvector4_t in_color);

/*******************************************************************************
 *
 *   Function: renderer_3d_app_scene_run()
 *
 ******************************************************************************/
void renderer_3d_app_scene_run(
  struct window_layer_t* inout_window_layer)
{
  struct renderer_3d_app_t* app =
    container_of(
      inout_window_layer,
      struct renderer_3d_app_t,
      scene_layer.window_layer);
  struct renderer_3d_t* renderer = &(app->scene_layer.renderer_3d);

  const float elapsed_seconds = (float)application_get_elapsed_seconds(&(app->application));

  // Draws batch until the scene ends, so the camera is placed first
  camera_orbit_apply_to_camera(
    &(app->orbit),
    &(app->scene_layer.camera_perspective.base));

  // The grid on the ground, with the X and Z axes through it, and the Y axis
  //  rising from it
  renderer_3d_draw_grid(
    renderer,
    RENDERER_3D_APP_SCENE_GRID_HALF_EXTENT,
    (union fvector4_t){{0.35f, 0.35f, 0.4f, 1.0f}});
  renderer_3d_draw_axes(
    renderer,
    RENDERER_3D_APP_SCENE_AXIS_LENGTH);

  renderer_3d_app_scene_draw_centerpiece(
    app,
    elapsed_seconds);
  renderer_3d_app_scene_draw_ring(
    renderer,
    elapsed_seconds);
}

/*******************************************************************************
 *
 *   Function: renderer_3d_app_scene_draw_centerpiece()
 *
 ******************************************************************************/
static void renderer_3d_app_scene_draw_centerpiece(
  struct renderer_3d_app_t* inout_app,
  float in_elapsed_seconds)
{
  struct renderer_3d_t* renderer = &(inout_app->scene_layer.renderer_3d);

  const union fvector3_t position = {{0.0f, 1.5f, 0.0f}};
  const union fvector3_t diagonal_axis = {{1.0f, 1.0f, 0.0f}};

  // The cube primitive spans -0.5 to 0.5 on each axis, so it's scaled by the
  //  size wanted
  const union fmatrix_4x4_t cube_transform =
    fmatrix_4x4_transform_compose(
      (union fvector3_t){{1.5f, 1.5f, 1.5f}},
      diagonal_axis,
      (0.7f * in_elapsed_seconds),
      position);
  const union fmatrix_4x4_t cage_transform =
    fmatrix_4x4_transform_compose(
      (union fvector3_t){{2.8f, 2.8f, 2.8f}},
      diagonal_axis,
      (-0.4f * in_elapsed_seconds),
      position);

  // The placeholder texture when the image couldn't be loaded
  renderer_3d_draw_textured_cube(
    renderer,
    &(cube_transform),
    &(inout_app->cube_texture));

  renderer_3d_draw_unfilled_cube(
    renderer,
    &(cage_transform),
    (union fvector4_t){{1.0f, 0.85f, 0.3f, 1.0f}});
}

/*******************************************************************************
 *
 *   Function: renderer_3d_app_scene_draw_ring()
 *
 ******************************************************************************/
static void renderer_3d_app_scene_draw_ring(
  struct renderer_3d_t* inout_renderer,
  float in_elapsed_seconds)
{
  const float t = in_elapsed_seconds;
  const float ring_turn_radians_per_second = 0.35f;
  const float spin_radians_per_second = 1.2f;
  const float bob_radians_per_second = 1.5f;

  for (unsigned int i = 0; i < RENDERER_3D_APP_SCENE_RING_SHAPE_COUNT; i++) {
    // Each shape's share of the way around the ring, which also sets its hue
    const float fraction = ((float)i / (float)RENDERER_3D_APP_SCENE_RING_SHAPE_COUNT);
    const float ring_angle_radians =
      ((2.0f * FMATH_PI * fraction) + (ring_turn_radians_per_second * t));

    const union fvector3_t position =
      {{(RENDERER_3D_APP_SCENE_RING_RADIUS * cosf(ring_angle_radians)),
        (1.0f + (0.5f * sinf((bob_radians_per_second * t) + (float)i))),
        (RENDERER_3D_APP_SCENE_RING_RADIUS * sinf(ring_angle_radians))}};

    // A bright color of the shape's hue
    const union fvector4_t color =
      color_from_hsv(
        fraction,
        0.65f,
        0.95f,
        1.0f);

    renderer_3d_app_scene_draw_ring_shape(
      inout_renderer,
      i,
      position,
      ((spin_radians_per_second * t) + (float)i),
      color);
  }
}

/*******************************************************************************
 *
 *   Function: renderer_3d_app_scene_draw_ring_shape()
 *
 ******************************************************************************/
static void renderer_3d_app_scene_draw_ring_shape(
  struct renderer_3d_t* inout_renderer,
  unsigned int in_index,
  union fvector3_t in_position,
  float in_spin_radians,
  union fvector4_t in_color)
{
  const union fvector3_t y_axis = {{0.0f, 1.0f, 0.0f}};

  switch (in_index % 3)
  {
    case 0:
    {
      const union fmatrix_4x4_t transform =
        fmatrix_4x4_transform_compose(
          (union fvector3_t){{0.8f, 0.8f, 0.8f}},
          y_axis,
          in_spin_radians,
          in_position);

      renderer_3d_draw_cube(
        inout_renderer,
        &(transform),
        in_color);
    }
    break;

    case 1:
    {
      // The pyramid stands in the same box as the cube, apex up
      const union fmatrix_4x4_t transform =
        fmatrix_4x4_transform_compose(
          (union fvector3_t){{0.9f, 1.1f, 0.9f}},
          y_axis,
          in_spin_radians,
          in_position);

      renderer_3d_draw_pyramid(
        inout_renderer,
        &(transform),
        in_color);
    }
    break;

    default:
    {
      // The sphere has a radius of 1, so it's scaled by the radius wanted
      const union fmatrix_4x4_t transform =
        fmatrix_4x4_transform_compose(
          (union fvector3_t){{0.5f, 0.5f, 0.5f}},
          y_axis,
          in_spin_radians,
          in_position);

      renderer_3d_draw_sphere(
        inout_renderer,
        &(transform),
        in_color);
    }
    break;
  }
}
