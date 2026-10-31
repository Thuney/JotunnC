#include "imgui_app.h"
#include "imgui_app_ui.h"

#include "color.h"

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Gives the settings their starting values
//
//  out:    out_settings
static void imgui_app_init_settings(
  struct imgui_app_settings_t* out_settings);

// Opens the window. Its list of layers is allocated by the engine, being
//  given NULL storage; see AllocationStorageApp for providing it
//
//  in/out: inout_app
//  ret:    0 on success; non-zero when the window can't be opened
static uint8_t imgui_app_create_window(
  struct imgui_app_t* inout_app);

// Sets up the UI layer covering the window, has it call imgui_app_ui_render()
//  each frame, and adds it to the window. Its renderer's batch storage is
//  allocated by the engine. The window's graphics context must be current
//
//  in/out: inout_app
//  ret:    0 on success; non-zero when the layer can't be set up or added
static uint8_t imgui_app_create_ui_layer(
  struct imgui_app_t* inout_app);

/*******************************************************************************
 *
 *   Function: imgui_app_init()
 *
 ******************************************************************************/
uint8_t imgui_app_init(
  struct imgui_app_t* out_app)
{
  const uint8_t max_windows = 1;

  imgui_app_init_settings(&(out_app->settings));

  // The engine allocates the list of windows, being given NULL storage
  uint8_t error =
    application_init(
      &(out_app->application),
      "ImGuiApp",
      NULL,
      max_windows);

  if (!error) {
    error = imgui_app_create_window(out_app);
  }

  // window_init() left the window's graphics context current, which the UI
  //  layer is made in
  if (!error) {
    error = imgui_app_create_ui_layer(out_app);
  }

  if (!error) {
    imgui_app_apply_background_color(out_app);

    error =
      application_add_window(
        &(out_app->application),
        &(out_app->window));
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: imgui_app_cleanup()
 *
 ******************************************************************************/
void imgui_app_cleanup(
  struct imgui_app_t* inout_app)
{
  // The UI layer makes its window's graphics context current to clean up
  ui_layer_cleanup(&(inout_app->ui_layer));

  // Cleans up the window too, but leaves its memory, which is ours
  application_cleanup(&(inout_app->application));
}

/*******************************************************************************
 *
 *   Function: imgui_app_apply_background_color()
 *
 ******************************************************************************/
void imgui_app_apply_background_color(
  struct imgui_app_t* inout_app)
{
  window_set_background_color(
    &(inout_app->window),
    color_from_rgb(
      inout_app->settings.background_color,
      1.0f));
}

/*******************************************************************************
 *
 *   Function: imgui_app_init_settings()
 *
 ******************************************************************************/
static void imgui_app_init_settings(
  struct imgui_app_settings_t* out_settings)
{
  out_settings->background_color = (union fvector3_t){{0.15f, 0.25f, 0.35f}};
  out_settings->wave_frequency_hz = 1.0f;
  out_settings->wave_amplitude = 1.0f;
  out_settings->button_click_count = 0;
  out_settings->is_demo_window_shown = false;
}

/*******************************************************************************
 *
 *   Function: imgui_app_create_window()
 *
 ******************************************************************************/
static uint8_t imgui_app_create_window(
  struct imgui_app_t* inout_app)
{
  const uint8_t max_window_layers = 1;

  const uint8_t error =
    window_init(
      &(inout_app->window),
      NULL,
      max_window_layers,
      IMGUI_APP_WIDTH_PIXELS,
      IMGUI_APP_HEIGHT_PIXELS,
      "Jotunn - ImGui Example",
      &(inout_app->application));

  return error;
}

/*******************************************************************************
 *
 *   Function: imgui_app_create_ui_layer()
 *
 ******************************************************************************/
static uint8_t imgui_app_create_ui_layer(
  struct imgui_app_t* inout_app)
{
  uint8_t error =
    ui_layer_init(
      &(inout_app->window),
      &(inout_app->ui_layer),
      NULL);

  if (!error) {
    ui_layer_set_custom_render(
      &(inout_app->ui_layer),
      &(imgui_app_ui_render));

    // A UI layer is drawn into the window through the 2D layer it embeds
    error =
      window_add_layer(
        &(inout_app->window),
        &(inout_app->ui_layer.layer_2d.window_layer));
  }

  return error;
}
