#pragma once

// Helpers for the ImGui and ImPlot calls a UI layer's custom render makes,
//  covering what cimgui and cimplot leave awkward from C. Each must be called
//  while a UI layer's contexts are current, as they are during its render

#include "cimgui.h"
#include "cimplot.h"

#include <stdbool.h>

/*******************************************************************************
 *
 *   PANELS
 *
 ******************************************************************************/

// Begins an ImGui window, placing it at in_position_pixels from the top-left
//  of the UI, in_width_pixels wide and as tall as its contents, the first
//  time it's ever shown; after that it stays wherever it's moved and however
//  it's sized. It must be ended with igEnd() whatever this returns
//
//  in:     in_title, in_position_pixels, in_width_pixels
//  ret:    true when the window is open, and its contents should be made
bool ui_imgui_begin_panel(
  const char* in_title,
  ImVec2_c in_position_pixels,
  float in_width_pixels);

/*******************************************************************************
 *
 *   PLOTS
 *
 ******************************************************************************/

// Reads ImPlot's default plot item specification, as the ImPlot_Plot*
//  functions take it. cimplot leaves the defaults to ImPlotSpec's C++
//  constructor, so they're copied from one made once, on the first call
//
//  ret:    A copy of the defaults
ImPlotSpec_c ui_imgui_get_default_plot_spec(void);
