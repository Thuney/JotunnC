#pragma once

// The GPU objects and CPU-side vertex storage behind renderer_2d's batches of
//  triangles, quads, textured quads, circles and lines, and behind its
//  meshes, with the vertex layouts each one's shader reads

#include "fvector.h"
#include "shader.h"
#include "u8vector.h"
#include "vertexarray.h"
#include "vertexbuffer.h"

/*******************************************************************************
 *
 *   SHAPES
 *
 ******************************************************************************/

// Number of triangles used to approximate a circle
#define _RENDERABLE_2D_CIRCLE_RESOLUTION (16)
#define _NUM_CIRCLE_2D_VERTICES (_RENDERABLE_2D_CIRCLE_RESOLUTION + 1)

// Each primitive's vertex positions before its transform: the triangle and
//  quads span the unit square from the origin, and the circle is its center
//  followed by points around the unit circle, which
//  renderable_2d_circle_data_init() fills in
extern const union fvector3_t triangle_2d_position_data[3];
extern const union fvector3_t quad_2d_position_data[4];
extern const union fvector3_t textured_quad_2d_position_data[4];
extern const union fvector2_t textured_quad_texture_coordinates[4];
extern union fvector3_t circle_2d_position_data[_NUM_CIRCLE_2D_VERTICES];

/*******************************************************************************
 *
 *   VERTEX LAYOUTS
 *
 ******************************************************************************/

// Each primitive's vertex attributes, indexing its layout
enum renderable_2d_triangle_vertex_attribute_t
{
  RENDERABLE_2D_TRIANGLE_VERTEX_ATTRIBUTE_POSITION = 0x00,
  RENDERABLE_2D_TRIANGLE_VERTEX_ATTRIBUTE_COLOR,
  RENDERABLE_2D_TRIANGLE_VERTEX_ATTRIBUTE_COUNT
};

enum renderable_2d_quad_vertex_attribute_t
{
  RENDERABLE_2D_QUAD_VERTEX_ATTRIBUTE_POSITION = 0x00,
  RENDERABLE_2D_QUAD_VERTEX_ATTRIBUTE_COLOR,
  RENDERABLE_2D_QUAD_VERTEX_ATTRIBUTE_COUNT
};

enum renderable_2d_textured_quad_vertex_attribute_t
{
  RENDERABLE_2D_TEXTURED_QUAD_VERTEX_ATTRIBUTE_POSITION = 0x00,
  RENDERABLE_2D_TEXTURED_QUAD_VERTEX_ATTRIBUTE_TEXTURE_COORDINATES,
  RENDERABLE_2D_TEXTURED_QUAD_VERTEX_ATTRIBUTE_TEXTURE_INDEX,
  RENDERABLE_2D_TEXTURED_QUAD_VERTEX_ATTRIBUTE_TILING_FACTOR,
  RENDERABLE_2D_TEXTURED_QUAD_VERTEX_ATTRIBUTE_COUNT
};

enum renderable_2d_circle_vertex_attribute_t
{
  RENDERABLE_2D_CIRCLE_VERTEX_ATTRIBUTE_POSITION = 0x00,
  RENDERABLE_2D_CIRCLE_VERTEX_ATTRIBUTE_COLOR,
  RENDERABLE_2D_CIRCLE_VERTEX_ATTRIBUTE_COUNT
};

enum renderable_2d_line_vertex_attribute_t
{
  RENDERABLE_2D_LINE_VERTEX_ATTRIBUTE_POSITION = 0x00,
  RENDERABLE_2D_LINE_VERTEX_ATTRIBUTE_COLOR,
  RENDERABLE_2D_LINE_VERTEX_ATTRIBUTE_COUNT
};

enum renderable_2d_mesh_vertex_attribute_t
{
  RENDERABLE_2D_MESH_VERTEX_ATTRIBUTE_POSITION = 0x00,
  RENDERABLE_2D_MESH_VERTEX_ATTRIBUTE_TEXTURE_COORDINATES,
  RENDERABLE_2D_MESH_VERTEX_ATTRIBUTE_COLOR,
  RENDERABLE_2D_MESH_VERTEX_ATTRIBUTE_COUNT
};

// Each primitive's vertex, as its shader reads it
struct renderable_2d_triangle_vertex_t
{
  union fvector3_t position;
  union fvector4_t color;
};

