#pragma once

// The GPU objects and CPU-side vertex storage behind renderer_3d's batches of
//  lit meshes (cubes, pyramids and spheres), textured cubes and lines, along
//  with the meshes they repeat and the vertex layouts each one's shader reads

#include "fvector.h"
#include "shader.h"
#include "vertexarray.h"
#include "vertexbuffer.h"

/*******************************************************************************
 *
 *   SHAPES
 *
 ******************************************************************************/

// Number of longitudinal slices and latitudinal stacks approximating a sphere
#define _RENDERABLE_3D_SPHERE_SLICES (24)
#define _RENDERABLE_3D_SPHERE_STACKS (16)
#define _NUM_SPHERE_3D_VERTICES \
  ((_RENDERABLE_3D_SPHERE_SLICES + 1) * (_RENDERABLE_3D_SPHERE_STACKS + 1))
#define _NUM_SPHERE_3D_INDICES \
  (6 * _RENDERABLE_3D_SPHERE_SLICES * _RENDERABLE_3D_SPHERE_STACKS)

// Four vertices to a face, so that each face has a normal of its own
#define _NUM_CUBE_3D_VERTICES (24)
#define _NUM_CUBE_3D_INDICES (36)

// Three vertices to each side and four to the base
#define _NUM_PYRAMID_3D_VERTICES (16)
#define _NUM_PYRAMID_3D_INDICES (18)

// Each of the cube's 12 edges as a pair of corners
#define _NUM_CUBE_3D_EDGE_VERTICES (24)

// A mesh in its own space, which a batch repeats once for each copy drawn.
//  Each face is wound counter-clockwise seen from outside
struct renderable_3d_mesh_t
{
  const union fvector3_t* positions;
  const union fvector3_t* normals;
  const union fvector2_t* texture_coordinates;
  unsigned int vertex_count;
  //
  const unsigned int* indices;
  unsigned int index_count;
};

// The meshes the primitives are drawn from: the cube spans -0.5 to 0.5 on
//  each axis, the pyramid stands in that same box with its apex up, and the
//  sphere has a radius of 1 about the origin. The sphere's data is filled in
//  by the first renderable_3d_shaded_data_init() or
//  renderable_3d_textured_data_init()
extern const struct renderable_3d_mesh_t cube_3d_mesh;
extern const struct renderable_3d_mesh_t pyramid_3d_mesh;
extern const struct renderable_3d_mesh_t sphere_3d_mesh;

// The cube mesh's edges, for drawing its outline as lines
extern const union fvector3_t cube_3d_edge_position_data[_NUM_CUBE_3D_EDGE_VERTICES];

/*******************************************************************************
 *
 *   VERTEX LAYOUTS
 *
 ******************************************************************************/

// Each kind of vertex's attributes, indexing its layout
enum renderable_3d_shaded_vertex_attribute_t
{
  RENDERABLE_3D_SHADED_VERTEX_ATTRIBUTE_POSITION = 0x00,
  RENDERABLE_3D_SHADED_VERTEX_ATTRIBUTE_NORMAL,
  RENDERABLE_3D_SHADED_VERTEX_ATTRIBUTE_COLOR,
  RENDERABLE_3D_SHADED_VERTEX_ATTRIBUTE_COUNT
};

enum renderable_3d_textured_vertex_attribute_t
{
  RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_POSITION = 0x00,
  RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_NORMAL,
  RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_TEXTURE_COORDINATES,
  RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_TEXTURE_INDEX,
  RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_TILING_FACTOR,
  RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_COUNT
};

enum renderable_3d_line_vertex_attribute_t
{
  RENDERABLE_3D_LINE_VERTEX_ATTRIBUTE_POSITION = 0x00,
  RENDERABLE_3D_LINE_VERTEX_ATTRIBUTE_COLOR,
  RENDERABLE_3D_LINE_VERTEX_ATTRIBUTE_COUNT
};

// Each kind of vertex, as its shader reads it. Positions and normals are in
//  world space, transformed as they're batched
struct renderable_3d_shaded_vertex_t
{
  union fvector3_t position;
  union fvector3_t normal;
  union fvector4_t color;
};

struct renderable_3d_textured_vertex_t
{
  union fvector3_t position;
  union fvector3_t normal;
  union fvector2_t texture_coordinate;
  float texture_index;
  float tiling_factor;
};

struct renderable_3d_line_vertex_t
{
  union fvector3_t position;
  union fvector4_t color;
};

/*******************************************************************************
 *
 *   PRIMITIVE DATA
 *
 ******************************************************************************/

