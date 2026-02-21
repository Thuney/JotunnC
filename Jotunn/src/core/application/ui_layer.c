#include "ui_layer.h"

// fvector4 ui_color_from_hex(uint32_t hex_code)
// {
//     uint8_t r_hex, g_hex, b_hex, a_hex;

//     r_hex = (hex_code & 0xFF000000) >> 24;
//     g_hex = (hex_code & 0x00FF0000) >> 16;
//     b_hex = (hex_code & 0x0000FF00) >>  8;
//     a_hex = (hex_code & 0x000000FF);

//     fvector4 color =
//         (fvector4)
//         {
//             (r_hex / 255.0f),
//             (g_hex / 255.0f),
//             (b_hex / 255.0f),
//             (a_hex / 255.0f)
//         };

//     return color;
// }

extern void ui_layer_graphics_init(
  struct ui_layer_t* ui_layer);

extern void ui_layer_graphics_cleanup(
  struct ui_layer_t* ui_layer);

extern void ui_layer_graphics_render_begin(
  struct ui_layer_t* ui_layer);

extern void ui_layer_graphics_render_end(
  struct ui_layer_t* ui_layer);

//

static void ui_layer_window_layer_render(
  struct window_layer_t* window_layer);

//
void ui_layer_init(
  struct window_t* parent_window,
  struct ui_layer_t* ui_layer,
  uint32_t width,
  uint32_t height)
{
  memset(ui_layer, 0, sizeof(struct ui_layer_t));

  framebuffer_init(
    &(ui_layer->ui_framebuffer),
    width,
    height);

  window_layer_init(
    parent_window,
    &(ui_layer->ui_window_layer),
    &(ui_layer->ui_framebuffer),
    &(ui_layer->ui_camera_ortho.base),
    &(ui_layer->ui_renderer_2d.base));

  // Camera stuff
  const fvector3 camera_position = (fvector3) { {0.0f, 0.0f,  2.0f} };
  const fvector3 camera_up       = (fvector3) { {0.0f, 1.0f,  0.0f} };
  const fvector3 camera_front    = (fvector3) { {0.0f, 0.0f, -1.0f} };

  const float ortho_left   = 0.0f;
  const float ortho_right  = (float)width;
  const float ortho_top    = (float)height;
  const float ortho_bottom = 0.0f;
  const float ortho_near_plane = -3.0f;
  const float ortho_far_plane  = 100.0f;

  camera_init_orthographic(
    &(ui_layer->ui_camera_ortho),
    camera_position,
    camera_up,
    camera_front);
  camera_set_projection_orthographic(
    &(ui_layer->ui_camera_ortho),
    ortho_left,
    ortho_right,
    ortho_top,
    ortho_bottom,
    ortho_near_plane,
    ortho_far_plane);

  renderer_2d_init(
    &(ui_layer->ui_renderer_2d),
    &(ui_layer->ui_camera_ortho.base),
    parent_window,
    "ui_layer_renderer_2d");

  ui_layer_graphics_init(
    ui_layer);

  window_layer_set_custom_layer_run(&(ui_layer->ui_window_layer), ui_layer_window_layer_render);
  // window_layer_set_event_react(&(ui_layer->ui_window_layer), ui_layer_event_handle);
}

void ui_layer_cleanup(
  struct ui_layer_t* ui_layer)
{
  framebuffer_cleanup(&(ui_layer->ui_framebuffer));

  ui_layer_graphics_cleanup(ui_layer);
}

void ui_layer_set_custom_render(
  struct ui_layer_t* ui_layer,
  void (*function_custom_ui_layer_render)(struct ui_layer_t* ui_layer))
{
  ui_layer->function_custom_ui_layer_render =
    function_custom_ui_layer_render;
}

static void ui_layer_window_layer_render(
  struct window_layer_t* window_layer)
{
  struct ui_layer_t *ui_layer =
    (struct ui_layer_t*)window_layer;

  ui_layer_graphics_render_begin(ui_layer);

  if (ui_layer->function_custom_ui_layer_render)
  {
    ui_layer->function_custom_ui_layer_render(ui_layer);
  }

  ui_layer_graphics_render_end(ui_layer);
}
