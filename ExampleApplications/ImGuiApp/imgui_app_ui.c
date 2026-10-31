#include "imgui_app_ui.h"
#include "imgui_app.h"

#include "fmath.h"
#include "ui_imgui.h"
#include "util.h"

#include <math.h>
#include <stdbool.h>

// How much of the wave the plot shows
#define IMGUI_APP_UI_PLOT_WINDOW_SECONDS 2.0f

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Makes a button counting its clicks, and a checkbox showing ImGui's demo
//  window
//
//  in/out: inout_settings
static void imgui_app_ui_draw_widgets(
  struct imgui_app_settings_t* inout_settings);

// Makes a color picker for the window's background, applying each change to
//  the window
//
//  in/out: inout_app
static void imgui_app_ui_draw_background_picker(
  struct imgui_app_t* inout_app);

// Makes sliders for the wave's frequency and amplitude, then plots the wave
//  with ImPlot
//
//  in/out: inout_app
static void imgui_app_ui_draw_wave_plot(
  struct imgui_app_t* inout_app);

// Fills the app's wave samples with IMGUI_APP_UI_PLOT_WINDOW_SECONDS of a
//  sine wave from in_start_time_seconds, at the settings' frequency and
//  amplitude
//
//  in:     in_start_time_seconds
//  in/out: inout_app
static inline void imgui_app_ui_sample_wave(
  struct imgui_app_t* inout_app,
  float in_start_time_seconds);

/*******************************************************************************
 *
 *   Function: imgui_app_ui_render()
 *
 ******************************************************************************/
void imgui_app_ui_render(
  struct ui_layer_t* inout_ui_layer)
{
  struct imgui_app_t* app =
    container_of(
      inout_ui_layer,
      struct imgui_app_t,
      ui_layer);

  // Placed the first time it's shown, and as tall as its contents
  if (ui_imgui_begin_panel(
        "Jotunn ImGui Example",
        (ImVec2_c){20.0f, 20.0f},
        460.0f))
  {
    igSeparatorText("Widgets");
    imgui_app_ui_draw_widgets(&(app->settings));

    igSeparatorText("Window");
    imgui_app_ui_draw_background_picker(app);

    igSeparatorText("ImPlot");
    imgui_app_ui_draw_wave_plot(app);
  }
  igEnd();

  if (app->settings.is_demo_window_shown) {
    igShowDemoWindow(&(app->settings.is_demo_window_shown));
  }
}

/*******************************************************************************
 *
 *   Function: imgui_app_ui_draw_widgets()
 *
 ******************************************************************************/
static void imgui_app_ui_draw_widgets(
  struct imgui_app_settings_t* inout_settings)
{
  if (igButton(
        "Click me",
        (ImVec2_c){0.0f, 0.0f}))
  {
    inout_settings->button_click_count++;
  }
  igSameLine(
    0.0f,
    -1.0f);
  igText(
    "Clicked %d times",
    inout_settings->button_click_count);

  igCheckbox(
    "Show the ImGui demo window",
    &(inout_settings->is_demo_window_shown));
}

/*******************************************************************************
 *
 *   Function: imgui_app_ui_draw_background_picker()
 *
 ******************************************************************************/
static void imgui_app_ui_draw_background_picker(
  struct imgui_app_t* inout_app)
{
  if (igColorEdit3(
        "Background",
        inout_app->settings.background_color.vec,
        0))
  {
    imgui_app_apply_background_color(inout_app);
  }
}

/*******************************************************************************
 *
 *   Function: imgui_app_ui_draw_wave_plot()
 *
 ******************************************************************************/
static void imgui_app_ui_draw_wave_plot(
  struct imgui_app_t* inout_app)
{
  const double sample_period_seconds =
    ((double)IMGUI_APP_UI_PLOT_WINDOW_SECONDS / (double)IMGUI_APP_PLOT_SAMPLE_COUNT);

  igSliderFloat(
    "Frequency (Hz)",
    &(inout_app->settings.wave_frequency_hz),
    0.1f,
    5.0f,
    "%.2f",
    0);
  igSliderFloat(
    "Amplitude",
    &(inout_app->settings.wave_amplitude),
    0.0f,
    2.0f,
    "%.2f",
    0);

  // Scrolling with ImGui's clock
  imgui_app_ui_sample_wave(
    inout_app,
    (float)igGetTime());

  if (ImPlot_BeginPlot(
        "Sine wave",
        (ImVec2_c){-1.0f, 220.0f},
        ImPlotFlags_NoMenus))
  {
    ImPlot_SetupAxes(
      "t (s)",
      "y",
      0,
      0);
    ImPlot_SetupAxisLimits(
      ImAxis_X1,
      0.0,
      (double)IMGUI_APP_UI_PLOT_WINDOW_SECONDS,
      ImPlotCond_Always);
    ImPlot_SetupAxisLimits(
      ImAxis_Y1,
      -2.5,
      2.5,
      ImPlotCond_Always);

    ImPlot_PlotLine_FloatPtrInt(
      "sin",
      inout_app->wave_samples,
      IMGUI_APP_PLOT_SAMPLE_COUNT,
      sample_period_seconds,
      0.0,
      ui_imgui_get_default_plot_spec());

    ImPlot_EndPlot();
  }
}

/*******************************************************************************
 *
 *   Function: imgui_app_ui_sample_wave()
 *
 ******************************************************************************/
static inline void imgui_app_ui_sample_wave(
  struct imgui_app_t* inout_app,
  float in_start_time_seconds)
{
  const float sample_period_seconds =
    (IMGUI_APP_UI_PLOT_WINDOW_SECONDS / (float)IMGUI_APP_PLOT_SAMPLE_COUNT);
  const float angular_frequency_radians_per_second =
    (2.0f * FMATH_PI * inout_app->settings.wave_frequency_hz);

  for (int i = 0; i < IMGUI_APP_PLOT_SAMPLE_COUNT; i++) {
    const float time_seconds = (in_start_time_seconds + ((float)i * sample_period_seconds));

    inout_app->wave_samples[i] =
      (inout_app->settings.wave_amplitude *
       sinf(angular_frequency_radians_per_second * time_seconds));
  }
}
