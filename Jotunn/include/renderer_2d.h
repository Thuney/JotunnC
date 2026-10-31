#pragma once

// A renderer for 2D scenes. Triangles, quads, textured quads, circles and
//  lines are collected into batches between renderer_2d_begin_scene() and
//  renderer_2d_end_scene(), and drawn when a batch fills or the scene ends;
//  within a batch they're drawn in that order, whatever order they came in.
//  Strings are batched as textured quads, while meshes are drawn at once.
//  Positions are in the coordinates of the renderer's camera

struct font_typeface_t;
struct window_t;

#include "fmatrix.h"
#include "font.h"
#include "renderable_2d_primitive.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"

#include <stdbool.h>
#include <stdint.h>

// The most textures a batch of textured quads can sample from
#define _RENDERER_2D_MAX_TEXTURES 16

/*******************************************************************************
 *
 *   MESHES
 *
 ******************************************************************************/

// A rectangle of the render target in pixels, with the origin at its
//  bottom-left corner
struct renderer_2d_clip_rect_t
{
  int32_t x_pixels;
  int32_t y_pixels;
  int32_t width_pixels;
  int32_t height_pixels;
};

// A range of a mesh's indices, drawn with one texture and clip rectangle
struct renderer_2d_submesh_t
{
  // Multiplied with the vertex colors; NULL leaves the vertex colors as they
  //  are
  const struct texture_2d_t* texture;
  // When set, pixels outside of clip_rect are left untouched
  bool is_clipped;
  struct renderer_2d_clip_rect_t clip_rect;
  // The range drawn, within the mesh's indices
  uint32_t index_offset;
  uint32_t index_count;
  // Added to each index in the range before it selects a vertex
  uint32_t vertex_offset;
};

// An arbitrary triangle mesh, in the coordinates of the renderer's camera,
//  drawn as one or more submeshes
struct renderer_2d_mesh_t
{
  const struct renderable_2d_mesh_vertex_t* vertices;
  uint32_t vertex_count;
  // Either RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_SHORT or
  //  RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_INT
  enum render_api_element_data_type_t index_type;
  const void* indices;
  uint32_t index_count;
  const struct renderer_2d_submesh_t* submeshes;
  uint32_t submesh_count;
};

/*******************************************************************************
 *
 *   RENDERER
 *
 ******************************************************************************/

// The renderer's shaders and batches
struct renderer_2d_data_t
{
  union fmatrix_4x4_t model_matrix;

  struct shader_program_t triangle_shader;
  struct shader_program_t quad_shader;
  struct shader_program_t textured_quad_shader;
  struct shader_program_t circle_shader;
  struct shader_program_t line_shader;
  struct shader_program_t mesh_shader;
  //
  // The textures the batch's textured quads sample, and how many there are
  const struct texture_2d_t* textures[_RENDERER_2D_MAX_TEXTURES];
  unsigned int textures_index;
  //
  struct renderable_2d_triangle_data_t triangle_data;
  struct renderable_2d_quad_data_t quad_data;
  struct renderable_2d_textured_quad_data_t textured_quad_data;
  struct renderable_2d_circle_data_t circle_data;
  struct renderable_2d_line_data_t line_data;
  struct renderable_2d_mesh_data_t mesh_data;
  // Sampled by submeshes without a texture of their own
  struct texture_2d_t white_texture;
  // Applied each time the batch's lines are drawn
  float line_width_pixels;
};

// A 2D renderer, usable wherever a renderer_base_t is through base
struct renderer_2d_t
{
  struct renderer_base_t base;
  //
  struct renderer_2d_data_t render_data;
};

/*******************************************************************************
 *
 *   INITIALIZATION AND CLEANUP
 *
 ******************************************************************************/

// Loads the renderer's shaders from Jotunn's res/shaders and creates its
//  batches, logging any shader that fails to load in DEBUG builds. The
//  renderer keeps in_camera, in_parent_window and in_tag, which names it in
//  diagnostics; all three must outlive it
//
//  in:     in_camera, in_parent_window, in_tag
//  out:    out_renderer
void renderer_2d_init(
  struct renderer_2d_t* out_renderer,
  struct camera_base_t* in_camera,
  struct window_t* in_parent_window,
  const char* in_tag);

// Releases everything renderer_2d_init() made
//
//  in/out: inout_renderer
void renderer_2d_cleanup(
  struct renderer_2d_t* inout_renderer);

