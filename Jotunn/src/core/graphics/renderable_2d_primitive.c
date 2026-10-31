#include <memory.h>
#include <stdlib.h>
#ifdef DEBUG
  #include <stdio.h>
#endif
#include <math.h>
#include <stddef.h>

#include "renderable_2d_primitive.h"

#include "allocator.h"

const union fvector3_t triangle_2d_position_data[3] =
  {
    [0] = {{0.0f, 0.0f, 0.0f}},
    [1] = {{0.5f, 1.0f, 0.0f}},
    [2] = {{1.0f, 0.0f, 0.0f}}
  };

const union fvector3_t quad_2d_position_data[4] =
  {
    [0] = {{0.0f, 0.0f, 0.0f}},
    [1] = {{1.0f, 0.0f, 0.0f}},
    [2] = {{1.0f, 1.0f, 0.0f}},
    [3] = {{0.0f, 1.0f, 0.0f}}
  };

const union fvector3_t textured_quad_2d_position_data[4] =
  {
    [0] = {{0.0f, 0.0f, 0.0f}},
    [1] = {{1.0f, 0.0f, 0.0f}},
    [2] = {{1.0f, 1.0f, 0.0f}},
    [3] = {{0.0f, 1.0f, 0.0f}}
  };

const union fvector2_t textured_quad_texture_coordinates[4] =
  {
    [0] = {{0.0f, 0.0f}},
    [1] = {{1.0f, 0.0f}},
    [2] = {{1.0f, 1.0f}},
    [3] = {{0.0f, 1.0f}}
  };

union fvector3_t circle_2d_position_data[_NUM_CIRCLE_2D_VERTICES];

#define PI 3.14159f

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Fills circle_2d_position_data with the center of the unit circle followed
//  by _RENDERABLE_2D_CIRCLE_RESOLUTION points evenly spaced around it
static void renderable_2d_generate_circle_position_data(void);

// Creates a primitive's vertex array and buffers, and an index buffer when
//  asked to, then binds each attribute to the shader's attribute of the same
//  name, filling in its index. The vertex array is left bound. out_ebo is
//  untouched, and may be NULL, unless in_should_use_ebo is set
//
//  in:     in_should_use_ebo, in_num_attributes, in_shader_program
//  in/out: inout_attributes
//  out:    out_vao, out_vbo, out_ebo
static void renderable_2d_primitive_data_init(
  struct vertex_array_t* out_vao,
  struct vertex_buffer_t* out_vbo,
  struct element_buffer_t* out_ebo,
  const unsigned int in_should_use_ebo,
  struct vertex_attribute_t* inout_attributes,
  const unsigned int in_num_attributes,
  struct shader_program_t* in_shader_program);

// Triangle 2D vertex attributes
static struct vertex_attribute_t triangle_vertex_attributes[2] =
{
  [0] = {
    .attribute_name = "position",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 3,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_triangle_vertex_t),
    .ptr_offset_to_attrib = 0
  },
  [1] =
  {
    .attribute_name = "color",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 4,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_triangle_vertex_t),
    .ptr_offset_to_attrib = (void*)sizeof(union fvector3_t)
  }
};

// Quad 2D vertex attributes
static struct vertex_attribute_t quad_vertex_attributes[2] =
{
  [0] =
  {
    .attribute_name = "position",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 3,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_quad_vertex_t),
    .ptr_offset_to_attrib = 0
  },
  [1] =
  {
    .attribute_name = "color",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 4,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_quad_vertex_t),
    .ptr_offset_to_attrib = (void*)sizeof(union fvector3_t)
  }
};

