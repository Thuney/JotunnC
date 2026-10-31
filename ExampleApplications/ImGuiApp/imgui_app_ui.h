#pragma once

// The ImGui example's interface: a panel of widgets, a color picker driving
//  the window's background and an ImPlot plot, plus ImGui's own demo window
//  on request. Each frame the UI layer opens an ImGui frame, calls
//  imgui_app_ui_render() for its ig* and ImPlot_* calls, then draws the result

#include "ui_layer.h"

/*******************************************************************************
 *
 *   RENDERING
 *
 ******************************************************************************/

// Makes the frame's ImGui and ImPlot calls. Set as the UI layer's custom
//  render by imgui_app_init(), so inout_ui_layer is the ui_layer of an
//  imgui_app_t
//
//  in/out: inout_ui_layer
void imgui_app_ui_render(
  struct ui_layer_t* inout_ui_layer);
