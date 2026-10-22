#pragma once

struct typeface_t;
struct window_t;

#include "fmatrix.h"
#include "font.h"
#include "renderable_2d_primitive.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"

#include <stdbool.h>
#include <stdint.h>

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
  // Either ELEMENT_UNSIGNED_SHORT or ELEMENT_UNSIGNED_INT
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

struct renderer_2d_data_t
{
  fmatrix_4x4 model_matrix;

  struct shader_program_t triangle_shader;
  struct shader_program_t quad_shader;
  struct shader_program_t textured_quad_shader;
  struct shader_program_t circle_shader;
  struct shader_program_t line_shader;
  struct shader_program_t mesh_shader;
  //
  struct texture_2d_t* textures[_RENDERER_2D_MAX_TEXTURES];
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
  //
  float line_width;
};

struct renderer_2d_t
{
  struct renderer_base_t base;
  //
  struct renderer_2d_data_t render_data;
};

void renderer_2d_init(
  struct renderer_2d_t* renderer,
  struct camera_base_t* camera,
  struct window_t* parent_window,
  const char* tag);
void renderer_2d_cleanup(struct renderer_2d_t* renderer);

void renderer_2d_begin_scene(void* renderer);
void renderer_2d_end_scene(void* renderer);

void renderer_2d_draw_triangle(
  struct renderer_2d_t* renderer,
  const fmatrix_4x4* transform,
  const fvector4 color);

void renderer_2d_draw_quad(
  struct renderer_2d_t* renderer,
  const fmatrix_4x4* transform,
  const fvector4 color);
void renderer_2d_draw_unfilled_quad(
  struct renderer_2d_t* renderer,
  const fmatrix_4x4* transform,
  const fvector4 color);
void renderer_2d_draw_textured_quad(
  struct renderer_2d_t* renderer,
  const fmatrix_4x4* transform,
  struct texture_2d_t* texture);
void renderer_2d_draw_subtextured_quad(
  struct renderer_2d_t* renderer,
  const fmatrix_4x4* transform,
  struct texture_2d_t* texture,
  const fvector2 subtexture_coords[4]);

void renderer_2d_draw_circle(
  struct renderer_2d_t* renderer,
  const fmatrix_4x4* transform,
  const fvector4 color);
void renderer_2d_draw_line(
  struct renderer_2d_t* renderer,
  const fvector3 pos_1,
  const fvector3 pos_2,
  const fvector4 color);
// Draws a mesh on top of everything submitted to the renderer before it,
//  flushing the pending batch first. Unlike the other primitives the mesh is
//  drawn immediately, so its data need only stay valid for the call
//
//  in:     in_mesh
//  in/out: inout_renderer
void renderer_2d_draw_mesh(
  struct renderer_2d_t* inout_renderer,
  const struct renderer_2d_mesh_t* in_mesh);

void renderer_2d_draw_string(
  struct renderer_2d_t* renderer,
  const struct typeface_t* typeface,
  const fvector3 start_position,
  const char* draw_string);

void renderer_2d_set_line_width(
  struct renderer_2d_t* renderer,
  const float line_width);
