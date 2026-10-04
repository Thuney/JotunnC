#pragma once

#include "fvector.h"
#include "shader.h"
#include "vertexarray.h"
#include "vertexbuffer.h"

// Mesh data

// Number of longitudinal slices / latitudinal stacks approximating a sphere
#define _RENDERABLE_3D_SPHERE_SLICES (24)
#define _RENDERABLE_3D_SPHERE_STACKS (16)
#define _NUM_SPHERE_3D_VERTICES \
  ((_RENDERABLE_3D_SPHERE_SLICES + 1) * (_RENDERABLE_3D_SPHERE_STACKS + 1))
#define _NUM_SPHERE_3D_INDICES \
  (6 * _RENDERABLE_3D_SPHERE_SLICES * _RENDERABLE_3D_SPHERE_STACKS)

#define _NUM_CUBE_3D_VERTICES (24)
#define _NUM_CUBE_3D_INDICES (36)

#define _NUM_PYRAMID_3D_VERTICES (16)
#define _NUM_PYRAMID_3D_INDICES (18)

// Describes a single, untransformed mesh that the batches repeat
//  - All meshes are centered on the origin and fit within a unit cube
//    (the sphere has a radius of 1, like the 2D circle)
struct renderable_3d_mesh_t
{
  const fvector3* positions;
  const fvector3* normals;
  const fvector2* texture_coordinates;
  unsigned int num_vertices;
  //
  const unsigned int* indices;
  unsigned int num_indices;
};

extern const struct renderable_3d_mesh_t cube_3d_mesh;
extern const struct renderable_3d_mesh_t pyramid_3d_mesh;
extern const struct renderable_3d_mesh_t sphere_3d_mesh;

// Edges of the cube mesh, as pairs of corner positions, for wireframe drawing
extern const fvector3 cube_3d_edge_data[24];

enum renderable_3d_shaded_vertex_attribute_t
{
  SHADED_3D_POSITION = 0x00,
  SHADED_3D_NORMAL,
  SHADED_3D_COLOR,
  _SHADED_3D_ATTRIBUTE_NUM
};

enum renderable_3d_textured_vertex_attribute_t
{
  TEXTURED_3D_POSITION = 0x00,
  TEXTURED_3D_NORMAL,
  TEXTURED_3D_TEXTURE_COORDINATES,
  TEXTURED_3D_TEXTURE_INDEX,
  TEXTURED_3D_TILING_FACTOR,
  _TEXTURED_3D_ATTRIBUTE_NUM
};

enum renderable_3d_line_vertex_attribute_t
{
  LINE_3D_POSITION = 0x00,
  LINE_3D_COLOR,
  _LINE_3D_ATTRIBUTE_NUM
};

//

struct renderable_3d_shaded_vertex_t
{
  fvector3 position;
  fvector3 normal;
  fvector4 color;
};

struct renderable_3d_textured_vertex_t
{
  fvector3 position;
  fvector3 normal;
  fvector2 texture_coordinate;
  float texture_index;
  float tiling_factor;
};

struct renderable_3d_line_vertex_t
{
  fvector3 position;
  fvector4 color;
};

//

// A batch of flat colored, lit copies of a single mesh
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

// A batch of textured, lit copies of a single mesh
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

//

void renderable_3d_generate_mesh_data(void);

void renderable_3d_shaded_data_init(
  struct renderable_3d_shaded_data_t* shaded_data,
  const struct renderable_3d_mesh_t* mesh,
  const unsigned int max_count,
  struct shader_program_t* shader_program);
void renderable_3d_textured_data_init(
  struct renderable_3d_textured_data_t* textured_data,
  const struct renderable_3d_mesh_t* mesh,
  const unsigned int max_count,
  struct shader_program_t* shader_program);
void renderable_3d_line_data_init(
  struct renderable_3d_line_data_t* line_data,
  const unsigned int max_count,
  struct shader_program_t* shader_program);

void renderable_3d_shaded_data_cleanup(
  struct renderable_3d_shaded_data_t* shaded_data);
void renderable_3d_textured_data_cleanup(
  struct renderable_3d_textured_data_t* textured_data);
void renderable_3d_line_data_cleanup(
  struct renderable_3d_line_data_t* line_data);
