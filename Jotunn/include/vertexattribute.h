#pragma once

// Vertex attributes: where and how each value a vertex shader takes per
//  vertex is laid out in a vertex buffer. A vertex array is told about each
//  one with vertex_array_set_attribute()

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

enum vertex_attribute_data_type_t
{
  VERTEX_ATTRIBUTE_DATA_TYPE_BYTE = 0x00,
  VERTEX_ATTRIBUTE_DATA_TYPE_UNSIGNED_BYTE,
  VERTEX_ATTRIBUTE_DATA_TYPE_SHORT,
  VERTEX_ATTRIBUTE_DATA_TYPE_UNSIGNED_SHORT,
  VERTEX_ATTRIBUTE_DATA_TYPE_INT,
  VERTEX_ATTRIBUTE_DATA_TYPE_UNSIGNED_INT,
  VERTEX_ATTRIBUTE_DATA_TYPE_HALF_FLOAT,
  VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
  VERTEX_ATTRIBUTE_DATA_TYPE_DOUBLE,
  VERTEX_ATTRIBUTE_DATA_TYPE_FIXED
};

struct vertex_attribute_t
{
  char* attribute_name;
  unsigned int index;
  int size;
  enum vertex_attribute_data_type_t data_type;
  unsigned int should_normalize;
  unsigned int stride;
  void* ptr_offset_to_attrib;
};

/*******************************************************************************
 *
 *   VERTEX ATTRIBUTES
 *
 ******************************************************************************/

// Describes a vertex attribute, keeping a copy of its name (NULL when it
//  can't be allocated) until vertex_attribute_cleanup(). in_index is the
//  attribute's location in the shader, in_size its number of components,
//  in_stride the bytes from one vertex to the next (0 when tightly packed),
//  and in_ptr_offset_to_attrib the byte offset of its first component in the
//  buffer, cast to a pointer. A nonzero in_should_normalize maps integer data
//  to [0, 1] or [-1, 1]
//
//  in:     in_attribute_name, in_index, in_size, in_data_type,
//            in_should_normalize, in_stride, in_ptr_offset_to_attrib
//  out:    out_vertex_attribute
void vertex_attribute_init(
  struct vertex_attribute_t* out_vertex_attribute,
  const char* in_attribute_name,
  const unsigned int in_index,
  const int in_size,
  const enum vertex_attribute_data_type_t in_data_type,
  const unsigned int in_should_normalize,
  const unsigned int in_stride,
  void* in_ptr_offset_to_attrib);

// Frees the names of in_num vertex attributes, one in each element of the
//  array inout_vertex_attribute points to, leaving them NULL
//
//  in:     in_num
//  in/out: inout_vertex_attribute
void vertex_attribute_cleanup(
  struct vertex_attribute_t* inout_vertex_attribute,
  const unsigned int in_num);

/*******************************************************************************
 *
 *   DEBUGGING
 *
 ******************************************************************************/

#ifdef DEBUG
// Prints a vertex attribute's description to stdout; DEBUG builds only
//
//  in:     in_vertex_attribute
void vertex_attribute_print(
  const struct vertex_attribute_t* in_vertex_attribute);
#endif
