#pragma once

// The ImGui example's application: one window whose only layer is a Dear
//  ImGui interface, and the settings that interface edits. Setting it up is
//  here, in imgui_app.c; the interface itself is in imgui_app_ui.c

#include "application.h"
#include "fvector.h"
#include "ui_layer.h"

#include <stdbool.h>
#include <stdint.h>

#define IMGUI_APP_WIDTH_PIXELS 1000
#define IMGUI_APP_HEIGHT_PIXELS 700

// How many points of the wave are plotted
#define IMGUI_APP_PLOT_SAMPLE_COUNT 200

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// What the interface edits
struct imgui_app_settings_t
{
  // Red, green and blue, edited through vec
  union fvector3_t background_color;
  float wave_frequency_hz;
  float wave_amplitude;
  int button_click_count;
  bool is_demo_window_shown;
};

// Everything the application owns. The UI layer's render callback gets back
//  to it from the layer with container_of()
struct imgui_app_t
{
  struct application_t application;
  struct window_t window;
  struct ui_layer_t ui_layer;
  //
  struct imgui_app_settings_t settings;
  // The wave as last plotted
  float wave_samples[IMGUI_APP_PLOT_SAMPLE_COUNT];
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Sets up the application and its window, with a UI layer as the window's
//  only layer, drawing imgui_app_ui_render()'s interface
//
//  out:    out_app
//  ret:    0 on success; non-zero when the application, its window or its UI
//            layer can't be set up
uint8_t imgui_app_init(
  struct imgui_app_t* out_app);

// Releases what imgui_app_init() set up
//
//  in/out: inout_app
void imgui_app_cleanup(
  struct imgui_app_t* inout_app);

/*******************************************************************************
 *
 *   WINDOW
 *
 ******************************************************************************/

// Sets the window's background, which shows wherever the UI layer draws
//  nothing, to the color in the app's settings
//
//  in/out: inout_app
void imgui_app_apply_background_color(
  struct imgui_app_t* inout_app);