struct renderable_2d_quad_vertex_t
{
  union fvector3_t position;
  union fvector4_t color;
};

struct renderable_2d_textured_quad_vertex_t
{
  union fvector3_t position;
  union fvector2_t texture_coordinate;
  float texture_index;
  float tiling_factor;
};

struct renderable_2d_circle_vertex_t
{
  union fvector3_t position;
  union fvector4_t color;
};

struct renderable_2d_line_vertex_t
{
  union fvector3_t position;
  union fvector4_t color;
};

// A vertex of an arbitrary textured, colored mesh. Packed to 20 bytes, with
//  the color normalized from bytes as it is read by the shader
struct renderable_2d_mesh_vertex_t
{
  union fvector2_t position;
  union fvector2_t texture_coordinate;
  union u8vector4_t color;
};

/*******************************************************************************
 *
 *   PRIMITIVE DATA
 *
 ******************************************************************************/

// Each batched primitive's GPU objects, with the vertices submitted to the
//  batch so far stored between vertex_data_base and vertex_data_ptr
struct renderable_2d_triangle_data_t
{
  struct vertex_array_t vao;
  struct vertex_buffer_t vbo;
  struct element_buffer_t ebo;
  //
  unsigned int triangle_index_count;
  // Data pointers
  struct renderable_2d_triangle_vertex_t* vertex_data_base;
  struct renderable_2d_triangle_vertex_t* vertex_data_ptr;
};

struct renderable_2d_quad_data_t
{
  struct vertex_array_t vao;
  struct vertex_buffer_t vbo;
  struct element_buffer_t ebo;
  //
  unsigned int quad_index_count;
  // Data pointers
  struct renderable_2d_quad_vertex_t* vertex_data_base;
  struct renderable_2d_quad_vertex_t* vertex_data_ptr;
};

struct renderable_2d_textured_quad_data_t
{
  struct vertex_array_t vao;
  struct vertex_buffer_t vbo;
  struct element_buffer_t ebo;
  //
  unsigned int textured_quad_index_count;
  // Data pointers
  struct renderable_2d_textured_quad_vertex_t* vertex_data_base;
  struct renderable_2d_textured_quad_vertex_t* vertex_data_ptr;
};

struct renderable_2d_circle_data_t
{
  struct vertex_array_t vao;
  struct vertex_buffer_t vbo;
  struct element_buffer_t ebo;
  //
  unsigned int circle_index_count;
  // Data pointers
  struct renderable_2d_circle_vertex_t* vertex_data_base;
  struct renderable_2d_circle_vertex_t* vertex_data_ptr;
};

struct renderable_2d_line_data_t
{
  struct vertex_array_t vao;
  struct vertex_buffer_t vbo;
  //
  unsigned int line_vertex_count;
  // Data pointers
  struct renderable_2d_line_vertex_t* vertex_data_base;
  struct renderable_2d_line_vertex_t* vertex_data_ptr;
};

// GPU objects for meshes, which are uploaded whole as they are drawn rather
//  than accumulated into a batch, so no CPU-side storage is kept
struct renderable_2d_mesh_data_t
{
  struct vertex_array_t vao;
  struct vertex_buffer_t vbo;
  struct element_buffer_t ebo;
};

/*******************************************************************************
 *
 *   INITIALIZATION AND CLEANUP
 *
 ******************************************************************************/

// Creates the vertex array and buffers for batches of up to in_max_count
//  triangles, binds the triangle vertex layout to in_shader_program's
//  attributes, and keeps a batch's vertices in inout_vertex_storage, which
//  must hold 3 for each triangle and outlive the batch. Indices are
//  uploaded once, a chunk at a time
//
//  in:     in_max_count, in_shader_program
//  in/out: inout_vertex_storage
//  out:    out_triangle_data
void renderable_2d_triangle_data_init(
  struct renderable_2d_triangle_data_t* out_triangle_data,
  unsigned int in_max_count,
  struct renderable_2d_triangle_vertex_t* inout_vertex_storage,
  struct shader_program_t* in_shader_program);

