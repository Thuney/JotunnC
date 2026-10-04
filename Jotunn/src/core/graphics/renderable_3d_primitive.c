#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#include "renderable_3d_primitive.h"

#define PI 3.14159265f

// Normal components of the slanted pyramid faces (1/sqrt(5) and 2/sqrt(5))
#define PYRAMID_NORMAL_Y 0.4472136f
#define PYRAMID_NORMAL_XZ 0.8944272f

// Cube - 4 vertices per face so each face gets its own normal; each face is
// wound counter-clockwise when viewed from outside the cube
static const fvector3 cube_3d_position_data[_NUM_CUBE_3D_VERTICES] =
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

static const fvector3 cube_3d_normal_data[_NUM_CUBE_3D_VERTICES] =
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

static const fvector2 cube_3d_texture_coordinates[_NUM_CUBE_3D_VERTICES] =
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

static const unsigned int cube_3d_index_data[_NUM_CUBE_3D_INDICES] =
  {
     0,  1,  2,  0,  2,  3,
     4,  5,  6,  4,  6,  7,
     8,  9, 10,  8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23
  };

const fvector3 cube_3d_edge_data[24] =
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

const struct renderable_3d_mesh_t cube_3d_mesh =
  {
    .positions           = cube_3d_position_data,
    .normals             = cube_3d_normal_data,
    .texture_coordinates = cube_3d_texture_coordinates,
    .num_vertices        = _NUM_CUBE_3D_VERTICES,
    .indices             = cube_3d_index_data,
    .num_indices         = _NUM_CUBE_3D_INDICES
  };

// Pyramid - square base on y = -0.5, apex at y = 0.5
static const fvector3 pyramid_3d_position_data[_NUM_PYRAMID_3D_VERTICES] =
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

static const fvector3 pyramid_3d_normal_data[_NUM_PYRAMID_3D_VERTICES] =
  {
    [0]  = {{ 0.0f, PYRAMID_NORMAL_Y,  PYRAMID_NORMAL_XZ}},
    [1]  = {{ 0.0f, PYRAMID_NORMAL_Y,  PYRAMID_NORMAL_XZ}},
    [2]  = {{ 0.0f, PYRAMID_NORMAL_Y,  PYRAMID_NORMAL_XZ}},
    [3]  = {{ PYRAMID_NORMAL_XZ, PYRAMID_NORMAL_Y, 0.0f}},
    [4]  = {{ PYRAMID_NORMAL_XZ, PYRAMID_NORMAL_Y, 0.0f}},
    [5]  = {{ PYRAMID_NORMAL_XZ, PYRAMID_NORMAL_Y, 0.0f}},
    [6]  = {{ 0.0f, PYRAMID_NORMAL_Y, -PYRAMID_NORMAL_XZ}},
    [7]  = {{ 0.0f, PYRAMID_NORMAL_Y, -PYRAMID_NORMAL_XZ}},
    [8]  = {{ 0.0f, PYRAMID_NORMAL_Y, -PYRAMID_NORMAL_XZ}},
    [9]  = {{-PYRAMID_NORMAL_XZ, PYRAMID_NORMAL_Y, 0.0f}},
    [10] = {{-PYRAMID_NORMAL_XZ, PYRAMID_NORMAL_Y, 0.0f}},
    [11] = {{-PYRAMID_NORMAL_XZ, PYRAMID_NORMAL_Y, 0.0f}},
    [12] = {{ 0.0f, -1.0f, 0.0f}},
    [13] = {{ 0.0f, -1.0f, 0.0f}},
    [14] = {{ 0.0f, -1.0f, 0.0f}},
    [15] = {{ 0.0f, -1.0f, 0.0f}}
  };

static const fvector2 pyramid_3d_texture_coordinates[_NUM_PYRAMID_3D_VERTICES] =
  {
    [0]  = {{0.0f, 0.0f}}, [1]  = {{1.0f, 0.0f}}, [2]  = {{0.5f, 1.0f}},
    [3]  = {{0.0f, 0.0f}}, [4]  = {{1.0f, 0.0f}}, [5]  = {{0.5f, 1.0f}},
    [6]  = {{0.0f, 0.0f}}, [7]  = {{1.0f, 0.0f}}, [8]  = {{0.5f, 1.0f}},
    [9]  = {{0.0f, 0.0f}}, [10] = {{1.0f, 0.0f}}, [11] = {{0.5f, 1.0f}},
    [12] = {{0.0f, 0.0f}}, [13] = {{1.0f, 0.0f}},
    [14] = {{1.0f, 1.0f}}, [15] = {{0.0f, 1.0f}}
  };

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
    .texture_coordinates = pyramid_3d_texture_coordinates,
    .num_vertices        = _NUM_PYRAMID_3D_VERTICES,
    .indices             = pyramid_3d_index_data,
    .num_indices         = _NUM_PYRAMID_3D_INDICES
  };

