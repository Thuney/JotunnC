#pragma once

// The renderer that composites a window's layers: each layer's framebuffer is
//  drawn as a quad filling the window's default framebuffer, blended as
//  premultiplied alpha, with no projection

#include "fmatrix.h"
#include "framebuffer.h"
#include "renderable_2d_primitive.h"
#include "renderer.h"
#include "shader.h"

struct window_layer_t;

#define _RENDERER_WINDOW_LAYER_MAX_TEXTURES 16

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// The shader and buffers the quad is drawn with
struct renderer_window_layer_data_t
{
  struct shader_program_t framebuffer_shader;

  struct vertex_array_t vao;
  struct vertex_buffer_t vbo;
  struct element_buffer_t ebo;
};

struct renderer_window_layer_t
{
  struct renderer_base_t base;
  //
  struct camera_base_t camera_unprojected;
  struct renderer_window_layer_data_t render_data;
};

/*******************************************************************************
 *
 *   RENDERER
 *
 ******************************************************************************/

// Sets up the renderer in the current graphics context, loading its shader
//  and creating its buffers. The tag is copied; the window isn't owned
//
//  in:     in_parent_window, in_tag
//  out:    out_renderer
void renderer_window_layer_init(
  struct renderer_window_layer_t* out_renderer,
  struct window_t* in_parent_window,
  const char* in_tag);

// Releases the renderer's shader, buffers and tag
//
//  in/out: inout_renderer
void renderer_window_layer_cleanup(
  struct renderer_window_layer_t* inout_renderer);

// Opens the compositing scene: binds the window's default framebuffer, then
//  binds and clears the renderer's render state, and counts draw calls
//  against the renderer for diagnostics. Called directly rather than through
//  renderer_base_begin_scene(); it is also the renderer's
//  renderer_begin_scene callback
//
//  in:     in_renderer, the base of a struct renderer_window_layer_t
void renderer_window_layer_begin_scene(
  struct renderer_base_t* in_renderer);

// Closes the compositing scene opened by renderer_window_layer_begin_scene(),
//  unbinding the vertex array and framebuffer
//
//  in:     in_renderer, the base of a struct renderer_window_layer_t
void renderer_window_layer_end_scene(
  struct renderer_base_t* in_renderer);

// Draws a window layer's framebuffer over the whole render target, between
//  renderer_window_layer_begin_scene() and renderer_window_layer_end_scene().
//  in_layer_number is unused
//
//  in:     in_window_layer, in_layer_number
//  in/out: inout_renderer
void renderer_window_layer_draw(
  struct renderer_window_layer_t* inout_renderer,
  const struct window_layer_t* in_window_layer,
  uint8_t in_layer_number);
