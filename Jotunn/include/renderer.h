#pragma once

// The part every renderer shares: the window it draws for, a tag naming it,
//  the camera it views through and the render state it draws with. A
//  renderer embeds renderer_base_t as its first member and fills in the scene
//  callbacks, which renderer_base_begin_scene() and renderer_base_end_scene()
//  call with the renderer itself

struct window_t;

#include "camera.h"
#include "render_api.h"

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

struct renderer_base_t
{
  struct window_t* parent_window;
  //
  char* tag;
  //
  struct camera_base_t* camera;
  //
  struct render_api_implementation_t render_api;
  // Set by the renderer embedding this base, and called with that renderer
  void (*renderer_begin_scene)(void* inout_renderer);
  void (*renderer_end_scene)(void* inout_renderer);
};

/*******************************************************************************
 *
 *   RENDERER
 *
 ******************************************************************************/

// Sets up the shared part of a renderer, initializing its render state with
//  in_render_flags in the current graphics context. The tag is copied, and
//  freed by renderer_base_cleanup(); the camera and window aren't owned and
//  must outlive the renderer. The scene callbacks are left for the caller to
//  set
//
//  in:     in_render_flags, in_camera, in_parent_window, in_tag
//  out:    out_renderer
void renderer_base_init(
  struct renderer_base_t* out_renderer,
  const uint16_t in_render_flags,
  struct camera_base_t* in_camera,
  struct window_t* in_parent_window,
  const char* in_tag);

// Frees the renderer's tag
//
//  in/out: inout_renderer
void renderer_base_cleanup(
  struct renderer_base_t* inout_renderer);

// Opens a scene: binds and clears the renderer's render state, then calls its
//  renderer_begin_scene callback. Draw calls are counted against the
//  renderer, for diagnostics, until renderer_base_end_scene()
//
//  in/out: inout_renderer
void renderer_base_begin_scene(
  struct renderer_base_t* inout_renderer);

// Closes the scene opened by renderer_base_begin_scene(), after calling the
//  renderer's renderer_end_scene callback, which may still draw what it
//  batched
//
//  in/out: inout_renderer
void renderer_base_end_scene(
  struct renderer_base_t* inout_renderer);
