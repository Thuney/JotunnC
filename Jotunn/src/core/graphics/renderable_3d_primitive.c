#include <math.h>
#include <stdbool.h>
#include <stddef.h>

#include "renderable_3d_primitive.h"

#include "fmath.h"

// The components of the normals of the pyramid's sides, which rise 1 for
//  every 0.5 they run in: 1/sqrt(5) up and 2/sqrt(5) out
#define PYRAMID_3D_NORMAL_Y 0.4472136f
#define PYRAMID_3D_NORMAL_XZ 0.8944272f

// How many indices are built at a time when filling an element buffer
#define RENDERABLE_3D_INDEX_UPLOAD_CHUNK_COUNT 256

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Fills in sphere_3d_mesh's data, a UV sphere of radius 1, the first time
//  it's called; later calls do nothing
static void renderable_3d_generate_sphere_data(void);

// Creates a primitive's vertex array and buffers, and an index buffer when
//  asked to, then binds each attribute to the shader's attribute of the same
//  name. The attribute templates are left as they are. The vertex array is
//  left bound. out_ebo is untouched, and may be NULL, unless
//  in_should_use_ebo is set
//
//  in:     in_should_use_ebo, in_attributes, in_num_attributes,
//            in_shader_program
//  out:    out_vao, out_vbo, out_ebo
static void renderable_3d_primitive_data_init(
  struct vertex_array_t* out_vao,
  struct vertex_buffer_t* out_vbo,
  struct element_buffer_t* out_ebo,
  bool in_should_use_ebo,
  const struct vertex_attribute_t* in_attributes,
  unsigned int in_num_attributes,
  struct shader_program_t* in_shader_program);

// Fills the bound element buffer, which must be inout_ebo, with in_mesh's
//  indices repeated in_max_count times, each repeat offset to the vertices of
//  its copy of the mesh. They're built and uploaded a chunk at a time, so
//  however many there are, only a chunk's worth is held in memory
//
//  in:     in_mesh, in_max_count
//  in/out: inout_ebo
static void renderable_3d_upload_mesh_indices(
  struct element_buffer_t* inout_ebo,
  const struct renderable_3d_mesh_t* in_mesh,
  unsigned int in_max_count);

/*******************************************************************************
 *
 *   MESH DATA
 *
 ******************************************************************************/

static const union fvector3_t cube_3d_position_data[_NUM_CUBE_3D_VERTICES] =
  {
    // +X
    [0]  = {{ 0.5f, -0.5f,  0.5f}},
    [1]  = {{ 0.5f, -0.5f, -0.5f}},
    [2]  = {{ 0.5f,  0.5f, -0.5f}},
    [3]  = {{ 0.5f,  0.5f,  0.5f}},
    // -X
    [4]  = {{-0.5f, -0.5f, -0.5f}},
    [5]  = {{-0.5f, -0.5f,  0.5f}},
    [6]  = {{-0.5f,  0.5f,  0.5f}},
    [7]  = {{-0.5f,  0.5f, -0.5f}},
    // +Y
    [8]  = {{-0.5f,  0.5f,  0.5f}},
    [9]  = {{ 0.5f,  0.5f,  0.5f}},
    [10] = {{ 0.5f,  0.5f, -0.5f}},
    [11] = {{-0.5f,  0.5f, -0.5f}},
    // -Y
    [12] = {{-0.5f, -0.5f, -0.5f}},
    [13] = {{ 0.5f, -0.5f, -0.5f}},
    [14] = {{ 0.5f, -0.5f,  0.5f}},
    [15] = {{-0.5f, -0.5f,  0.5f}},
    // +Z
    [16] = {{-0.5f, -0.5f,  0.5f}},
    [17] = {{ 0.5f, -0.5f,  0.5f}},
    [18] = {{ 0.5f,  0.5f,  0.5f}},
    [19] = {{-0.5f,  0.5f,  0.5f}},
    // -Z
    [20] = {{ 0.5f, -0.5f, -0.5f}},
    [21] = {{-0.5f, -0.5f, -0.5f}},
    [22] = {{-0.5f,  0.5f, -0.5f}},
    [23] = {{ 0.5f,  0.5f, -0.5f}}
  };

