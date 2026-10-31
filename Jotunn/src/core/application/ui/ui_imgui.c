#include "ui_imgui.h"

/*******************************************************************************
 *
 *   Function: ui_imgui_begin_panel()
 *
 ******************************************************************************/
bool ui_imgui_begin_panel(
  const char* in_title,
  ImVec2_c in_position_pixels,
  float in_width_pixels)
{
  // A height of 0 fits the window to its contents
  igSetNextWindowPos(
    in_position_pixels,
    ImGuiCond_FirstUseEver,
    (ImVec2_c){0.0f, 0.0f});
  igSetNextWindowSize(
    (ImVec2_c){in_width_pixels, 0.0f},
    ImGuiCond_FirstUseEver);

  const bool is_open =
    igBegin(
      in_title,
      NULL,
      0);

  return is_open;
}

/*******************************************************************************
 *
 *   Function: ui_imgui_get_default_plot_spec()
 *
 ******************************************************************************/
ImPlotSpec_c ui_imgui_get_default_plot_spec(void)
{
  static ImPlotSpec_c default_spec;
  static bool is_default_spec_read = false;

  // ImPlot's defaults are set by its C++ constructor, so one is constructed
  //  once to copy them from
  if (!is_default_spec_read) {
    ImPlotSpec_c* constructed_spec = ImPlotSpec_ImPlotSpec();
    default_spec = *constructed_spec;
    ImPlotSpec_destroy(constructed_spec);

    is_default_spec_read = true;
  }

  return default_spec;
}