// Each batch's GPU objects, with the vertices submitted to the batch so far
//  stored between vertex_data_base and vertex_data_ptr

// A batch of flat colored, lit copies of one mesh
struct renderable_3d_shaded_data_t
{
  struct vertex_array_t vao;
  struct vertex_buffer_t vbo;
  struct element_buffer_t ebo;
  //
  const struct renderable_3d_mesh_t* mesh;
  unsigned int index_count;
  unsigned int max_index_count;
  // Data pointers
  struct renderable_3d_shaded_vertex_t* vertex_data_base;
  struct renderable_3d_shaded_vertex_t* vertex_data_ptr;
};

// A batch of textured, lit copies of one mesh
struct renderable_3d_textured_data_t
{
  struct vertex_array_t vao;
  struct vertex_buffer_t vbo;
  struct element_buffer_t ebo;
  //
  const struct renderable_3d_mesh_t* mesh;
  unsigned int index_count;
  unsigned int max_index_count;
  // Data pointers
  struct renderable_3d_textured_vertex_t* vertex_data_base;
  struct renderable_3d_textured_vertex_t* vertex_data_ptr;
};

// A batch of lines, which are unlit
struct renderable_3d_line_data_t
{
  struct vertex_array_t vao;
  struct vertex_buffer_t vbo;
  //
  unsigned int line_vertex_count;
  unsigned int max_line_vertex_count;
  // Data pointers
  struct renderable_3d_line_vertex_t* vertex_data_base;
  struct renderable_3d_line_vertex_t* vertex_data_ptr;
};

/*******************************************************************************
 *
 *   INITIALIZATION AND CLEANUP
 *
 ******************************************************************************/

// Creates the vertex array and buffers for batches of up to in_max_count
//  copies of in_mesh, binds the shaded vertex layout to in_shader_program's
//  attributes, and keeps a batch's vertices in inout_vertex_storage, which
//  must hold in_mesh's vertex count for each copy and outlive the batch.
//  Indices are uploaded once, a chunk at a time. in_mesh must outlive the
//  batch
//
//  in:     in_mesh, in_max_count, in_shader_program
//  in/out: inout_vertex_storage
//  out:    out_shaded_data
void renderable_3d_shaded_data_init(
  struct renderable_3d_shaded_data_t* out_shaded_data,
  const struct renderable_3d_mesh_t* in_mesh,
  unsigned int in_max_count,
  struct renderable_3d_shaded_vertex_t* inout_vertex_storage,
  struct shader_program_t* in_shader_program);

// Creates the vertex array and buffers for batches of up to in_max_count
//  textured copies of in_mesh, binds the textured vertex layout to
//  in_shader_program's attributes, and keeps a batch's vertices in
//  inout_vertex_storage, which must hold in_mesh's vertex count for each copy
//  and outlive the batch. Indices are uploaded once, a chunk at a time.
//  in_mesh must outlive the batch
//
//  in:     in_mesh, in_max_count, in_shader_program
//  in/out: inout_vertex_storage
//  out:    out_textured_data
void renderable_3d_textured_data_init(
  struct renderable_3d_textured_data_t* out_textured_data,
  const struct renderable_3d_mesh_t* in_mesh,
  unsigned int in_max_count,
  struct renderable_3d_textured_vertex_t* inout_vertex_storage,
  struct shader_program_t* in_shader_program);

// Creates the vertex array and buffer for batches of up to in_max_count
//  lines, binds the line vertex layout to in_shader_program's attributes,
//  and keeps a batch's vertices in inout_vertex_storage, which must hold 2
//  for each line and outlive the batch. Lines are drawn without indices
//
//  in:     in_max_count, in_shader_program
//  in/out: inout_vertex_storage
//  out:    out_line_data
void renderable_3d_line_data_init(
  struct renderable_3d_line_data_t* out_line_data,
  unsigned int in_max_count,
  struct renderable_3d_line_vertex_t* inout_vertex_storage,
  struct shader_program_t* in_shader_program);

// Destroys the GPU objects made by renderable_3d_shaded_data_init()
//
//  in/out: inout_shaded_data
void renderable_3d_shaded_data_cleanup(
  struct renderable_3d_shaded_data_t* inout_shaded_data);

// Destroys the GPU objects made by renderable_3d_textured_data_init()
//
//  in/out: inout_textured_data
void renderable_3d_textured_data_cleanup(
  struct renderable_3d_textured_data_t* inout_textured_data);

// Destroys the GPU objects made by renderable_3d_line_data_init()
//
//  in/out: inout_line_data
void renderable_3d_line_data_cleanup(
  struct renderable_3d_line_data_t* inout_line_data);