static const union fvector3_t cube_3d_normal_data[_NUM_CUBE_3D_VERTICES] =
  {
    [0]  = {{ 1.0f,  0.0f,  0.0f}},
    [1]  = {{ 1.0f,  0.0f,  0.0f}},
    [2]  = {{ 1.0f,  0.0f,  0.0f}},
    [3]  = {{ 1.0f,  0.0f,  0.0f}},
    [4]  = {{-1.0f,  0.0f,  0.0f}},
    [5]  = {{-1.0f,  0.0f,  0.0f}},
    [6]  = {{-1.0f,  0.0f,  0.0f}},
    [7]  = {{-1.0f,  0.0f,  0.0f}},
    [8]  = {{ 0.0f,  1.0f,  0.0f}},
    [9]  = {{ 0.0f,  1.0f,  0.0f}},
    [10] = {{ 0.0f,  1.0f,  0.0f}},
    [11] = {{ 0.0f,  1.0f,  0.0f}},
    [12] = {{ 0.0f, -1.0f,  0.0f}},
    [13] = {{ 0.0f, -1.0f,  0.0f}},
    [14] = {{ 0.0f, -1.0f,  0.0f}},
    [15] = {{ 0.0f, -1.0f,  0.0f}},
    [16] = {{ 0.0f,  0.0f,  1.0f}},
    [17] = {{ 0.0f,  0.0f,  1.0f}},
    [18] = {{ 0.0f,  0.0f,  1.0f}},
    [19] = {{ 0.0f,  0.0f,  1.0f}},
    [20] = {{ 0.0f,  0.0f, -1.0f}},
    [21] = {{ 0.0f,  0.0f, -1.0f}},
    [22] = {{ 0.0f,  0.0f, -1.0f}},
    [23] = {{ 0.0f,  0.0f, -1.0f}}
  };

// Each face shows the whole texture, upright from outside
static const union fvector2_t cube_3d_texture_coordinate_data[_NUM_CUBE_3D_VERTICES] =
  {
    [0]  = {{0.0f, 0.0f}}, [1]  = {{1.0f, 0.0f}},
    [2]  = {{1.0f, 1.0f}}, [3]  = {{0.0f, 1.0f}},
    [4]  = {{0.0f, 0.0f}}, [5]  = {{1.0f, 0.0f}},
    [6]  = {{1.0f, 1.0f}}, [7]  = {{0.0f, 1.0f}},
    [8]  = {{0.0f, 0.0f}}, [9]  = {{1.0f, 0.0f}},
    [10] = {{1.0f, 1.0f}}, [11] = {{0.0f, 1.0f}},
    [12] = {{0.0f, 0.0f}}, [13] = {{1.0f, 0.0f}},
    [14] = {{1.0f, 1.0f}}, [15] = {{0.0f, 1.0f}},
    [16] = {{0.0f, 0.0f}}, [17] = {{1.0f, 0.0f}},
    [18] = {{1.0f, 1.0f}}, [19] = {{0.0f, 1.0f}},
    [20] = {{0.0f, 0.0f}}, [21] = {{1.0f, 0.0f}},
    [22] = {{1.0f, 1.0f}}, [23] = {{0.0f, 1.0f}}
  };

// Two triangles to a face, over its four vertices
static const unsigned int cube_3d_index_data[_NUM_CUBE_3D_INDICES] =
  {
     0,  1,  2,  0,  2,  3,
     4,  5,  6,  4,  6,  7,
     8,  9, 10,  8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23
  };

const struct renderable_3d_mesh_t cube_3d_mesh =
  {
    .positions           = cube_3d_position_data,
    .normals             = cube_3d_normal_data,
    .texture_coordinates = cube_3d_texture_coordinate_data,
    .vertex_count        = _NUM_CUBE_3D_VERTICES,
    .indices             = cube_3d_index_data,
    .index_count         = _NUM_CUBE_3D_INDICES
  };