// Creates the vertex array and buffers for batches of up to in_max_count
//  quads, binds the quad vertex layout to in_shader_program's
//  attributes, and keeps a batch's vertices in inout_vertex_storage, which
//  must hold 4 for each quad and outlive the batch. Indices are
//  uploaded once, a chunk at a time
//
//  in:     in_max_count, in_shader_program
//  in/out: inout_vertex_storage
//  out:    out_quad_data
void renderable_2d_quad_data_init(
  struct renderable_2d_quad_data_t* out_quad_data,
  unsigned int in_max_count,
  struct renderable_2d_quad_vertex_t* inout_vertex_storage,
  struct shader_program_t* in_shader_program);

// Creates the vertex array and buffers for batches of up to in_max_count
//  textured quads, binds the textured quad vertex layout to in_shader_program's
//  attributes, and keeps a batch's vertices in inout_vertex_storage, which
//  must hold 4 for each textured quad and outlive the batch. Indices are
//  uploaded once, a chunk at a time
//
//  in:     in_max_count, in_shader_program
//  in/out: inout_vertex_storage
//  out:    out_textured_quad_data
void renderable_2d_textured_quad_data_init(
  struct renderable_2d_textured_quad_data_t* out_textured_quad_data,
  unsigned int in_max_count,
  struct renderable_2d_textured_quad_vertex_t* inout_vertex_storage,
  struct shader_program_t* in_shader_program);

// Creates the vertex array and buffers for batches of up to in_max_count
//  circles, binds the circle vertex layout to in_shader_program's
//  attributes, and keeps a batch's vertices in inout_vertex_storage, which
//  must hold _NUM_CIRCLE_2D_VERTICES for each circle and outlive the batch.
//  Indices are uploaded once, a chunk at a time. Also fills in
//  circle_2d_position_data
//
//  in:     in_max_count, in_shader_program
//  in/out: inout_vertex_storage
//  out:    out_circle_data
void renderable_2d_circle_data_init(
  struct renderable_2d_circle_data_t* out_circle_data,
  unsigned int in_max_count,
  struct renderable_2d_circle_vertex_t* inout_vertex_storage,
  struct shader_program_t* in_shader_program);

// Creates the vertex array and buffer for batches of up to in_max_count
//  lines, binds the line vertex layout to in_shader_program's attributes,
//  and keeps a batch's vertices in inout_vertex_storage, which must hold 2
//  for each line and outlive the batch. Lines are drawn without
//  indices
//
//  in:     in_max_count, in_shader_program
//  in/out: inout_vertex_storage
//  out:    out_line_data
void renderable_2d_line_data_init(
  struct renderable_2d_line_data_t* out_line_data,
  unsigned int in_max_count,
  struct renderable_2d_line_vertex_t* inout_vertex_storage,
  struct shader_program_t* in_shader_program);

// Destroys the GPU objects made by
//  renderable_2d_triangle_data_init()
//
//  in/out: inout_triangle_data
void renderable_2d_triangle_data_cleanup(
  struct renderable_2d_triangle_data_t* inout_triangle_data);

// Destroys the GPU objects made by
//  renderable_2d_quad_data_init()
//
//  in/out: inout_quad_data
void renderable_2d_quad_data_cleanup(
  struct renderable_2d_quad_data_t* inout_quad_data);

// Destroys the GPU objects made by
//  renderable_2d_textured_quad_data_init()
//
//  in/out: inout_textured_quad_data
void renderable_2d_textured_quad_data_cleanup(
  struct renderable_2d_textured_quad_data_t* inout_textured_quad_data);

// Destroys the GPU objects made by
//  renderable_2d_circle_data_init()
//
//  in/out: inout_circle_data
void renderable_2d_circle_data_cleanup(
  struct renderable_2d_circle_data_t* inout_circle_data);

// Destroys the GPU objects made by
//  renderable_2d_line_data_init()
//
//  in/out: inout_line_data
void renderable_2d_line_data_cleanup(
  struct renderable_2d_line_data_t* inout_line_data);

// Creates the vertex array and buffers for meshes and binds the mesh vertex
//  layout to in_shader_program's attributes
//
//  in:     in_shader_program
//  out:    out_mesh_data
void renderable_2d_mesh_data_init(
  struct renderable_2d_mesh_data_t* out_mesh_data,
  struct shader_program_t* in_shader_program);

// Destroys the vertex array and buffers created by
//  renderable_2d_mesh_data_init()
//
//  in/out: inout_mesh_data
void renderable_2d_mesh_data_cleanup(
  struct renderable_2d_mesh_data_t* inout_mesh_data);