// Sphere - UV sphere of radius 1, generated by renderable_3d_generate_mesh_data
static fvector3 sphere_3d_position_data[_NUM_SPHERE_3D_VERTICES];
static fvector3 sphere_3d_normal_data[_NUM_SPHERE_3D_VERTICES];
static fvector2 sphere_3d_texture_coordinates[_NUM_SPHERE_3D_VERTICES];
static unsigned int sphere_3d_index_data[_NUM_SPHERE_3D_INDICES];

const struct renderable_3d_mesh_t sphere_3d_mesh =
  {
    .positions           = sphere_3d_position_data,
    .normals             = sphere_3d_normal_data,
    .texture_coordinates = sphere_3d_texture_coordinates,
    .num_vertices        = _NUM_SPHERE_3D_VERTICES,
    .indices             = sphere_3d_index_data,
    .num_indices         = _NUM_SPHERE_3D_INDICES
  };

/*******************************************************************************
 *
 * Function: gen_sphere_3d_data()
 *
 ******************************************************************************/
static void gen_sphere_3d_data(void)
{
  const unsigned int slices = _RENDERABLE_3D_SPHERE_SLICES;
  const unsigned int stacks = _RENDERABLE_3D_SPHERE_STACKS;

  unsigned int stack, slice;

  // Vertices, from the north pole (+Y) down to the south pole (-Y)
  for (stack = 0; stack <= stacks; stack++) {
    const float phi = PI * ((float)stack / (float)stacks);

    for (slice = 0; slice <= slices; slice++) {
      const float theta = 2.0f * PI * ((float)slice / (float)slices);

      const unsigned int v = (stack * (slices + 1)) + slice;

      const fvector3 position =
        (fvector3){{sinf(phi) * cosf(theta),
                    cosf(phi),
                    sinf(phi) * sinf(theta)}};

      sphere_3d_position_data[v] = position;
      sphere_3d_normal_data[v] = position;
      sphere_3d_texture_coordinates[v] =
        (fvector2){{(float)slice / (float)slices,
                    1.0f - ((float)stack / (float)stacks)}};
    }
  }

  // Two counter-clockwise triangles per stack / slice patch
  unsigned int i = 0;

  for (stack = 0; stack < stacks; stack++) {
    for (slice = 0; slice < slices; slice++) {
      const unsigned int top = (stack * (slices + 1)) + slice;
      const unsigned int bottom = top + (slices + 1);

      sphere_3d_index_data[i++] = top;
      sphere_3d_index_data[i++] = top + 1;
      sphere_3d_index_data[i++] = bottom;

      sphere_3d_index_data[i++] = top + 1;
      sphere_3d_index_data[i++] = bottom + 1;
      sphere_3d_index_data[i++] = bottom;
    }
  }
}

// Vertex attributes - the shader index is filled in per shader program
static const struct vertex_attribute_t
  shaded_vertex_attributes[_SHADED_3D_ATTRIBUTE_NUM] =
{
  [SHADED_3D_POSITION] =
  {
    .attribute_name = "position",
    .index = (unsigned int)-1,
    .size = 3,
    .data_type = V_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_shaded_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_shaded_vertex_t,
        position)
  },
  [SHADED_3D_NORMAL] =
  {
    .attribute_name = "normal",
    .index = (unsigned int)-1,
    .size = 3,
    .data_type = V_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_shaded_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_shaded_vertex_t,
        normal)
  },
  [SHADED_3D_COLOR] =
  {
    .attribute_name = "color",
    .index = (unsigned int)-1,
    .size = 4,
    .data_type = V_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_shaded_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_shaded_vertex_t,
        color)
  }
};

static const struct vertex_attribute_t
  textured_vertex_attributes[_TEXTURED_3D_ATTRIBUTE_NUM] =
{
  [TEXTURED_3D_POSITION] =
  {
    .attribute_name = "position",
    .index = (unsigned int)-1,
    .size = 3,
    .data_type = V_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_textured_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_textured_vertex_t,
        position)
  },
  [TEXTURED_3D_NORMAL] =
  {
    .attribute_name = "normal",
    .index = (unsigned int)-1,
    .size = 3,
    .data_type = V_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_textured_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_textured_vertex_t,
        normal)
  },
  [TEXTURED_3D_TEXTURE_COORDINATES] =
  {
    .attribute_name = "texCoord",
    .index = (unsigned int)-1,
    .size = 2,
    .data_type = V_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_textured_vertex_t),
    .ptr_offset_to_attrib = (void*)offsetof(
      struct renderable_3d_textured_vertex_t,
      texture_coordinate)
  },
  [TEXTURED_3D_TEXTURE_INDEX] =
  {
    .attribute_name = "texIndex",
    .index = (unsigned int)-1,
    .size = 1,
    .data_type = V_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_textured_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_textured_vertex_t,
        texture_index)
  },
  [TEXTURED_3D_TILING_FACTOR] =
  {
    .attribute_name = "tilingFactor",
    .index = (unsigned int)-1,
    .size = 1,
    .data_type = V_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_textured_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_textured_vertex_t,
        tiling_factor)
  }
};