const union fvector3_t cube_3d_edge_position_data[_NUM_CUBE_3D_EDGE_VERTICES] =
  {
    // Bottom
    {{-0.5f, -0.5f, -0.5f}}, {{ 0.5f, -0.5f, -0.5f}},
    {{ 0.5f, -0.5f, -0.5f}}, {{ 0.5f, -0.5f,  0.5f}},
    {{ 0.5f, -0.5f,  0.5f}}, {{-0.5f, -0.5f,  0.5f}},
    {{-0.5f, -0.5f,  0.5f}}, {{-0.5f, -0.5f, -0.5f}},
    // Top
    {{-0.5f,  0.5f, -0.5f}}, {{ 0.5f,  0.5f, -0.5f}},
    {{ 0.5f,  0.5f, -0.5f}}, {{ 0.5f,  0.5f,  0.5f}},
    {{ 0.5f,  0.5f,  0.5f}}, {{-0.5f,  0.5f,  0.5f}},
    {{-0.5f,  0.5f,  0.5f}}, {{-0.5f,  0.5f, -0.5f}},
    // Verticals
    {{-0.5f, -0.5f, -0.5f}}, {{-0.5f,  0.5f, -0.5f}},
    {{ 0.5f, -0.5f, -0.5f}}, {{ 0.5f,  0.5f, -0.5f}},
    {{ 0.5f, -0.5f,  0.5f}}, {{ 0.5f,  0.5f,  0.5f}},
    {{-0.5f, -0.5f,  0.5f}}, {{-0.5f,  0.5f,  0.5f}}
  };

// Square base on y = -0.5, apex at (0, 0.5, 0)
static const union fvector3_t pyramid_3d_position_data[_NUM_PYRAMID_3D_VERTICES] =
  {
    // +Z side
    [0]  = {{-0.5f, -0.5f,  0.5f}},
    [1]  = {{ 0.5f, -0.5f,  0.5f}},
    [2]  = {{ 0.0f,  0.5f,  0.0f}},
    // +X side
    [3]  = {{ 0.5f, -0.5f,  0.5f}},
    [4]  = {{ 0.5f, -0.5f, -0.5f}},
    [5]  = {{ 0.0f,  0.5f,  0.0f}},
    // -Z side
    [6]  = {{ 0.5f, -0.5f, -0.5f}},
    [7]  = {{-0.5f, -0.5f, -0.5f}},
    [8]  = {{ 0.0f,  0.5f,  0.0f}},
    // -X side
    [9]  = {{-0.5f, -0.5f, -0.5f}},
    [10] = {{-0.5f, -0.5f,  0.5f}},
    [11] = {{ 0.0f,  0.5f,  0.0f}},
    // Base
    [12] = {{-0.5f, -0.5f, -0.5f}},
    [13] = {{ 0.5f, -0.5f, -0.5f}},
    [14] = {{ 0.5f, -0.5f,  0.5f}},
    [15] = {{-0.5f, -0.5f,  0.5f}}
  };

static const union fvector3_t pyramid_3d_normal_data[_NUM_PYRAMID_3D_VERTICES] =
  {
    [0]  = {{ 0.0f, PYRAMID_3D_NORMAL_Y,  PYRAMID_3D_NORMAL_XZ}},
    [1]  = {{ 0.0f, PYRAMID_3D_NORMAL_Y,  PYRAMID_3D_NORMAL_XZ}},
    [2]  = {{ 0.0f, PYRAMID_3D_NORMAL_Y,  PYRAMID_3D_NORMAL_XZ}},
    [3]  = {{ PYRAMID_3D_NORMAL_XZ, PYRAMID_3D_NORMAL_Y, 0.0f}},
    [4]  = {{ PYRAMID_3D_NORMAL_XZ, PYRAMID_3D_NORMAL_Y, 0.0f}},
    [5]  = {{ PYRAMID_3D_NORMAL_XZ, PYRAMID_3D_NORMAL_Y, 0.0f}},
    [6]  = {{ 0.0f, PYRAMID_3D_NORMAL_Y, -PYRAMID_3D_NORMAL_XZ}},
    [7]  = {{ 0.0f, PYRAMID_3D_NORMAL_Y, -PYRAMID_3D_NORMAL_XZ}},
    [8]  = {{ 0.0f, PYRAMID_3D_NORMAL_Y, -PYRAMID_3D_NORMAL_XZ}},
    [9]  = {{-PYRAMID_3D_NORMAL_XZ, PYRAMID_3D_NORMAL_Y, 0.0f}},
    [10] = {{-PYRAMID_3D_NORMAL_XZ, PYRAMID_3D_NORMAL_Y, 0.0f}},
    [11] = {{-PYRAMID_3D_NORMAL_XZ, PYRAMID_3D_NORMAL_Y, 0.0f}},
    [12] = {{ 0.0f, -1.0f,  0.0f}},
    [13] = {{ 0.0f, -1.0f,  0.0f}},
    [14] = {{ 0.0f, -1.0f,  0.0f}},
    [15] = {{ 0.0f, -1.0f,  0.0f}}
  };

