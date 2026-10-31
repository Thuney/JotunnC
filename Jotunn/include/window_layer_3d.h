#pragma once

// A window layer drawn with a renderer_3d of its own, through a perspective
//  camera fitted to the window. A window layer only points at the
//  framebuffer, camera and renderer it draws with; this keeps the four
//  together, set up the way a 3D layer covering its window needs them

#include "camera.h"
#include "framebuffer.h"
#include "renderer_3d.h"
#include "window_layer.h"

#include <stdint.h>

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// A 3D layer. Its run is called with window_layer, from which whatever
//  embeds the layer is got back with container_of()
struct window_layer_3d_t
{
  struct window_layer_t window_layer;
  struct framebuffer_t framebuffer;
  struct camera_perspective_t camera_perspective;
  struct renderer_3d_t renderer_3d;
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Sets up a 3D layer covering in_parent_window, with a framebuffer of the
//  window's size, calling in_function_run each frame between the beginning
//  and end of its renderer's scene. The camera sees in_field_of_view_radians
//  vertically across the window's aspect ratio, between the given clip
//  planes, and starts at the origin looking along -Z with +Y as up; move it
//  with camera_set_view(). The renderer keeps its batches in
//  inout_renderer_storage, which must outlive the layer; when it's NULL, the
//  storage is allocated instead. The renderer keeps in_renderer_tag, which
//  names it in diagnostics and must outlive it. The window's graphics context
//  must be current. The layer isn't added to the window: pass its
//  window_layer to window_add_layer(), in the order the window's layers
//  should be drawn
//
//  in:     in_parent_window, in_renderer_tag, in_field_of_view_radians,
//            in_near_plane, in_far_plane, in_function_run
//  in/out: inout_renderer_storage
//  out:    out_layer
//  ret:    0 on success; 1 when inout_renderer_storage is NULL and the
//            storage can't be allocated, leaving nothing to clean up
uint8_t window_layer_3d_init(
  struct window_t* in_parent_window,
  struct window_layer_3d_t* out_layer,
  struct renderer_3d_storage_t* inout_renderer_storage,
  const char* in_renderer_tag,
  float in_field_of_view_radians,
  float in_near_plane,
  float in_far_plane,
  void (*in_function_run)(
    struct window_layer_t* inout_window_layer));

// Releases what window_layer_3d_init() made, including the renderer's
//  storage if it was allocated rather than provided. The parent window's
//  graphics context must be current
//
//  in/out: inout_layer
void window_layer_3d_cleanup(
  struct window_layer_3d_t* inout_layer);
