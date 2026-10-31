#pragma once

// The 3D renderer example's application: one window with one layer, drawn by
//  a renderer_3d through a perspective camera that Jotunn's orbit controller
//  moves around the scene. Setting it up, and feeding the mouse's events to
//  the orbit, is here, in renderer_3d_app.c; what the layer draws is in
//  renderer_3d_app_scene.c

#include "application.h"
#include "camera_orbit.h"
#include "texture.h"
#include "window_layer_3d.h"

#include <stdint.h>

#define RENDERER_3D_APP_WIDTH_PIXELS 1000
#define RENDERER_3D_APP_HEIGHT_PIXELS 700

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// Everything the application owns. The layer's run and the window's event
//  handler get back to it with container_of()
struct renderer_3d_app_t
{
  struct application_t application;
  struct window_t window;
  //
  // The scene's layer, with the framebuffer, camera and renderer it draws
  //  with
  struct window_layer_3d_t scene_layer;
  //
  // Jotunn's placeholder texture when its file can't be read
  struct texture_2d_t cube_texture;
  //
  // Where the scene's camera sits, moved by the mouse
  struct camera_orbit_t orbit;
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Sets up the application and its window, with one layer drawing
//  renderer_3d_app_scene_run()'s scene, and loads the cube's texture
//
//  out:    out_app
//  ret:    0 on success; non-zero when the application, its window or its
//            renderer can't be set up
uint8_t renderer_3d_app_init(
  struct renderer_3d_app_t* out_app);

// Releases what renderer_3d_app_init() set up
//
//  in/out: inout_app
void renderer_3d_app_cleanup(
  struct renderer_3d_app_t* inout_app);