static const union fvector2_t pyramid_3d_texture_coordinate_data[_NUM_PYRAMID_3D_VERTICES] =
  {
    [0]  = {{0.0f, 0.0f}}, [1]  = {{1.0f, 0.0f}}, [2]  = {{0.5f, 1.0f}},
    [3]  = {{0.0f, 0.0f}}, [4]  = {{1.0f, 0.0f}}, [5]  = {{0.5f, 1.0f}},
    [6]  = {{0.0f, 0.0f}}, [7]  = {{1.0f, 0.0f}}, [8]  = {{0.5f, 1.0f}},
    [9]  = {{0.0f, 0.0f}}, [10] = {{1.0f, 0.0f}}, [11] = {{0.5f, 1.0f}},
    [12] = {{0.0f, 0.0f}}, [13] = {{1.0f, 0.0f}},
    [14] = {{1.0f, 1.0f}}, [15] = {{0.0f, 1.0f}}
  };

// A triangle to each side, and two to the base
static const unsigned int pyramid_3d_index_data[_NUM_PYRAMID_3D_INDICES] =
  {
     0,  1,  2,
     3,  4,  5,
     6,  7,  8,
     9, 10, 11,
    12, 13, 14, 12, 14, 15
  };

const struct renderable_3d_mesh_t pyramid_3d_mesh =
  {
    .positions           = pyramid_3d_position_data,
    .normals             = pyramid_3d_normal_data,
    .texture_coordinates = pyramid_3d_texture_coordinate_data,
    .vertex_count        = _NUM_PYRAMID_3D_VERTICES,
    .indices             = pyramid_3d_index_data,
    .index_count         = _NUM_PYRAMID_3D_INDICES
  };

// Filled in by renderable_3d_generate_sphere_data()
static union fvector3_t sphere_3d_position_data[_NUM_SPHERE_3D_VERTICES];
static union fvector3_t sphere_3d_normal_data[_NUM_SPHERE_3D_VERTICES];
static union fvector2_t sphere_3d_texture_coordinate_data[_NUM_SPHERE_3D_VERTICES];
static unsigned int sphere_3d_index_data[_NUM_SPHERE_3D_INDICES];

const struct renderable_3d_mesh_t sphere_3d_mesh =
  {
    .positions           = sphere_3d_position_data,
    .normals             = sphere_3d_normal_data,
    .texture_coordinates = sphere_3d_texture_coordinate_data,
    .vertex_count        = _NUM_SPHERE_3D_VERTICES,
    .indices             = sphere_3d_index_data,
    .index_count         = _NUM_SPHERE_3D_INDICES
  };

/*******************************************************************************
 *
 *   VERTEX ATTRIBUTES
 *
 ******************************************************************************/

// Each kind of vertex's attributes. The shader index is looked up for each
//  shader program as a batch is made
static const struct vertex_attribute_t
  shaded_vertex_attributes[RENDERABLE_3D_SHADED_VERTEX_ATTRIBUTE_COUNT] =
{
  [RENDERABLE_3D_SHADED_VERTEX_ATTRIBUTE_POSITION] =
  {
    .attribute_name = "position",
    .index = (unsigned int)-1,
    .size = 3,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_shaded_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_shaded_vertex_t,
        position)
  },
  [RENDERABLE_3D_SHADED_VERTEX_ATTRIBUTE_NORMAL] =
  {
    .attribute_name = "normal",
    .index = (unsigned int)-1,
    .size = 3,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_shaded_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_shaded_vertex_t,
        normal)
  },
  [RENDERABLE_3D_SHADED_VERTEX_ATTRIBUTE_COLOR] =
  {
    .attribute_name = "color",
    .index = (unsigned int)-1,
    .size = 4,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_shaded_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_shaded_vertex_t,
        color)
  }
};

