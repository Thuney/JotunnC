#pragma once

// A renderer for 3D scenes, lit by a single directional light. Cubes,
//  pyramids, spheres, textured cubes and lines are collected into batches
//  between renderer_3d_begin_scene() and renderer_3d_end_scene(), and drawn
//  when a batch fills or the scene ends, with depth testing sorting out which
//  is in front. Each primitive is transformed into world space as it's
//  batched, so the camera's view and projection are all the shaders apply

struct window_t;

#include "fmatrix.h"
#include "renderable_3d_primitive.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"

#include <stdbool.h>
#include <stdint.h>

// The most textures a batch of textured cubes can sample from
#define _RENDERER_3D_MAX_TEXTURES 16

// The most of each kind of primitive (cubes, pyramids, spheres, textured
//  cubes and lines) a batch holds; drawing one more draws the batch first.
//  It sizes renderer_3d_storage_t, so Jotunn and the application must agree
//  on it: set it through the JOTUNN_RENDERER_3D_MAX_PRIMITIVES_PER_BATCH
//  CMake cache variable, which defines it for both
#ifndef RENDERER_3D_MAX_PRIMITIVES_PER_BATCH
  #define RENDERER_3D_MAX_PRIMITIVES_PER_BATCH 128
#endif

/*******************************************************************************
 *
 *   RENDERER
 *
 ******************************************************************************/

// A single directional light, falling on every cube, pyramid, sphere and
//  textured cube; lines are unlit
struct renderer_3d_light_t
{
  // The direction the light travels in, in world space
  union fvector3_t direction;
  union fvector3_t color;
  // The fraction of the light's color a surface gets whichever way it faces
  float ambient_strength;
};

// Where a renderer keeps the vertices of a batch until it's drawn, with room
//  for RENDERER_3D_MAX_PRIMITIVES_PER_BATCH of each kind of primitive. The
//  spheres take most of it; at the default of 128 it's about 2.5 MB
struct renderer_3d_storage_t
{
  struct renderable_3d_shaded_vertex_t
    cube_vertices[_NUM_CUBE_3D_VERTICES * RENDERER_3D_MAX_PRIMITIVES_PER_BATCH];
  struct renderable_3d_shaded_vertex_t
    pyramid_vertices[_NUM_PYRAMID_3D_VERTICES * RENDERER_3D_MAX_PRIMITIVES_PER_BATCH];
  struct renderable_3d_shaded_vertex_t
    sphere_vertices[_NUM_SPHERE_3D_VERTICES * RENDERER_3D_MAX_PRIMITIVES_PER_BATCH];
  struct renderable_3d_textured_vertex_t
    textured_cube_vertices[_NUM_CUBE_3D_VERTICES * RENDERER_3D_MAX_PRIMITIVES_PER_BATCH];
  struct renderable_3d_line_vertex_t
    line_vertices[2 * RENDERER_3D_MAX_PRIMITIVES_PER_BATCH];
};

// The renderer's shaders, batches and light
struct renderer_3d_data_t
{
  // Identity, as primitives are batched in world space
  union fmatrix_4x4_t model_matrix;

  struct shader_program_t shaded_shader;
  struct shader_program_t textured_shader;
  struct shader_program_t line_shader;
  //
  // The textures the batch's textured cubes sample, and how many there are
  const struct texture_2d_t* textures[_RENDERER_3D_MAX_TEXTURES];
  unsigned int textures_index;
  //
  struct renderable_3d_shaded_data_t cube_data;
  struct renderable_3d_shaded_data_t pyramid_data;
  struct renderable_3d_shaded_data_t sphere_data;
  struct renderable_3d_textured_data_t textured_cube_data;
  struct renderable_3d_line_data_t line_data;
  //
  struct renderer_3d_light_t light;
  // Applied each time the batch's lines are drawn
  float line_width_pixels;
  // Where the batches' vertices are kept
  struct renderer_3d_storage_t* storage;
  // Set when renderer_3d_init() allocated storage, which
  //  renderer_3d_cleanup() then frees; clear when the caller provided it
  bool is_storage_owned;
};

// A 3D renderer, usable wherever a renderer_base_t is through base. Its
//  camera is normally a camera_perspective_t
struct renderer_3d_t
{
  struct renderer_base_t base;
  //
  struct renderer_3d_data_t render_data;
};

/*******************************************************************************
 *
 *   INITIALIZATION AND CLEANUP
 *
 ******************************************************************************/

// Loads the renderer's shaders from Jotunn's res/shaders and creates its
//  batches in the current graphics context, logging any shader that fails to
//  load in DEBUG builds. The light starts white, falling down and away from
//  a camera looking along -Z, with an ambient strength of 0.25. The renderer
//  keeps in_camera, in_parent_window and in_tag, which names it in
//  diagnostics; all three must outlive it. The batches' vertices are kept in
//  inout_storage, which must outlive the renderer; when it's NULL, the
//  storage is allocated instead
//
//  in:     in_camera, in_parent_window, in_tag
//  in/out: inout_storage
//  out:    out_renderer
//  ret:    0 on success; 1 when inout_storage is NULL and the storage can't
//            be allocated, leaving nothing to clean up
uint8_t renderer_3d_init(
  struct renderer_3d_t* out_renderer,
  struct renderer_3d_storage_t* inout_storage,
  struct camera_base_t* in_camera,
  struct window_t* in_parent_window,
  const char* in_tag);

