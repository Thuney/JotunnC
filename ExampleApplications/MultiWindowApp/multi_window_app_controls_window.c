#include "multi_window_app_controls_window.h"

#include "color.h"
#include "ui_imgui.h"
#include "util.h"

#include <stdbool.h>

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Sets up the swatch layer and the UI layer and adds them to the window,
//  bottom first. The window's graphics context must be current
//
//  in/out: inout_controls_window
//  ret:    0 on success; non-zero when a layer can't be set up or added
static uint8_t multi_window_app_controls_window_create_layers(
  struct multi_window_app_controls_window_t* inout_controls_window);

// Draws a strip in the circles' color along the bottom of the window. The
//  swatch layer's custom run
//
//  in/out: inout_window_layer
static void multi_window_app_controls_window_run_swatch(
  struct window_layer_t* inout_window_layer);

// Makes the window's ImGui calls. The UI layer's custom render
//
//  in/out: inout_ui_layer
static void multi_window_app_controls_window_render_ui(
  struct ui_layer_t* inout_ui_layer);

// Makes the controls for the scene window's circles: how many, how fast they
//  turn, and their color
//
//  in/out: inout_state
static void multi_window_app_controls_window_draw_circle_controls(
  struct multi_window_app_state_t* inout_state);

// Shows what the scene window's overlay has recorded, the mouse's position
//  and the markers, with a button clearing the markers
//
//  in/out: inout_state
static void multi_window_app_controls_window_draw_overlay_readout(
  struct multi_window_app_state_t* inout_state);

/*******************************************************************************
 *
 *   Function: multi_window_app_controls_window_init()
 *
 ******************************************************************************/
uint8_t multi_window_app_controls_window_init(
  struct multi_window_app_controls_window_t* out_controls_window,
  struct application_t* in_application,
  struct multi_window_app_state_t* inout_state)
{
  const uint8_t max_window_layers = 2;

  out_controls_window->state = inout_state;

  // The engine allocates the list of layers, being given NULL storage.
  //  window_init() leaves the window's graphics context current, for its
  //  layers to be made in
  uint8_t error =
    window_init(
      &(out_controls_window->window),
      NULL,
      max_window_layers,
      MULTI_WINDOW_APP_CONTROLS_WINDOW_WIDTH_PIXELS,
      MULTI_WINDOW_APP_CONTROLS_WINDOW_HEIGHT_PIXELS,
      "Jotunn - Multi-Window Example: Controls",
      in_application);

  if (!error) {
    error = multi_window_app_controls_window_create_layers(out_controls_window);
  }