static const struct vertex_attribute_t
  textured_vertex_attributes[RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_COUNT] =
{
  [RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_POSITION] =
  {
    .attribute_name = "position",
    .index = (unsigned int)-1,
    .size = 3,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_textured_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_textured_vertex_t,
        position)
  },
  [RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_NORMAL] =
  {
    .attribute_name = "normal",
    .index = (unsigned int)-1,
    .size = 3,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_textured_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_textured_vertex_t,
        normal)
  },
  [RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_TEXTURE_COORDINATES] =
  {
    .attribute_name = "texCoord",
    .index = (unsigned int)-1,
    .size = 2,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_textured_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_textured_vertex_t,
        texture_coordinate)
  },
  [RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_TEXTURE_INDEX] =
  {
    .attribute_name = "texIndex",
    .index = (unsigned int)-1,
    .size = 1,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_textured_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_textured_vertex_t,
        texture_index)
  },
  [RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_TILING_FACTOR] =
  {
    .attribute_name = "tilingFactor",
    .index = (unsigned int)-1,
    .size = 1,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_textured_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_textured_vertex_t,
        tiling_factor)
  }
};

static const struct vertex_attribute_t
  line_vertex_attributes[RENDERABLE_3D_LINE_VERTEX_ATTRIBUTE_COUNT] =
{
  [RENDERABLE_3D_LINE_VERTEX_ATTRIBUTE_POSITION] =
  {
    .attribute_name = "position",
    .index = (unsigned int)-1,
    .size = 3,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_line_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_line_vertex_t,
        position)
  },
  [RENDERABLE_3D_LINE_VERTEX_ATTRIBUTE_COLOR] =
  {
    .attribute_name = "color",
    .index = (unsigned int)-1,
    .size = 4,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_line_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_line_vertex_t,
        color)
  }
};

/*******************************************************************************
 *
 *   Function: renderable_3d_shaded_data_init()
 *
 ******************************************************************************/
void renderable_3d_shaded_data_init(
  struct renderable_3d_shaded_data_t* out_shaded_data,
  const struct renderable_3d_mesh_t* in_mesh,
  unsigned int in_max_count,
  struct renderable_3d_shaded_vertex_t* inout_vertex_storage,
  struct shader_program_t* in_shader_program)
{
  const unsigned int max_vertices = (in_mesh->vertex_count * in_max_count);

  renderable_3d_generate_sphere_data();

  renderable_3d_primitive_data_init(
    &(out_shaded_data->vao),
    &(out_shaded_data->vbo),
    &(out_shaded_data->ebo),
    true,
    shaded_vertex_attributes,
    RENDERABLE_3D_SHADED_VERTEX_ATTRIBUTE_COUNT,
    in_shader_program);

  out_shaded_data->mesh = in_mesh;
  out_shaded_data->index_count = 0;
  out_shaded_data->max_index_count = (in_mesh->index_count * in_max_count);

  out_shaded_data->vertex_data_base = inout_vertex_storage;
  out_shaded_data->vertex_data_ptr = out_shaded_data->vertex_data_base;

  renderable_3d_upload_mesh_indices(
    &(out_shaded_data->ebo),
    in_mesh,
    in_max_count);
  vertex_buffer_set_data(
    &(out_shaded_data->vbo),
    NULL,
    (max_vertices * sizeof(struct renderable_3d_shaded_vertex_t)),
    VERTEX_BUFFER_DATA_USAGE_DYNAMIC_DRAW);

  vertex_array_unbind();
}

/*******************************************************************************
 *
 *   Function: renderable_3d_textured_data_init()
 *
 ******************************************************************************/
void renderable_3d_textured_data_init(
  struct renderable_3d_textured_data_t* out_textured_data,
  const struct renderable_3d_mesh_t* in_mesh,
  unsigned int in_max_count,
  struct renderable_3d_textured_vertex_t* inout_vertex_storage,
  struct shader_program_t* in_shader_program)
{
  const unsigned int max_vertices = (in_mesh->vertex_count * in_max_count);

  renderable_3d_generate_sphere_data();

  renderable_3d_primitive_data_init(
    &(out_textured_data->vao),
    &(out_textured_data->vbo),
    &(out_textured_data->ebo),
    true,
    textured_vertex_attributes,
    RENDERABLE_3D_TEXTURED_VERTEX_ATTRIBUTE_COUNT,
    in_shader_program);

  out_textured_data->mesh = in_mesh;
  out_textured_data->index_count = 0;
  out_textured_data->max_index_count = (in_mesh->index_count * in_max_count);

  out_textured_data->vertex_data_base = inout_vertex_storage;
  out_textured_data->vertex_data_ptr = out_textured_data->vertex_data_base;

