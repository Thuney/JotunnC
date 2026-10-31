#include <memory.h>
#include <stdlib.h>
#ifdef DEBUG
  #include <stdio.h>
#endif
#include <math.h>
#include <stddef.h>

#include "renderable_2d_primitive.h"

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

// How many indices are built at a time when filling an element buffer
#define RENDERABLE_2D_INDEX_UPLOAD_CHUNK_COUNT 256

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
  unsigned int in_should_use_ebo,
  struct vertex_attribute_t* inout_attributes,
  unsigned int in_num_attributes,
  struct shader_program_t* in_shader_program);

// Fills the bound element buffer, which must be inout_ebo, with
//  in_index_count indices, taking the one at each position from
//  in_function_get_index. They're built and uploaded a chunk at a time, so
//  however many there are, only a chunk's worth is held in memory
//
//  in:     in_index_count, in_function_get_index
//  in/out: inout_ebo
static void renderable_2d_upload_indices(
  struct element_buffer_t* inout_ebo,
  unsigned int in_index_count,
  unsigned int (*in_function_get_index)(
    unsigned int in_position));

// Finds the index at a position in a batch's triangle indices, which draw
//  each triangle's three vertices in turn
//
//  in:     in_position
//  ret:    The index of the vertex drawn at in_position
static unsigned int renderable_2d_get_triangle_index(
  unsigned int in_position);

// Finds the index at a position in a batch's quad indices, which draw each
//  quad's four vertices as two triangles
//
//  in:     in_position
//  ret:    The index of the vertex drawn at in_position
static unsigned int renderable_2d_get_quad_index(
  unsigned int in_position);

// Finds the index at a position in a batch's circle indices, which draw each
//  circle as a fan of _RENDERABLE_2D_CIRCLE_RESOLUTION triangles around its
//  center vertex
//
//  in:     in_position
//  ret:    The index of the vertex drawn at in_position
static unsigned int renderable_2d_get_circle_index(
  unsigned int in_position);

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
  unsigned int in_max_count,
  struct renderable_2d_triangle_vertex_t* inout_vertex_storage,
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

  out_triangle_data->vertex_data_base = inout_vertex_storage;
  out_triangle_data->vertex_data_ptr = out_triangle_data->vertex_data_base;

  renderable_2d_upload_indices(
    &(out_triangle_data->ebo),
    max_indices,
    &(renderable_2d_get_triangle_index));
  vertex_buffer_set_data(
    &(out_triangle_data->vbo),
    NULL,
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
  unsigned int in_max_count,
  struct renderable_2d_quad_vertex_t* inout_vertex_storage,
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

  out_quad_data->vertex_data_base = inout_vertex_storage;
  out_quad_data->vertex_data_ptr = out_quad_data->vertex_data_base;

  renderable_2d_upload_indices(
    &(out_quad_data->ebo),
    max_indices,
    &(renderable_2d_get_quad_index));
  vertex_buffer_set_data(
    &(out_quad_data->vbo),
    NULL,
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
  unsigned int in_max_count,
  struct renderable_2d_textured_quad_vertex_t* inout_vertex_storage,
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

  out_textured_quad_data->vertex_data_base = inout_vertex_storage;
  out_textured_quad_data->vertex_data_ptr = out_textured_quad_data->vertex_data_base;

  // Textured quads share the quads' indices
  renderable_2d_upload_indices(
    &(out_textured_quad_data->ebo),
    max_indices,
    &(renderable_2d_get_quad_index));
  vertex_buffer_set_data(
    &(out_textured_quad_data->vbo),
    NULL,
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
  unsigned int in_max_count,
  struct renderable_2d_circle_vertex_t* inout_vertex_storage,
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

  out_circle_data->vertex_data_base = inout_vertex_storage;
  out_circle_data->vertex_data_ptr = out_circle_data->vertex_data_base;

  renderable_2d_upload_indices(
    &(out_circle_data->ebo),
    max_indices,
    &(renderable_2d_get_circle_index));
  vertex_buffer_set_data(
    &(out_circle_data->vbo),
    NULL,
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
  unsigned int in_max_count,
  struct renderable_2d_line_vertex_t* inout_vertex_storage,
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

  out_line_data->vertex_data_base = inout_vertex_storage;
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
  unsigned int in_should_use_ebo,
  struct vertex_attribute_t* inout_attributes,
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

  unsigned int attribute_index, temp_shader_index;
  const char* attribute_name;

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

/*******************************************************************************
 *
 *   Function: renderable_2d_upload_indices()
 *
 ******************************************************************************/
static void renderable_2d_upload_indices(
  struct element_buffer_t* inout_ebo,
  unsigned int in_index_count,
  unsigned int (*in_function_get_index)(
    unsigned int in_position))
{
  // Sized for every index, then filled in a chunk at a time
  element_buffer_buffer_data(
    inout_ebo,
    NULL,
    (in_index_count * sizeof(unsigned int)),
    VERTEX_BUFFER_DATA_USAGE_STATIC_DRAW);

  unsigned int chunk[RENDERABLE_2D_INDEX_UPLOAD_CHUNK_COUNT];

  unsigned int start = 0;
  while (start < in_index_count) {
    const unsigned int remaining_count = (in_index_count - start);
    const unsigned int chunk_count =
      (remaining_count < RENDERABLE_2D_INDEX_UPLOAD_CHUNK_COUNT) ?
        remaining_count :
        RENDERABLE_2D_INDEX_UPLOAD_CHUNK_COUNT;

    for (unsigned int i = 0; i < chunk_count; i++) {
      chunk[i] = in_function_get_index(start + i);
    }

    element_buffer_set_sub_data(
      inout_ebo,
      (start * sizeof(unsigned int)),
      chunk,
      (chunk_count * sizeof(unsigned int)));

    start += chunk_count;
  }
}

/*******************************************************************************
 *
 *   Function: renderable_2d_get_triangle_index()
 *
 ******************************************************************************/
static unsigned int renderable_2d_get_triangle_index(
  unsigned int in_position)
{
  // Each triangle has vertices of its own, drawn in order
  return in_position;
}

/*******************************************************************************
 *
 *   Function: renderable_2d_get_quad_index()
 *
 ******************************************************************************/
static unsigned int renderable_2d_get_quad_index(
  unsigned int in_position)
{
  // Corners 0, 1, 2 then 0, 3, 2 of each quad's four vertices
  static const unsigned int quad_pattern[6] = {0, 1, 2, 0, 3, 2};

  const unsigned int quad = (in_position / 6);
  const unsigned int index = ((quad * 4) + quad_pattern[in_position % 6]);

  return index;
}

/*******************************************************************************
 *
 *   Function: renderable_2d_get_circle_index()
 *
 ******************************************************************************/
static unsigned int renderable_2d_get_circle_index(
  unsigned int in_position)
{
  const unsigned int indices_per_circle = (3 * _RENDERABLE_2D_CIRCLE_RESOLUTION);

  const unsigned int circle = (in_position / indices_per_circle);
  const unsigned int triangle = ((in_position % indices_per_circle) / 3);
  const unsigned int corner = (in_position % 3);

  // Vertex 0 is the circle's center, and 1 on are the points around it
  const unsigned int center_index = (circle * _NUM_CIRCLE_2D_VERTICES);

  if (corner == 0) {
    return center_index;
  }

  // The last triangle closes the fan, ending on the first point
  const unsigned int point =
    (corner == 1) ?
      triangle :
      ((triangle + 1) % _RENDERABLE_2D_CIRCLE_RESOLUTION);
  const unsigned int index = (center_index + 1 + point);

  return index;
}