// Textured Quad 2D vertex attributes
static struct vertex_attribute_t textured_quad_vertex_attributes[4] =
{
  [0] =
  {
    .attribute_name = "position",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 3,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_textured_quad_vertex_t),
    .ptr_offset_to_attrib = 0
  },
  [1] =
  {
    .attribute_name = "texCoord",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 2,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_textured_quad_vertex_t),
    .ptr_offset_to_attrib = (void*)sizeof(union fvector3_t)
  },
  [2] =
  {
    .attribute_name = "texIndex",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 1,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_textured_quad_vertex_t),
    .ptr_offset_to_attrib = (void*)(sizeof(union fvector3_t) + sizeof(union fvector2_t))
  },
  [3] =
  {
    .attribute_name = "tilingFactor",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 1,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_textured_quad_vertex_t),
    .ptr_offset_to_attrib =
      (void*)(sizeof(union fvector3_t) + sizeof(union fvector2_t) + sizeof(float))
  }
};

// Circle 2D vertex attributes
static struct vertex_attribute_t circle_vertex_attributes[2] =
{
  [0] =
  {
    .attribute_name = "position",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 3,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_circle_vertex_t),
    .ptr_offset_to_attrib = 0
  },
  [1] =
  {
    .attribute_name = "color",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 4,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_circle_vertex_t),
    .ptr_offset_to_attrib = (void*)sizeof(union fvector3_t)
  }
};

// Line 2D vertex attributes
static struct vertex_attribute_t line_vertex_attributes[2] =
{
  [0] =
  {
    .attribute_name = "position",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 3,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_line_vertex_t),
    .ptr_offset_to_attrib = 0
  },
  [1] =
  {
    .attribute_name = "color",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 4,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_line_vertex_t),
    .ptr_offset_to_attrib = (void*)sizeof(union fvector3_t)
  }
};

// Mesh 2D vertex attributes
static struct vertex_attribute_t mesh_vertex_attributes[3] =
{
  [RENDERABLE_2D_MESH_VERTEX_ATTRIBUTE_POSITION] =
  {
    .attribute_name = "position",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 2,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_mesh_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_2d_mesh_vertex_t,
        position)
  },
  [RENDERABLE_2D_MESH_VERTEX_ATTRIBUTE_TEXTURE_COORDINATES] =
  {
    .attribute_name = "texCoord",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 2,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderable_2d_mesh_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_2d_mesh_vertex_t,
        texture_coordinate)
  },
  [RENDERABLE_2D_MESH_VERTEX_ATTRIBUTE_COLOR] =
  {
    .attribute_name = "color",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 4,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_UNSIGNED_BYTE,
    .should_normalize = 1,
    .stride = sizeof(struct renderable_2d_mesh_vertex_t),
    .ptr_offset_to_attrib =
      (void*)offsetof(
        struct renderable_2d_mesh_vertex_t,
        color)
  }
};

/*******************************************************************************
 *
 *   Function: renderable_2d_triangle_data_init()
 *
 ******************************************************************************/
void renderable_2d_triangle_data_init(
  struct renderable_2d_triangle_data_t* out_triangle_data,
  const unsigned int in_max_count,
  struct shader_program_t* in_shader_program)
{
  const unsigned int max_vertices = 3 * in_max_count;
  const unsigned int max_indices = 3 * in_max_count;

  renderable_2d_primitive_data_init(
    &(out_triangle_data->vao),
    &(out_triangle_data->vbo),
    &(out_triangle_data->ebo),
    1,
    triangle_vertex_attributes,
    RENDERABLE_2D_TRIANGLE_VERTEX_ATTRIBUTE_COUNT,
    in_shader_program);

  out_triangle_data->vertex_data_base =
    (struct renderable_2d_triangle_vertex_t*)allocator_allocate_zeroed(
      max_vertices,
      sizeof(struct renderable_2d_triangle_vertex_t),
      ALLOCATOR_TAG_RENDERER);
  out_triangle_data->vertex_data_ptr = out_triangle_data->vertex_data_base;

  unsigned int triangle_indices[max_indices];

  unsigned int offset = 0;
  for (unsigned int i = 0; i < max_indices; i += 3) {
    triangle_indices[i + 0] = offset + 0;
    triangle_indices[i + 1] = offset + 1;
    triangle_indices[i + 2] = offset + 2;

    offset += 3;
  }