  renderable_3d_upload_mesh_indices(
    &(out_textured_data->ebo),
    in_mesh,
    in_max_count);
  vertex_buffer_set_data(
    &(out_textured_data->vbo),
    NULL,
    (max_vertices * sizeof(struct renderable_3d_textured_vertex_t)),
    VERTEX_BUFFER_DATA_USAGE_DYNAMIC_DRAW);

  vertex_array_unbind();
}

/*******************************************************************************
 *
 *   Function: renderable_3d_line_data_init()
 *
 ******************************************************************************/
void renderable_3d_line_data_init(
  struct renderable_3d_line_data_t* out_line_data,
  unsigned int in_max_count,
  struct renderable_3d_line_vertex_t* inout_vertex_storage,
  struct shader_program_t* in_shader_program)
{
  const unsigned int max_vertices = (2 * in_max_count);

  renderable_3d_primitive_data_init(
    &(out_line_data->vao),
    &(out_line_data->vbo),
    NULL,
    false,
    line_vertex_attributes,
    RENDERABLE_3D_LINE_VERTEX_ATTRIBUTE_COUNT,
    in_shader_program);

  out_line_data->line_vertex_count = 0;
  out_line_data->max_line_vertex_count = max_vertices;

  out_line_data->vertex_data_base = inout_vertex_storage;
  out_line_data->vertex_data_ptr = out_line_data->vertex_data_base;

  vertex_buffer_set_data(
    &(out_line_data->vbo),
    NULL,
    (max_vertices * sizeof(struct renderable_3d_line_vertex_t)),
    VERTEX_BUFFER_DATA_USAGE_DYNAMIC_DRAW);

  vertex_array_unbind();
}

/*******************************************************************************
 *
 *   Function: renderable_3d_shaded_data_cleanup()
 *
 ******************************************************************************/
void renderable_3d_shaded_data_cleanup(
  struct renderable_3d_shaded_data_t* inout_shaded_data)
{
  vertex_array_destroy(
    &(inout_shaded_data->vao),
    1);
  vertex_buffer_destroy(
    &(inout_shaded_data->vbo),
    1);
  element_buffer_destroy(
    &(inout_shaded_data->ebo),
    1);

  inout_shaded_data->vertex_data_base = NULL;
  inout_shaded_data->vertex_data_ptr = NULL;
}

/*******************************************************************************
 *
 *   Function: renderable_3d_textured_data_cleanup()
 *
 ******************************************************************************/
void renderable_3d_textured_data_cleanup(
  struct renderable_3d_textured_data_t* inout_textured_data)
{
  vertex_array_destroy(
    &(inout_textured_data->vao),
    1);
  vertex_buffer_destroy(
    &(inout_textured_data->vbo),
    1);
  element_buffer_destroy(
    &(inout_textured_data->ebo),
    1);

  inout_textured_data->vertex_data_base = NULL;
  inout_textured_data->vertex_data_ptr = NULL;
}

/*******************************************************************************
 *
 *   Function: renderable_3d_line_data_cleanup()
 *
 ******************************************************************************/
void renderable_3d_line_data_cleanup(
  struct renderable_3d_line_data_t* inout_line_data)
{
  vertex_array_destroy(
    &(inout_line_data->vao),
    1);
  vertex_buffer_destroy(
    &(inout_line_data->vbo),
    1);

  inout_line_data->vertex_data_base = NULL;
  inout_line_data->vertex_data_ptr = NULL;
}

/*******************************************************************************
 *
 *   Function: renderable_3d_generate_sphere_data()
 *
 ******************************************************************************/
