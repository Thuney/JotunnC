#pragma once

// The allocation storage example's interface: how full the pool is, a table
//  of every allocator tag's policy and memory, and buttons allocating through
//  Jotunn's allocator. Each frame the UI layer opens an ImGui frame, calls
//  allocation_storage_app_ui_render() for its ig* calls, then draws the
//  result

#include "ui_layer.h"

/*******************************************************************************
 *
 *   RENDERING
 *
 ******************************************************************************/

// Makes the frame's ImGui calls. Set as the UI layer's custom render by
//  allocation_storage_app_init(), so inout_ui_layer is the ui_layer of an
//  allocation_storage_app_t
//
//  in/out: inout_ui_layer
void allocation_storage_app_ui_render(
  struct ui_layer_t* inout_ui_layer);