static const struct vertex_attribute_t
  line_vertex_attributes[_LINE_3D_ATTRIBUTE_NUM] =
{
  [LINE_3D_POSITION] =
  {
    .attribute_name = "position",
    .index = (unsigned int)-1,
    .size = 3,
    .data_type = V_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_3d_line_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_3d_line_vertex_t,
        position)
  },
  [LINE_3D_COLOR] =
  {
    .attribute_name = "color",
    .index = (unsigned int)-1,
    .size = 4,
    .data_type = V_FLOAT,
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
 * Function: renderable_3d_primitive_data_init()
 *
 ******************************************************************************/
static void renderable_3d_primitive_data_init(
  struct vertex_array_t* vao,
  struct vertex_buffer_t* vbo,
  struct element_buffer_t* ebo,
  const unsigned int use_ebo,
  const struct vertex_attribute_t* attributes,
  const unsigned int num_attributes,
  struct shader_program_t* shader_program)
{
  vertex_array_init(
    vao,
    1);
  vertex_buffer_init(
    vbo,
    1);

  if (use_ebo) {
    element_buffer_init(
      ebo,
      1);
  }

  vertex_array_bind(vao);
  vertex_buffer_bind(vbo);

  if (use_ebo) {
    element_buffer_bind(ebo);
  }

  for (unsigned int i = 0; i < num_attributes; i++) {
    // Work on a copy so the shared attribute templates are left untouched
    struct vertex_attribute_t attribute = attributes[i];

    attribute.index =
      shader_program_get_attribute_location(
        shader_program,
        attribute.attribute_name);

    vertex_array_set_attribute(
      vao,
      &attribute);
  }
}

/*******************************************************************************
 *
 * Function: renderable_3d_buffer_mesh_indices()
 *
 ******************************************************************************/
static void renderable_3d_buffer_mesh_indices(
  struct element_buffer_t* ebo,
  const struct renderable_3d_mesh_t* mesh,
  const unsigned int max_count)
{
  // Repeat the mesh's indices once per batched copy, offset to that copy's
  //  vertices
  const unsigned int max_indices = mesh->num_indices * max_count;

  unsigned int* indices =
    (unsigned int*)malloc(max_indices * sizeof(unsigned int));

  unsigned int copy, i;

  for (copy = 0; copy < max_count; copy++) {
    for (i = 0; i < mesh->num_indices; i++) {
      indices[(copy * mesh->num_indices) + i] =
        mesh->indices[i] + (copy * mesh->num_vertices);
    }
  }

  element_buffer_buffer_data(
    ebo,
    indices,
    max_indices * sizeof(unsigned int),
    STATIC_DRAW);

  free(indices);
}

/*******************************************************************************
 *
 * Function: renderable_3d_generate_mesh_data()
 *
 ******************************************************************************/
void renderable_3d_generate_mesh_data(void)
{
  static unsigned int is_generated = 0;

  if (!is_generated) {
    gen_sphere_3d_data();

    is_generated = 1;
  }
}

/*******************************************************************************
 *
 * Function: renderable_3d_shaded_data_init()
 *
 ******************************************************************************/
void renderable_3d_shaded_data_init(
  struct renderable_3d_shaded_data_t* shaded_data,
  const struct renderable_3d_mesh_t* mesh,
  const unsigned int max_count,
  struct shader_program_t* shader_program)
{
  const unsigned int max_vertices = mesh->num_vertices * max_count;

  renderable_3d_primitive_data_init(
    &(shaded_data->vao),
    &(shaded_data->vbo),
    &(shaded_data->ebo),
    1,
    shaded_vertex_attributes,
    _SHADED_3D_ATTRIBUTE_NUM,
    shader_program);

  shaded_data->mesh = mesh;
  shaded_data->index_count = 0;
  shaded_data->max_index_count = mesh->num_indices * max_count;

  shaded_data->vertex_data_base =
    (struct renderable_3d_shaded_vertex_t*)calloc(
      max_vertices,
      sizeof(struct renderable_3d_shaded_vertex_t));
  shaded_data->vertex_data_ptr = shaded_data->vertex_data_base;

  renderable_3d_buffer_mesh_indices(
    &(shaded_data->ebo),
    mesh,
    max_count);
  vertex_buffer_buffer_data(
    &(shaded_data->vbo),
    (float*)shaded_data->vertex_data_base,
    max_vertices * sizeof(struct renderable_3d_shaded_vertex_t),
    DYNAMIC_DRAW);

  vertex_array_unbind();
}

/*******************************************************************************
 *
 * Function: renderable_3d_textured_data_init()
 *
 ******************************************************************************/
void renderable_3d_textured_data_init(
  struct renderable_3d_textured_data_t* textured_data,
  const struct renderable_3d_mesh_t* mesh,
  const unsigned int max_count,
  struct shader_program_t* shader_program)
{
  const unsigned int max_vertices = mesh->num_vertices * max_count;

  renderable_3d_primitive_data_init(
    &(textured_data->vao),
    &(textured_data->vbo),
    &(textured_data->ebo),
    1,
    textured_vertex_attributes,
    _TEXTURED_3D_ATTRIBUTE_NUM,
    shader_program);

  textured_data->mesh = mesh;
  textured_data->index_count = 0;
  textured_data->max_index_count = mesh->num_indices * max_count;

  textured_data->vertex_data_base =
    (struct renderable_3d_textured_vertex_t*)calloc(
      max_vertices,
      sizeof(struct renderable_3d_textured_vertex_t));
  textured_data->vertex_data_ptr = textured_data->vertex_data_base;

  renderable_3d_buffer_mesh_indices(
    &(textured_data->ebo),
    mesh,
    max_count);
  vertex_buffer_buffer_data(
    &(textured_data->vbo),
    (float*)textured_data->vertex_data_base,
    max_vertices * sizeof(struct renderable_3d_textured_vertex_t),
    DYNAMIC_DRAW);

  vertex_array_unbind();
}

/*******************************************************************************
 *
 * Function: renderable_3d_line_data_init()
 *
 ******************************************************************************/
void renderable_3d_line_data_init(
  struct renderable_3d_line_data_t* line_data,
  const unsigned int max_count,
  struct shader_program_t* shader_program)
{
  const unsigned int max_vertices = 2 * max_count;

  renderable_3d_primitive_data_init(
    &(line_data->vao),
    &(line_data->vbo),
    NULL,
    0,
    line_vertex_attributes,
    _LINE_3D_ATTRIBUTE_NUM,
    shader_program);

  line_data->line_vertex_count = 0;
  line_data->max_line_vertex_count = max_vertices;

  line_data->vertex_data_base =
    (struct renderable_3d_line_vertex_t*)calloc(
      max_vertices,
      sizeof(struct renderable_3d_line_vertex_t));
  line_data->vertex_data_ptr = line_data->vertex_data_base;

  vertex_buffer_buffer_data(
    &(line_data->vbo),
    0,
    max_vertices * sizeof(struct renderable_3d_line_vertex_t),
    DYNAMIC_DRAW);

  vertex_array_unbind();
}

/*******************************************************************************
 *
 * Function: renderable_3d_shaded_data_cleanup()
 *
 ******************************************************************************/
void renderable_3d_shaded_data_cleanup(
  struct renderable_3d_shaded_data_t* shaded_data)
{
  vertex_array_destroy(
    &(shaded_data->vao),
    1);
  vertex_buffer_destroy(
    &(shaded_data->vbo),
    1);
  element_buffer_destroy(
    &(shaded_data->ebo),
    1);

  free(shaded_data->vertex_data_base);
  shaded_data->vertex_data_base = NULL;
  shaded_data->vertex_data_ptr = NULL;
}

/*******************************************************************************
 *
 * Function: renderable_3d_textured_data_cleanup()
 *
 ******************************************************************************/
void renderable_3d_textured_data_cleanup(
  struct renderable_3d_textured_data_t* textured_data)
{
  vertex_array_destroy(
    &(textured_data->vao),
    1);
  vertex_buffer_destroy(
    &(textured_data->vbo),
    1);
  element_buffer_destroy(
    &(textured_data->ebo),
    1);

  free(textured_data->vertex_data_base);
  textured_data->vertex_data_base = NULL;
  textured_data->vertex_data_ptr = NULL;
}

/*******************************************************************************
 *
 * Function: renderable_3d_line_data_cleanup()
 *
 ******************************************************************************/
void renderable_3d_line_data_cleanup(
  struct renderable_3d_line_data_t* line_data)
{
  vertex_array_destroy(
    &(line_data->vao),
    1);
  vertex_buffer_destroy(
    &(line_data->vbo),
    1);

  free(line_data->vertex_data_base);
  line_data->vertex_data_base = NULL;
  line_data->vertex_data_ptr = NULL;
}