  element_buffer_buffer_data(
    &(out_triangle_data->ebo),
    triangle_indices,
    max_indices * sizeof(unsigned int),
    VERTEX_BUFFER_DATA_USAGE_STATIC_DRAW);
  vertex_buffer_set_data(
    &(out_triangle_data->vbo),
    (float*)out_triangle_data->vertex_data_base,
    max_vertices * sizeof(struct renderable_2d_triangle_vertex_t),
    VERTEX_BUFFER_DATA_USAGE_DYNAMIC_DRAW);
  vertex_array_unbind();
}

/*******************************************************************************
 *
 *   Function: renderable_2d_quad_data_init()
 *
 ******************************************************************************/
void renderable_2d_quad_data_init(
  struct renderable_2d_quad_data_t* out_quad_data,
  const unsigned int in_max_count,
  struct shader_program_t* in_shader_program)
{
  const unsigned int max_vertices = 4 * in_max_count;
  const unsigned int max_indices = 6 * in_max_count;

  renderable_2d_primitive_data_init(
    &(out_quad_data->vao),
    &(out_quad_data->vbo),
    &(out_quad_data->ebo),
    1,
    quad_vertex_attributes,
    RENDERABLE_2D_QUAD_VERTEX_ATTRIBUTE_COUNT,
    in_shader_program);

  out_quad_data->vertex_data_base =
    (struct renderable_2d_quad_vertex_t*)allocator_allocate_zeroed(
      max_vertices,
      sizeof(struct renderable_2d_quad_vertex_t),
      ALLOCATOR_TAG_RENDERER);
  out_quad_data->vertex_data_ptr = out_quad_data->vertex_data_base;

  unsigned int quad_indices[max_indices];

  unsigned int offset = 0;
  for (unsigned int i = 0; i < max_indices; i += 6) {
    quad_indices[i + 0] = offset + 0;
    quad_indices[i + 1] = offset + 1;
    quad_indices[i + 2] = offset + 2;

    quad_indices[i + 3] = offset + 0;
    quad_indices[i + 4] = offset + 3;
    quad_indices[i + 5] = offset + 2;

    offset += 4;
  }

  element_buffer_buffer_data(
    &(out_quad_data->ebo),
    quad_indices,
    max_indices * sizeof(unsigned int),
    VERTEX_BUFFER_DATA_USAGE_STATIC_DRAW);
  vertex_buffer_set_data(
    &(out_quad_data->vbo),
    (float*)out_quad_data->vertex_data_base,
    max_vertices * sizeof(struct renderable_2d_quad_vertex_t),
    VERTEX_BUFFER_DATA_USAGE_DYNAMIC_DRAW);

  vertex_array_unbind();
}

/*******************************************************************************
 *
 *   Function: renderable_2d_textured_quad_data_init()
 *
 ******************************************************************************/
void renderable_2d_textured_quad_data_init(
  struct renderable_2d_textured_quad_data_t* out_textured_quad_data,
  const unsigned int in_max_count,
  struct shader_program_t* in_shader_program)
{
  const unsigned int max_vertices = 4 * in_max_count;
  const unsigned int max_indices = 6 * in_max_count;

  renderable_2d_primitive_data_init(
    &(out_textured_quad_data->vao),
    &(out_textured_quad_data->vbo),
    &(out_textured_quad_data->ebo),
    1,
    textured_quad_vertex_attributes,
    RENDERABLE_2D_TEXTURED_QUAD_VERTEX_ATTRIBUTE_COUNT,
    in_shader_program);

  out_textured_quad_data->vertex_data_base =
    (struct renderable_2d_textured_quad_vertex_t*)allocator_allocate_zeroed(
      max_vertices,
      sizeof(struct renderable_2d_textured_quad_vertex_t),
      ALLOCATOR_TAG_RENDERER);
  out_textured_quad_data->vertex_data_ptr = out_textured_quad_data->vertex_data_base;

