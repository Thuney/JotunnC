#pragma once

// The 2D renderer example's application: one window with one layer, drawn by
//  a renderer_2d through an orthographic camera that maps one unit to one
//  pixel, with the origin at the window's bottom-left corner. Setting it up
//  is here, in renderer_2d_app.c; what the layer draws is in
//  renderer_2d_app_scene.c, and the textures and typeface it draws with are
//  loaded in renderer_2d_app_resources.c

#include "application.h"
#include "renderer_2d_app_resources.h"
#include "window_layer_2d.h"

#include <stdint.h>

#define RENDERER_2D_APP_WIDTH_PIXELS 1100
#define RENDERER_2D_APP_HEIGHT_PIXELS 700

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// Everything the application owns. The layer's run gets back to it with
//  container_of()
struct renderer_2d_app_t
{
  struct application_t application;
  struct window_t window;
  //
  // The scene's layer, with the framebuffer, camera and renderer it draws
  //  with
  struct window_layer_2d_t scene_layer;
  //
  struct renderer_2d_app_resources_t resources;
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Sets up the application and its window, with one layer drawing
//  renderer_2d_app_scene_run()'s scene, and loads the scene's resources
//
//  out:    out_app
//  ret:    0 on success; non-zero when the application, its window or its
//            renderer can't be set up
uint8_t renderer_2d_app_init(
  struct renderer_2d_app_t* out_app);

// Releases what renderer_2d_app_init() set up
//
//  in/out: inout_app
void renderer_2d_app_cleanup(
  struct renderer_2d_app_t* inout_app);