/*******************************************************************************
 *
 *   SCENES
 *
 ******************************************************************************/

// Starts a scene with an empty batch. The renderer's renderer_begin_scene
//  hook, so inout_renderer is the base of a struct renderer_2d_t
//
//  in/out: inout_renderer
void renderer_2d_begin_scene(
  struct renderer_base_t* inout_renderer);

// Draws what's left of the batch and ends the scene, unbinding the vertex
//  array and framebuffer. The renderer's renderer_end_scene hook, so
//  in_renderer is the base of a struct renderer_2d_t
//
//  in:     in_renderer
void renderer_2d_end_scene(
  struct renderer_base_t* in_renderer);

/*******************************************************************************
 *
 *   DRAWING
 *
 ******************************************************************************/

// Adds a triangle to the batch: the one with corners (0, 0), (0.5, 1) and
//  (1, 0), moved by in_transform
//
//  in:     in_transform, in_color
//  in/out: inout_renderer
void renderer_2d_draw_triangle(
  struct renderer_2d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color);

// Adds a quad to the batch: the unit square from the origin, moved by
//  in_transform
//
//  in:     in_transform, in_color
//  in/out: inout_renderer
void renderer_2d_draw_quad(
  struct renderer_2d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color);

// Adds a quad's outline to the batch, as four lines: the unit square from
//  the origin, moved by in_transform
//
//  in:     in_transform, in_color
//  in/out: inout_renderer
void renderer_2d_draw_unfilled_quad(
  struct renderer_2d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color);

// Adds a quad covered by a whole texture to the batch: the unit square from
//  the origin, moved by in_transform. The renderer keeps in_texture until the
//  batch is drawn
//
//  in:     in_transform, in_texture
//  in/out: inout_renderer
void renderer_2d_draw_textured_quad(
  struct renderer_2d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  const struct texture_2d_t* in_texture);

// Adds a quad covered by part of a texture to the batch: the unit square
//  from the origin, moved by in_transform, with in_subtexture_coords giving
//  the texture coordinates of its corners in the order (0, 0), (1, 0),
//  (1, 1), (0, 1). The renderer keeps in_texture until the batch is drawn
//
//  in:     in_transform, in_texture, in_subtexture_coords
//  in/out: inout_renderer
void renderer_2d_draw_subtextured_quad(
  struct renderer_2d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  const struct texture_2d_t* in_texture,
  const union fvector2_t in_subtexture_coords[4]);

// Adds a circle to the batch: the unit circle around the origin, moved by
//  in_transform, approximated by _RENDERABLE_2D_CIRCLE_RESOLUTION triangles
//
//  in:     in_transform, in_color
//  in/out: inout_renderer
void renderer_2d_draw_circle(
  struct renderer_2d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color);

// Adds a line between two points to the batch
//
//  in:     in_pos_1, in_pos_2, in_color
//  in/out: inout_renderer
void renderer_2d_draw_line(
  struct renderer_2d_t* inout_renderer,
  union fvector3_t in_pos_1,
  union fvector3_t in_pos_2,
  union fvector4_t in_color);

// Draws a mesh on top of everything submitted to the renderer before it,
//  flushing the pending batch first. Unlike the other primitives the mesh is
//  drawn immediately, so its data need only stay valid for the call
//
//  in:     in_mesh
//  in/out: inout_renderer
void renderer_2d_draw_mesh(
  struct renderer_2d_t* inout_renderer,
  const struct renderer_2d_mesh_t* in_mesh);

// Adds a string to the batch as a textured quad per character, one unit per
//  pixel of the typeface. The baseline sits the tallest glyph's height below
//  in_start_position. Characters without a glyph, outside the typeface's
//  loaded range or in a typeface not loaded, are skipped
//
//  in:     in_typeface, in_start_position, in_draw_string
//  in/out: inout_renderer
void renderer_2d_draw_string(
  struct renderer_2d_t* inout_renderer,
  const struct font_typeface_t* in_typeface,
  union fvector3_t in_start_position,
  const char* in_draw_string);
/*******************************************************************************
 *
 *   SETTINGS
 *
 ******************************************************************************/

// Sets the graphics API's line width. The renderer applies its own line
//  width whenever it draws its lines, so this lasts only until the next
//  batch is drawn. in_renderer is unused
//
//  in:     in_renderer, in_line_width_pixels
void renderer_2d_set_line_width(
  const struct renderer_2d_t* in_renderer,
  float in_line_width_pixels);