  unsigned int textured_quad_indices[max_indices];

  unsigned int offset = 0;
  for (unsigned int i = 0; i < max_indices; i += 6) {
    textured_quad_indices[i + 0] = offset + 0;
    textured_quad_indices[i + 1] = offset + 1;
    textured_quad_indices[i + 2] = offset + 2;

    textured_quad_indices[i + 3] = offset + 0;
    textured_quad_indices[i + 4] = offset + 3;
    textured_quad_indices[i + 5] = offset + 2;

    offset += 4;
  }

  element_buffer_buffer_data(
    &(out_textured_quad_data->ebo),
    textured_quad_indices,
    max_indices * sizeof(unsigned int),
    VERTEX_BUFFER_DATA_USAGE_STATIC_DRAW);
  vertex_buffer_set_data(
    &(out_textured_quad_data->vbo),
    (float*)out_textured_quad_data->vertex_data_base,
    max_vertices * sizeof(struct renderable_2d_textured_quad_vertex_t),
    VERTEX_BUFFER_DATA_USAGE_DYNAMIC_DRAW);

  vertex_array_unbind();
}

/*******************************************************************************
 *
 *   Function: renderable_2d_circle_data_init()
 *
 ******************************************************************************/
void renderable_2d_circle_data_init(
  struct renderable_2d_circle_data_t* out_circle_data,
  const unsigned int in_max_count,
  struct shader_program_t* in_shader_program)
{
  renderable_2d_generate_circle_position_data();

  const unsigned int indices_per_circle = (3 * _RENDERABLE_2D_CIRCLE_RESOLUTION);

  const unsigned int max_vertices = _NUM_CIRCLE_2D_VERTICES * in_max_count;
  const unsigned int max_indices = indices_per_circle * in_max_count;

  renderable_2d_primitive_data_init(
    &(out_circle_data->vao),
    &(out_circle_data->vbo),
    &(out_circle_data->ebo),
    1,
    circle_vertex_attributes,
    RENDERABLE_2D_CIRCLE_VERTEX_ATTRIBUTE_COUNT,
    in_shader_program);

  out_circle_data->vertex_data_base =
    (struct renderable_2d_circle_vertex_t*)allocator_allocate_zeroed(
      max_vertices,
      sizeof(struct renderable_2d_circle_vertex_t),
      ALLOCATOR_TAG_RENDERER);
  out_circle_data->vertex_data_ptr = out_circle_data->vertex_data_base;

  unsigned int circle_indices[max_indices];

  unsigned int circle;

  unsigned int vertex_offset = 0;

  for (circle = 0; circle < in_max_count; circle++) {
    unsigned int triangle;
    for (triangle = 1; triangle < _RENDERABLE_2D_CIRCLE_RESOLUTION; triangle++) {
      circle_indices[indices_per_circle * circle + (3 * (triangle - 1)) + 0] = vertex_offset;
      circle_indices[indices_per_circle * circle + (3 * (triangle - 1)) + 1] =
        vertex_offset + triangle;
      circle_indices[indices_per_circle * circle + (3 * (triangle - 1)) + 2] =
        vertex_offset + triangle + 1;
    }
    circle_indices[indices_per_circle * circle + (3 * (triangle - 1)) + 0] = vertex_offset;
    circle_indices[indices_per_circle * circle + (3 * (triangle - 1)) + 1] =
      vertex_offset + triangle;
    circle_indices[indices_per_circle * circle + (3 * (triangle - 1)) + 2] = vertex_offset + 1;

    vertex_offset += _NUM_CIRCLE_2D_VERTICES;
  }