  if (!error) {
    window_set_background_color(
      &(out_controls_window->window),
      (union fvector4_t){{0.18f, 0.18f, 0.2f, 1.0f}});
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: multi_window_app_controls_window_cleanup()
 *
 ******************************************************************************/
void multi_window_app_controls_window_cleanup(
  struct multi_window_app_controls_window_t* inout_controls_window)
{
  window_set_context(&(inout_controls_window->window));

  window_layer_2d_cleanup(&(inout_controls_window->swatch_layer));

  // Makes its window's graphics context current itself, and releases it
  //  after
  ui_layer_cleanup(&(inout_controls_window->ui_layer));
}

/*******************************************************************************
 *
 *   Function: multi_window_app_controls_window_create_layers()
 *
 ******************************************************************************/
static uint8_t multi_window_app_controls_window_create_layers(
  struct multi_window_app_controls_window_t* inout_controls_window)
{
  struct window_t* window = &(inout_controls_window->window);

  // Each covers the window, and has its renderer's batch storage allocated
  //  by the engine, being given NULL
  uint8_t error =
    window_layer_2d_init(
      window,
      &(inout_controls_window->swatch_layer),
      NULL,
      "multi_window_app_swatch_renderer",
      &(multi_window_app_controls_window_run_swatch));

  if (!error) {
    error =
      ui_layer_init(
        window,
        &(inout_controls_window->ui_layer),
        NULL);
  }

  if (error) {
    return error;
  }

  ui_layer_set_custom_render(
    &(inout_controls_window->ui_layer),
    &(multi_window_app_controls_window_render_ui));

  // Drawn in the order they're added, so the UI goes over the swatch
  error |=
    window_add_layer(
      window,
      &(inout_controls_window->swatch_layer.window_layer));
  error |=
    window_add_layer(
      window,
      &(inout_controls_window->ui_layer.layer_2d.window_layer));

  return error;
}

/*******************************************************************************
 *
 *   Function: multi_window_app_controls_window_run_swatch()
 *
 ******************************************************************************/
static void multi_window_app_controls_window_run_swatch(
  struct window_layer_t* inout_window_layer)
{
  struct multi_window_app_controls_window_t* controls_window =
    container_of(
      inout_window_layer,
      struct multi_window_app_controls_window_t,
      swatch_layer.window_layer);

  const float strip_height_pixels = 40.0f;
  const float width_pixels = (float)controls_window->window.metadata.width;

  // The unit square, stretched across the bottom of the window from its
  //  bottom-left corner
  const union fmatrix_4x4_t transform =
    fmatrix_4x4_transform_compose_2d(
      (union fvector2_t){{0.0f, 0.0f}},
      (union fvector2_t){{width_pixels, strip_height_pixels}},
      0.0f,
      (union fvector2_t){{0.0f, 0.0f}});

  renderer_2d_draw_quad(
    &(controls_window->swatch_layer.renderer_2d),
    &(transform),
    color_from_rgb(
      controls_window->state->circle_color,
      1.0f));
}

/*******************************************************************************
 *
 *   Function: multi_window_app_controls_window_render_ui()
 *
 ******************************************************************************/
static void multi_window_app_controls_window_render_ui(
  struct ui_layer_t* inout_ui_layer)
{
  struct multi_window_app_controls_window_t* controls_window =
    container_of(
      inout_ui_layer,
      struct multi_window_app_controls_window_t,
      ui_layer);

  // Placed the first time it's shown, and as tall as its contents
  if (ui_imgui_begin_panel(
        "Scene controls",
        (ImVec2_c){10.0f, 10.0f},
        400.0f))
  {
    igSeparatorText("Circles");
    multi_window_app_controls_window_draw_circle_controls(controls_window->state);

    igSeparatorText("Overlay");
    multi_window_app_controls_window_draw_overlay_readout(controls_window->state);

    igTextWrapped("The strip below is a 2D layer drawn under this UI layer.");
  }
  igEnd();
}

/*******************************************************************************
 *
 *   Function: multi_window_app_controls_window_draw_circle_controls()
 *
 ******************************************************************************/
static void multi_window_app_controls_window_draw_circle_controls(
  struct multi_window_app_state_t* inout_state)
{
  igSliderInt(
    "Count",
    &(inout_state->circle_count),
    1,
    MULTI_WINDOW_APP_STATE_MAX_CIRCLES,
    "%d",
    0);
  igSliderFloat(
    "Speed (rad/s)",
    &(inout_state->orbit_speed_radians_per_second),
    -3.0f,
    3.0f,
    "%.2f",
    0);
  igColorEdit3(
    "Color",
    inout_state->circle_color.vec,
    0);
}

/*******************************************************************************
 *
 *   Function: multi_window_app_controls_window_draw_overlay_readout()
 *
 ******************************************************************************/
static void multi_window_app_controls_window_draw_overlay_readout(
  struct multi_window_app_state_t* inout_state)
{
  if (inout_state->is_mouse_over_scene) {
    igText(
      "Mouse at (%.0f, %.0f)",
      inout_state->mouse_position_pixels.comp.x,
      inout_state->mouse_position_pixels.comp.y);
  }
  else {
    igTextDisabled("Mouse outside the scene window");
  }

  igText(
    "%u markers",
    inout_state->marker_count);
  igSameLine(
    0.0f,
    -1.0f);

  if (igButton(
        "Clear",
        (ImVec2_c){0.0f, 0.0f}))
  {
    multi_window_app_state_clear_markers(inout_state);
  }
}