// Releases everything renderer_3d_init() made, including the storage for the
//  batches' vertices if it was allocated rather than provided. The parent
//  window's graphics context must be current
//
//  in/out: inout_renderer
void renderer_3d_cleanup(
  struct renderer_3d_t* inout_renderer);

/*******************************************************************************
 *
 *   SCENES
 *
 ******************************************************************************/

// Starts a scene with an empty batch. The renderer's renderer_begin_scene
//  hook, so inout_renderer is the base of a struct renderer_3d_t
//
//  in/out: inout_renderer
void renderer_3d_begin_scene(
  struct renderer_base_t* inout_renderer);

// Draws what's left of the batch and ends the scene, unbinding the vertex
//  array and framebuffer. The renderer's renderer_end_scene hook, so
//  in_renderer is the base of a struct renderer_3d_t
//
//  in:     in_renderer
void renderer_3d_end_scene(
  struct renderer_base_t* in_renderer);

/*******************************************************************************
 *
 *   DRAWING
 *
 ******************************************************************************/

// Adds a cube to the batch: the one spanning -0.5 to 0.5 on each axis, moved
//  by in_transform
//
//  in:     in_transform, in_color
//  in/out: inout_renderer
void renderer_3d_draw_cube(
  struct renderer_3d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color);

// Adds a cube's edges to the batch, as twelve lines: the cube spanning -0.5
//  to 0.5 on each axis, moved by in_transform
//
//  in:     in_transform, in_color
//  in/out: inout_renderer
void renderer_3d_draw_unfilled_cube(
  struct renderer_3d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color);

// Adds a cube with a whole texture on each face to the batch: the cube
//  spanning -0.5 to 0.5 on each axis, moved by in_transform. The renderer
//  keeps in_texture until the batch is drawn
//
//  in:     in_transform, in_texture
//  in/out: inout_renderer
void renderer_3d_draw_textured_cube(
  struct renderer_3d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  const struct texture_2d_t* in_texture);

// Adds a square pyramid to the batch: its base spanning -0.5 to 0.5 on X and
//  Z at a Y of -0.5, and its apex at (0, 0.5, 0), moved by in_transform
//
//  in:     in_transform, in_color
//  in/out: inout_renderer
void renderer_3d_draw_pyramid(
  struct renderer_3d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color);

// Adds a sphere to the batch: the unit sphere about the origin, moved by
//  in_transform, approximated by _RENDERABLE_3D_SPHERE_SLICES slices and
//  _RENDERABLE_3D_SPHERE_STACKS stacks
//
//  in:     in_transform, in_color
//  in/out: inout_renderer
void renderer_3d_draw_sphere(
  struct renderer_3d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color);

// Adds a line between two points in world space to the batch
//
//  in:     in_pos_1, in_pos_2, in_color
//  in/out: inout_renderer
void renderer_3d_draw_line(
  struct renderer_3d_t* inout_renderer,
  union fvector3_t in_pos_1,
  union fvector3_t in_pos_2,
  union fvector4_t in_color);

// Adds a grid on the XZ plane to the batch, as lines one unit apart reaching
//  in_half_extent_units from the origin each way. The two lines through the
//  origin are drawn as the X axis in red and the Z axis in blue, the rest in
//  in_color
//
//  in:     in_half_extent_units, in_color
//  in/out: inout_renderer
void renderer_3d_draw_grid(
  struct renderer_3d_t* inout_renderer,
  uint32_t in_half_extent_units,
  union fvector4_t in_color);

// Adds the X, Y and Z axes to the batch, as lines in_length_units long from
//  the origin along each, in red, green and blue
//
//  in:     in_length_units
//  in/out: inout_renderer
void renderer_3d_draw_axes(
  struct renderer_3d_t* inout_renderer,
  float in_length_units);

/*******************************************************************************
 *
 *   SETTINGS
 *
 ******************************************************************************/

// Sets the width the renderer draws its lines with, from the next time its
//  batch is drawn
//
//  in:     in_line_width_pixels
//  in/out: inout_renderer
void renderer_3d_set_line_width(
  struct renderer_3d_t* inout_renderer,
  float in_line_width_pixels);

// Sets the light falling on the renderer's lit primitives, from the next time
//  its batch is drawn. in_direction needn't be of unit length
//
//  in:     in_direction, in_color, in_ambient_strength
//  in/out: inout_renderer
void renderer_3d_set_light(
  struct renderer_3d_t* inout_renderer,
  union fvector3_t in_direction,
  union fvector3_t in_color,
  float in_ambient_strength);