  element_buffer_buffer_data(
    &(out_circle_data->ebo),
    circle_indices,
    max_indices * sizeof(unsigned int),
    VERTEX_BUFFER_DATA_USAGE_STATIC_DRAW);
  vertex_buffer_set_data(
    &(out_circle_data->vbo),
    (float*)out_circle_data->vertex_data_base,
    max_vertices * sizeof(struct renderable_2d_circle_vertex_t),
    VERTEX_BUFFER_DATA_USAGE_DYNAMIC_DRAW);

  vertex_array_unbind();
}

/*******************************************************************************
 *
 *   Function: renderable_2d_line_data_init()
 *
 ******************************************************************************/
void renderable_2d_line_data_init(
  struct renderable_2d_line_data_t* out_line_data,
  const unsigned int in_max_count,
  struct shader_program_t* in_shader_program)
{
  const unsigned int max_vertices = 2 * in_max_count;

  renderable_2d_primitive_data_init(
    &(out_line_data->vao),
    &(out_line_data->vbo),
    NULL,
    0,
    line_vertex_attributes,
    RENDERABLE_2D_LINE_VERTEX_ATTRIBUTE_COUNT,
    in_shader_program);

  out_line_data->vertex_data_base =
    (struct renderable_2d_line_vertex_t*)allocator_allocate_zeroed(
      max_vertices,
      sizeof(struct renderable_2d_line_vertex_t),
      ALLOCATOR_TAG_RENDERER);
  out_line_data->vertex_data_ptr = out_line_data->vertex_data_base;

  vertex_buffer_set_data(
    &(out_line_data->vbo),
    0,
    max_vertices * sizeof(struct renderable_2d_line_vertex_t),
    VERTEX_BUFFER_DATA_USAGE_DYNAMIC_DRAW);

  vertex_array_unbind();
}

/*******************************************************************************
 *
 *   Function: renderable_2d_mesh_data_init()
 *
 ******************************************************************************/
void renderable_2d_mesh_data_init(
  struct renderable_2d_mesh_data_t* out_mesh_data,
  struct shader_program_t* in_shader_program)
{
  renderable_2d_primitive_data_init(
    &(out_mesh_data->vao),
    &(out_mesh_data->vbo),
    &(out_mesh_data->ebo),
    1,
    mesh_vertex_attributes,
    RENDERABLE_2D_MESH_VERTEX_ATTRIBUTE_COUNT,
    in_shader_program);

  vertex_array_unbind();
}

/*******************************************************************************
 *
 *   Function: renderable_2d_triangle_data_cleanup()
 *
 ******************************************************************************/
void renderable_2d_triangle_data_cleanup(
  struct renderable_2d_triangle_data_t* inout_triangle_data)
{
  vertex_array_destroy(
    &(inout_triangle_data->vao),
    1);
  vertex_buffer_destroy(
    &(inout_triangle_data->vbo),
    1);
  element_buffer_destroy(
    &(inout_triangle_data->ebo),
    1);

  allocator_free(inout_triangle_data->vertex_data_base);
}

/*******************************************************************************
 *
 *   Function: renderable_2d_quad_data_cleanup()
 *
 ******************************************************************************/
void renderable_2d_quad_data_cleanup(
  struct renderable_2d_quad_data_t* inout_quad_data)
{
  vertex_array_destroy(
    &(inout_quad_data->vao),
    1);
  vertex_buffer_destroy(
    &(inout_quad_data->vbo),
    1);
  element_buffer_destroy(
    &(inout_quad_data->ebo),
    1);

  allocator_free(inout_quad_data->vertex_data_base);
}

/*******************************************************************************
 *
 *   Function: renderable_2d_textured_quad_data_cleanup()
 *
 ******************************************************************************/
void renderable_2d_textured_quad_data_cleanup(
  struct renderable_2d_textured_quad_data_t* inout_textured_quad_data)
{
  vertex_array_destroy(
    &(inout_textured_quad_data->vao),
    1);
  vertex_buffer_destroy(
    &(inout_textured_quad_data->vbo),
    1);
  element_buffer_destroy(
    &(inout_textured_quad_data->ebo),
    1);

