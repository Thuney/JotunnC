#pragma once

// What the 2D renderer example's layer draws: a title, a row with each of
//  renderer_2d's primitives and a pair spinning with the application's
//  clock, and a row of textures. Each frame the window begins the layer's
//  renderer_2d scene, calls renderer_2d_app_scene_run() to batch the draws,
//  then ends the scene, drawing the batches

#include "window_layer.h"

/*******************************************************************************
 *
 *   DRAWING
 *
 ******************************************************************************/

// Draws the scene. Set as the custom run of the app's scene layer by
//  renderer_2d_app_init(), so inout_window_layer is the scene_layer of a
//  renderer_2d_app_t
//
//  in/out: inout_window_layer
void renderer_2d_app_scene_run(
  struct window_layer_t* inout_window_layer);
