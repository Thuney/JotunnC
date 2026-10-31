#pragma once

// What the 3D renderer example's layer draws: a grid on the ground with the
//  axes through it, a textured cube tumbling inside a wireframe cage, and a
//  ring of cubes, pyramids and spheres. Each frame the window begins the
//  layer's renderer_3d scene, calls renderer_3d_app_scene_run() to batch the
//  draws, then ends the scene, drawing the batches

#include "window_layer.h"

/*******************************************************************************
 *
 *   DRAWING
 *
 ******************************************************************************/

// Points the camera from the orbit, then draws the scene. Set as the custom
//  run of the app's scene layer by renderer_3d_app_init(), so
//  inout_window_layer is the scene_layer of a renderer_3d_app_t
//
//  in/out: inout_window_layer
void renderer_3d_app_scene_run(
  struct window_layer_t* inout_window_layer);