static void renderable_3d_generate_sphere_data(void)
{
  static bool is_generated = false;

  if (is_generated) {
    return;
  }

  const unsigned int slices = _RENDERABLE_3D_SPHERE_SLICES;
  const unsigned int stacks = _RENDERABLE_3D_SPHERE_STACKS;

  unsigned int stack, slice;

  // Rings of vertices from the north pole (+Y) down to the south pole (-Y),
  //  each with a repeat of its first vertex to close the texture's seam
  for (stack = 0; stack <= stacks; stack++) {
    const float polar_angle_radians = (FMATH_PI * ((float)stack / (float)stacks));

    for (slice = 0; slice <= slices; slice++) {
      const float azimuth_radians = (2.0f * FMATH_PI * ((float)slice / (float)slices));

      const unsigned int v = ((stack * (slices + 1)) + slice);

      // On the unit sphere, each vertex's position is also its normal
      const union fvector3_t position =
        (union fvector3_t){{(sinf(polar_angle_radians) * cosf(azimuth_radians)),
                            cosf(polar_angle_radians),
                            (sinf(polar_angle_radians) * sinf(azimuth_radians))}};

      sphere_3d_position_data[v] = position;
      sphere_3d_normal_data[v] = position;
      sphere_3d_texture_coordinate_data[v] =
        (union fvector2_t){{((float)slice / (float)slices),
                            (1.0f - ((float)stack / (float)stacks))}};
    }
  }

  // Two triangles to each patch between neighbouring rings and slices, wound
  //  counter-clockwise from outside
  unsigned int i = 0;

  for (stack = 0; stack < stacks; stack++) {
    for (slice = 0; slice < slices; slice++) {
      const unsigned int top = ((stack * (slices + 1)) + slice);
      const unsigned int bottom = (top + (slices + 1));

      sphere_3d_index_data[i++] = top;
      sphere_3d_index_data[i++] = (top + 1);
      sphere_3d_index_data[i++] = bottom;

      sphere_3d_index_data[i++] = (top + 1);
      sphere_3d_index_data[i++] = (bottom + 1);
      sphere_3d_index_data[i++] = bottom;
    }
  }

  is_generated = true;
}

/*******************************************************************************
 *
 *   Function: renderable_3d_primitive_data_init()
 *
 ******************************************************************************/
static void renderable_3d_primitive_data_init(
  struct vertex_array_t* out_vao,
  struct vertex_buffer_t* out_vbo,
  struct element_buffer_t* out_ebo,
  bool in_should_use_ebo,
  const struct vertex_attribute_t* in_attributes,
  unsigned int in_num_attributes,
  struct shader_program_t* in_shader_program)
{
  vertex_array_init(
    out_vao,
    1);
  vertex_buffer_init(
    out_vbo,
    1);

  if (in_should_use_ebo) {
    element_buffer_init(
      out_ebo,
      1);
  }

  vertex_array_bind(out_vao);
  vertex_buffer_bind(out_vbo);

  if (in_should_use_ebo) {
    element_buffer_bind(out_ebo);
  }

  for (unsigned int i = 0; i < in_num_attributes; i++) {
    // A copy, so that the templates are shared between shader programs
    struct vertex_attribute_t attribute = in_attributes[i];

    attribute.index =
      shader_program_get_attribute_location(
        in_shader_program,
        attribute.attribute_name);

    vertex_array_set_attribute(
      out_vao,
      &(attribute));
  }
}

/*******************************************************************************
 *
 *   Function: renderable_3d_upload_mesh_indices()
 *
 ******************************************************************************/
static void renderable_3d_upload_mesh_indices(
  struct element_buffer_t* inout_ebo,
  const struct renderable_3d_mesh_t* in_mesh,
  unsigned int in_max_count)
{
  const unsigned int index_count = (in_mesh->index_count * in_max_count);

  // Sized for every index, then filled in a chunk at a time
  element_buffer_buffer_data(
    inout_ebo,
    NULL,
    (index_count * sizeof(unsigned int)),
    VERTEX_BUFFER_DATA_USAGE_STATIC_DRAW);

  unsigned int chunk[RENDERABLE_3D_INDEX_UPLOAD_CHUNK_COUNT];

  unsigned int start = 0;
  while (start < index_count) {
    const unsigned int remaining_count = (index_count - start);
    const unsigned int chunk_count =
      (remaining_count < RENDERABLE_3D_INDEX_UPLOAD_CHUNK_COUNT) ?
        remaining_count :
        RENDERABLE_3D_INDEX_UPLOAD_CHUNK_COUNT;

    for (unsigned int i = 0; i < chunk_count; i++) {
      const unsigned int position = (start + i);
      // Which copy of the mesh the position falls in, offsetting the mesh's
      //  index to that copy's vertices
      const unsigned int copy = (position / in_mesh->index_count);

      chunk[i] =
        (in_mesh->indices[position % in_mesh->index_count] +
         (copy * in_mesh->vertex_count));
    }

    element_buffer_set_sub_data(
      inout_ebo,
      (start * sizeof(unsigned int)),
      chunk,
      (chunk_count * sizeof(unsigned int)));

    start += chunk_count;
  }
}
