#include <memory.h>
#include <stdlib.h>

#include <stdio.h>

#include "vertexattribute.h"
#include "vertex_attribute_internal.h"

/*******************************************************************************
 *
 *   Function: vertex_attribute_init()
 *
 ******************************************************************************/
void vertex_attribute_init(
  struct vertex_attribute_t* out_vertex_attribute,
  const char* in_attribute_name,
  unsigned int in_index,
  int in_size,
  enum vertex_attribute_data_type_t in_data_type,
  unsigned int in_should_normalize,
  unsigned int in_stride,
  void* in_ptr_offset_to_attrib)
{
  out_vertex_attribute->attribute_name = in_attribute_name;

  vertex_attribute_internal_platform_init(
    out_vertex_attribute,
    in_index,
    in_size,
    in_data_type,
    in_should_normalize,
    in_stride,
    in_ptr_offset_to_attrib);
}

#ifdef DEBUG
/*******************************************************************************
 *
 *   Function: vertex_attribute_print()
 *
 ******************************************************************************/
void vertex_attribute_print(
  const struct vertex_attribute_t* in_vertex_attribute)
{
  fprintf(
    stdout,
    "Vertex Attribute - Name: %s, Index: %u, Size: %u, Data Type: %u, "
    "Normalize: %u, Stride: %u\n",
    in_vertex_attribute->attribute_name,
    in_vertex_attribute->index,
    in_vertex_attribute->size,
    in_vertex_attribute->data_type,
    in_vertex_attribute->should_normalize,
    in_vertex_attribute->stride);
}
#endif
