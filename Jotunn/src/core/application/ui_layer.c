#include "ui_layer.h"

#include "ui/ui_layer_imgui.h"
#include "util.h"

#include <memory.h>

// union fvector4_t ui_color_from_hex(uint32_t hex_code)
// {
//     uint8_t r_hex, g_hex, b_hex, a_hex;

//     r_hex = (hex_code & 0xFF000000) >> 24;
//     g_hex = (hex_code & 0x00FF0000) >> 16;
//     b_hex = (hex_code & 0x0000FF00) >>  8;
//     a_hex = (hex_code & 0x000000FF);

//     union fvector4_t color =
//         (union fvector4_t)
//         {
//             (r_hex / 255.0f),
//             (g_hex / 255.0f),
//             (b_hex / 255.0f),
//             (a_hex / 255.0f)
//         };

//     return color;
// }

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Runs the layer each frame: an ImGui frame around the custom render function,
//  drawn through the layer's renderer_2d
//
//  in/out: inout_window_layer
static void ui_layer_render(
  struct window_layer_t* inout_window_layer);

// Hands events reaching the layer to its UI implementation
//
//  in/out: inout_parent_window, inout_window_layer, inout_event
static void _ui_layer_event_react(
  struct window_t* inout_parent_window,
  struct window_layer_t* inout_window_layer,
  struct event_base_t* inout_event);

/*******************************************************************************
 *
 *   Function: ui_layer_init()
 *
 ******************************************************************************/
uint8_t ui_layer_init(
  struct window_t* in_parent_window,
  struct ui_layer_t* out_ui_layer,
  struct renderer_2d_storage_t* inout_renderer_storage,
  uint32_t in_width_pixels,
  uint32_t in_height_pixels)
{
  memset(
    out_ui_layer,
    0,
    sizeof(struct ui_layer_t));

  framebuffer_init(
    &(out_ui_layer->ui_framebuffer),
    in_width_pixels,
    in_height_pixels);

  window_layer_init(
    in_parent_window,
    &(out_ui_layer->ui_window_layer),
    &(out_ui_layer->ui_framebuffer),
    &(out_ui_layer->ui_camera_ortho.base),
    &(out_ui_layer->ui_renderer_2d.base));

  camera_orthographic_init_pixel_space(
    &(out_ui_layer->ui_camera_ortho),
    in_width_pixels,
    in_height_pixels);

  const uint8_t error =
    renderer_2d_init(
      &(out_ui_layer->ui_renderer_2d),
      inout_renderer_storage,
      &(out_ui_layer->ui_camera_ortho.base),
      in_parent_window,
      "ui_layer_renderer_2d");

  if (error) {
    framebuffer_cleanup(&(out_ui_layer->ui_framebuffer));

    return error;
  }

  // The UI is drawn strictly in submission order, so depth testing is off
  render_api_set_render_flags(
    &(out_ui_layer->ui_renderer_2d.base.render_api),
    (RENDER_API_RENDER_FLAGS_COLOR | RENDER_API_RENDER_FLAGS_BLEND));

  ui_layer_internal_imgui_init(out_ui_layer);

  window_layer_set_custom_layer_run(
    &(out_ui_layer->ui_window_layer),
    ui_layer_render);
  window_layer_set_event_react(
    &(out_ui_layer->ui_window_layer),
    &(_ui_layer_event_react));

  return 0;
}

/*******************************************************************************
 *
 *   Function: ui_layer_cleanup()
 *
 ******************************************************************************/
void ui_layer_cleanup(
  struct ui_layer_t* inout_ui_layer)
{
  struct window_t* parent_window = inout_ui_layer->ui_window_layer.parent_window;

  // Releasing the UI's textures needs the window's graphics context
  window_set_context(parent_window);

  ui_layer_internal_imgui_cleanup(inout_ui_layer);

  renderer_2d_cleanup(&(inout_ui_layer->ui_renderer_2d));

  framebuffer_cleanup(&(inout_ui_layer->ui_framebuffer));

  window_release_context(parent_window);
}

/*******************************************************************************
 *
 *   Function: ui_layer_set_custom_render()
 *
 ******************************************************************************/
void ui_layer_set_custom_render(
  struct ui_layer_t* inout_ui_layer,
  void (*in_function_custom_ui_layer_render)(struct ui_layer_t* inout_ui_layer))
{
  inout_ui_layer->function_custom_ui_layer_render = in_function_custom_ui_layer_render;
}

/*******************************************************************************
 *
 *   Function: ui_layer_set_custom_event_react()
 *
 ******************************************************************************/
void ui_layer_set_custom_event_react(
  struct ui_layer_t* inout_ui_layer,
  void (*in_function_custom_ui_layer_event_react)(
    struct ui_layer_t* inout_ui_layer,
    struct event_base_t* inout_event))
{
  inout_ui_layer->function_custom_ui_layer_event_react = in_function_custom_ui_layer_event_react;
}

/*******************************************************************************
 *
 *   Function: ui_layer_render()
 *
 ******************************************************************************/
static void ui_layer_render(
  struct window_layer_t* inout_window_layer)
{
  struct ui_layer_t* ui_layer =
    container_of(
      inout_window_layer,
      struct ui_layer_t,
      ui_window_layer);

  ui_layer_internal_imgui_frame_begin(ui_layer);

  if (ui_layer->function_custom_ui_layer_render) {
    ui_layer->function_custom_ui_layer_render(ui_layer);
  }

  ui_layer_internal_imgui_frame_end(ui_layer);
}

/*******************************************************************************
 *
 *   Function: _ui_layer_event_react()
 *
 ******************************************************************************/
static void _ui_layer_event_react(
  struct window_t* inout_parent_window,
  struct window_layer_t* inout_window_layer,
  struct event_base_t* inout_event)
{
  (void)inout_parent_window;

  struct ui_layer_t* ui_layer =
    container_of(
      inout_window_layer,
      struct ui_layer_t,
      ui_window_layer);

  // The UI sees every event, whatever the custom handler then does with it
  const bool is_captured =
    ui_layer_internal_imgui_event_react(
      ui_layer,
      inout_event);

  // The custom handler is part of the UI, so it sees every event too
  if (ui_layer->function_custom_ui_layer_event_react) {
    ui_layer->function_custom_ui_layer_event_react(
      ui_layer,
      inout_event);
  }

  // Input the UI wants exclusive use of is kept from the layers beneath
  if (is_captured) {
    inout_event->is_handled = 1U;
  }
}
