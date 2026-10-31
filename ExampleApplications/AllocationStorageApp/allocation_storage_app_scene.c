#include "allocation_storage_app_scene.h"
#include "allocation_storage_app.h"

#include "util.h"

// The bar's place along the bottom of the window
#define ALLOCATION_STORAGE_APP_SCENE_MARGIN_PIXELS 20.0f
#define ALLOCATION_STORAGE_APP_SCENE_BAR_HEIGHT_PIXELS 28.0f
// How far the peak's tick reaches past the bar, above and below
#define ALLOCATION_STORAGE_APP_SCENE_TICK_OVERHANG_PIXELS 6.0f

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Draws the outline of the whole pool's bar
//
//  in:     in_bar_width_pixels
//  in/out: inout_renderer
static void allocation_storage_app_scene_draw_pool_outline(
  struct renderer_2d_t* inout_renderer,
  float in_bar_width_pixels);

// Fills the part of the bar in use, from its left end
//
//  in:     in_bar_width_pixels, in_used_fraction
//  in/out: inout_renderer
static void allocation_storage_app_scene_draw_pool_used(
  struct renderer_2d_t* inout_renderer,
  float in_bar_width_pixels,
  float in_used_fraction);

// Draws a tick across the bar at the most of the pool that's been in use at
//  once
//
//  in:     in_bar_width_pixels, in_peak_fraction
//  in/out: inout_renderer
static void allocation_storage_app_scene_draw_peak_tick(
  struct renderer_2d_t* inout_renderer,
  float in_bar_width_pixels,
  float in_peak_fraction);

// Finds the transform that stretches the unit square into a bar
//  in_width_pixels long, its bottom-left corner at the bar's place
//
//  in:     in_width_pixels
//  ret:    The transform
static inline union fmatrix_4x4_t allocation_storage_app_scene_calculate_bar_transform(
  float in_width_pixels);

/*******************************************************************************
 *
 *   Function: allocation_storage_app_scene_run()
 *
 ******************************************************************************/
void allocation_storage_app_scene_run(
  struct window_layer_t* inout_window_layer)
{
  struct allocation_storage_app_t* app =
    container_of(
      inout_window_layer,
      struct allocation_storage_app_t,
      scene_layer.window_layer);

  // The pool's fields are the allocator's to change, but fine to read
  const struct allocator_pool_t* pool = &(app->memory.pool);

  const float bar_width_pixels =
    ((float)app->window.metadata.width - (2.0f * ALLOCATION_STORAGE_APP_SCENE_MARGIN_PIXELS));
  const float used_fraction = ((float)pool->used_bytes / (float)pool->size_bytes);
  const float peak_fraction = ((float)pool->peak_used_bytes / (float)pool->size_bytes);

  allocation_storage_app_scene_draw_pool_outline(
    &(app->scene_layer.renderer_2d),
    bar_width_pixels);
  allocation_storage_app_scene_draw_pool_used(
    &(app->scene_layer.renderer_2d),
    bar_width_pixels,
    used_fraction);
  allocation_storage_app_scene_draw_peak_tick(
    &(app->scene_layer.renderer_2d),
    bar_width_pixels,
    peak_fraction);
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_scene_draw_pool_outline()
 *
 ******************************************************************************/
static void allocation_storage_app_scene_draw_pool_outline(
  struct renderer_2d_t* inout_renderer,
  float in_bar_width_pixels)
{
  const union fmatrix_4x4_t transform =
    allocation_storage_app_scene_calculate_bar_transform(in_bar_width_pixels);

  renderer_2d_draw_unfilled_quad(
    inout_renderer,
    &(transform),
    (union fvector4_t){{0.5f, 0.5f, 0.55f, 1.0f}});
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_scene_draw_pool_used()
 *
 ******************************************************************************/
static void allocation_storage_app_scene_draw_pool_used(
  struct renderer_2d_t* inout_renderer,
  float in_bar_width_pixels,
  float in_used_fraction)
{
  if (in_used_fraction <= 0.0f) {
    return;
  }

  const union fmatrix_4x4_t transform =
    allocation_storage_app_scene_calculate_bar_transform(
      (in_bar_width_pixels * in_used_fraction));

  renderer_2d_draw_quad(
    inout_renderer,
    &(transform),
    (union fvector4_t){{0.3f, 0.65f, 0.4f, 1.0f}});
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_scene_draw_peak_tick()
 *
 ******************************************************************************/
static void allocation_storage_app_scene_draw_peak_tick(
  struct renderer_2d_t* inout_renderer,
  float in_bar_width_pixels,
  float in_peak_fraction)
{
  const float margin_pixels = ALLOCATION_STORAGE_APP_SCENE_MARGIN_PIXELS;
  const float overhang_pixels = ALLOCATION_STORAGE_APP_SCENE_TICK_OVERHANG_PIXELS;

  const float x_pixels = (margin_pixels + (in_bar_width_pixels * in_peak_fraction));
  const float bottom_y_pixels = (margin_pixels - overhang_pixels);
  const float top_y_pixels =
    (margin_pixels + ALLOCATION_STORAGE_APP_SCENE_BAR_HEIGHT_PIXELS + overhang_pixels);

  renderer_2d_draw_line(
    inout_renderer,
    (union fvector3_t){{x_pixels, bottom_y_pixels, 0.0f}},
    (union fvector3_t){{x_pixels, top_y_pixels, 0.0f}},
    (union fvector4_t){{0.95f, 0.75f, 0.3f, 1.0f}});
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_scene_calculate_bar_transform()
 *
 ******************************************************************************/
static inline union fmatrix_4x4_t allocation_storage_app_scene_calculate_bar_transform(
  float in_width_pixels)
{
  const float margin_pixels = ALLOCATION_STORAGE_APP_SCENE_MARGIN_PIXELS;

  // The unit square spans 0 to 1 on each axis, so it's scaled to the bar's
  //  size, then moved to its place by its bottom-left corner
  const union fmatrix_4x4_t transform =
    fmatrix_4x4_transform_compose_2d(
      (union fvector2_t){{0.0f, 0.0f}},
      (union fvector2_t){{in_width_pixels, ALLOCATION_STORAGE_APP_SCENE_BAR_HEIGHT_PIXELS}},
      0.0f,
      (union fvector2_t){{margin_pixels, margin_pixels}});

  return transform;
}
