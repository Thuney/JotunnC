#include "window_layer_2d.h"

#include "window.h"

/*******************************************************************************
 *
 *   Function: window_layer_2d_init()
 *
 ******************************************************************************/
uint8_t window_layer_2d_init(
  struct window_t* in_parent_window,
  struct window_layer_2d_t* out_layer,
  struct renderer_2d_storage_t* inout_renderer_storage,
  const char* in_renderer_tag,
  void (*in_function_run)(
    struct window_layer_t* inout_window_layer))
{
  const uint32_t width_pixels = in_parent_window->metadata.width;
  const uint32_t height_pixels = in_parent_window->metadata.height;

  camera_orthographic_init_pixel_space(
    &(out_layer->camera_ortho),
    width_pixels,
    height_pixels);

  // Made first, as the only step that can fail
  const uint8_t error =
    renderer_2d_init(
      &(out_layer->renderer_2d),
      inout_renderer_storage,
      &(out_layer->camera_ortho.base),
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
      &(out_layer->camera_ortho.base),
      &(out_layer->renderer_2d.base));
    window_layer_set_custom_layer_run(
      &(out_layer->window_layer),
      in_function_run);
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: window_layer_2d_cleanup()
 *
 ******************************************************************************/
void window_layer_2d_cleanup(
  struct window_layer_2d_t* inout_layer)
{
  renderer_2d_cleanup(&(inout_layer->renderer_2d));
  framebuffer_cleanup(&(inout_layer->framebuffer));
}