  allocator_free(inout_textured_quad_data->vertex_data_base);
}

/*******************************************************************************
 *
 *   Function: renderable_2d_circle_data_cleanup()
 *
 ******************************************************************************/
void renderable_2d_circle_data_cleanup(
  struct renderable_2d_circle_data_t* inout_circle_data)
{
  vertex_array_destroy(
    &(inout_circle_data->vao),
    1);
  vertex_buffer_destroy(
    &(inout_circle_data->vbo),
    1);
  element_buffer_destroy(
    &(inout_circle_data->ebo),
    1);

  allocator_free(inout_circle_data->vertex_data_base);
}

/*******************************************************************************
 *
 *   Function: renderable_2d_line_data_cleanup()
 *
 ******************************************************************************/
void renderable_2d_line_data_cleanup(
  struct renderable_2d_line_data_t* inout_line_data)
{
  vertex_array_destroy(
    &(inout_line_data->vao),
    1);
  vertex_buffer_destroy(
    &(inout_line_data->vbo),
    1);

  allocator_free(inout_line_data->vertex_data_base);
}

/*******************************************************************************
 *
 *   Function: renderable_2d_mesh_data_cleanup()
 *
 ******************************************************************************/
void renderable_2d_mesh_data_cleanup(
  struct renderable_2d_mesh_data_t* inout_mesh_data)
{
  vertex_array_destroy(
    &(inout_mesh_data->vao),
    1);
  vertex_buffer_destroy(
    &(inout_mesh_data->vbo),
    1);
  element_buffer_destroy(
    &(inout_mesh_data->ebo),
    1);
}

/*******************************************************************************
 *
 *   Function: renderable_2d_generate_circle_position_data()
 *
 ******************************************************************************/
static void renderable_2d_generate_circle_position_data(void)
{
  const float angle_increment_degrees = (360.0f / (float)_RENDERABLE_2D_CIRCLE_RESOLUTION);

  float angle_degrees = 0.0f;

  unsigned int v;

  const union fvector2_t offset = {0.0f, 0.0f};

  float x, y, angle_radians;

  circle_2d_position_data[0] =
    (union fvector3_t){offset.comp.x, offset.comp.y, 0.0f}; // Origin / Center of circle

  // Generate vertices on circle
  for (v = 1; v < _NUM_CIRCLE_2D_VERTICES; v++) {
    angle_radians = (PI / 180.0f) * angle_degrees;

    x = (cosf(angle_radians) + offset.comp.x);
    y = (sinf(angle_radians) + offset.comp.y);

    circle_2d_position_data[v] = (union fvector3_t){x, y, 0.0f};

    angle_degrees += angle_increment_degrees;
  }
}

/*******************************************************************************
 *
 *   Function: renderable_2d_primitive_data_init()
 *
 ******************************************************************************/
static void renderable_2d_primitive_data_init(
  struct vertex_array_t* out_vao,
  struct vertex_buffer_t* out_vbo,
  struct element_buffer_t* out_ebo,
  const unsigned int in_should_use_ebo,
  struct vertex_attribute_t* inout_attributes,
  const unsigned int in_num_attributes,
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

  unsigned int attribute_index, temp_shader_index;
  char* attribute_name;

  for (attribute_index = 0; attribute_index < in_num_attributes; attribute_index++) {
    attribute_name = inout_attributes[attribute_index].attribute_name;
    temp_shader_index = shader_program_get_attribute_location(
      in_shader_program,
      attribute_name);

    // #ifdef DEBUG
    //  fprintf(stdout, "Setting shader attribute at index %u with name %s\n",
    //  temp_shader_index, attribute_name);
    // #endif

    inout_attributes[attribute_index].index = temp_shader_index;

    // #ifdef DEBUG
    //    vertex_attribute_print(&attributes[attribute_index]);
    // #endif

    vertex_array_set_attribute(
      out_vao,
      &(inout_attributes[attribute_index]));
  }
}
