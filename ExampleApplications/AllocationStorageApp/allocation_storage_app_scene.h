#pragma once

// What the allocation storage example's 2D layer draws: a bar along the
//  bottom of the window showing how full the pool is, with a tick at the most
//  it's held at once. Each frame the window begins the layer's renderer_2d
//  scene, calls allocation_storage_app_scene_run() to batch the draws, then
//  ends the scene, drawing the batches

#include "window_layer.h"

/*******************************************************************************
 *
 *   DRAWING
 *
 ******************************************************************************/

// Draws the pool's bar. Set as the custom run of the app's scene layer by
//  allocation_storage_app_init(), so inout_window_layer is the scene_layer
//  of an allocation_storage_app_t
//
//  in/out: inout_window_layer
void allocation_storage_app_scene_run(
  struct window_layer_t* inout_window_layer);
