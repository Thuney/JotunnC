#include "window_layer_3d.h"

#include "window.h"

/*******************************************************************************
 *
 *   Function: window_layer_3d_init()
 *
 ******************************************************************************/
uint8_t window_layer_3d_init(
  struct window_t* in_parent_window,
  struct window_layer_3d_t* out_layer,
  struct renderer_3d_storage_t* inout_renderer_storage,
  const char* in_renderer_tag,
  float in_field_of_view_radians,
  float in_near_plane,
  float in_far_plane,
  void (*in_function_run)(
    struct window_layer_t* inout_window_layer))
{
  const uint32_t width_pixels = in_parent_window->metadata.width;
  const uint32_t height_pixels = in_parent_window->metadata.height;

  camera_perspective_init(
    &(out_layer->camera_perspective),
    (union fvector3_t){{0.0f, 0.0f, 0.0f}},
    (union fvector3_t){{0.0f, 1.0f, 0.0f}},
    (union fvector3_t){{0.0f, 0.0f, -1.0f}});

  // The window refits the aspect ratio whenever it's resized
  camera_perspective_set_projection(
    &(out_layer->camera_perspective),
    in_field_of_view_radians,
    ((float)width_pixels / (float)height_pixels),
    in_near_plane,
    in_far_plane);

  // Made first, as the only step that can fail
  const uint8_t error =
    renderer_3d_init(
      &(out_layer->renderer_3d),
      inout_renderer_storage,
      &(out_layer->camera_perspective.base),
      in_parent_window,
      in_renderer_tag);

  if (!error) {
    framebuffer_init(
      &(out_layer->framebuffer),
      width_pixels,
      height_pixels);

    window_layer_init(
      in_parent_window,
      &(out_layer->window_layer),
      &(out_layer->framebuffer),
      &(out_layer->camera_perspective.base),
      &(out_layer->renderer_3d.base));
    window_layer_set_custom_layer_run(
      &(out_layer->window_layer),
      in_function_run);
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: window_layer_3d_cleanup()
 *
 ******************************************************************************/
void window_layer_3d_cleanup(
  struct window_layer_3d_t* inout_layer)
{
  renderer_3d_cleanup(&(inout_layer->renderer_3d));
  framebuffer_cleanup(&(inout_layer->framebuffer));
}
