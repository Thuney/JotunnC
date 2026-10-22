#include <memory.h>
#include <stdlib.h>

#include <stdio.h>

#include "vertexattribute.h"

#include "allocator.h"

extern void platform_vertex_attribute_init(
  struct vertex_attribute_t* vertex_attribute,
  unsigned int index,
  int size,
  enum vertex_attribute_data_type_t data_type,
  unsigned int should_normalize,
  unsigned int stride,
  void* ptr_offset_to_attrib);

void vertex_attribute_init(
  struct vertex_attribute_t* vertex_attribute,
  char* attribute_name,
  unsigned int index,
  int size,
  enum vertex_attribute_data_type_t data_type,
  unsigned int should_normalize,
  unsigned int stride,
  void* ptr_offset_to_attrib)
{
  vertex_attribute->attribute_name =
    allocator_duplicate_string(
      attribute_name,
      ALLOCATOR_TAG_RENDERER);

  platform_vertex_attribute_init(
    vertex_attribute,
    index,
    size,
    data_type,
    should_normalize,
    stride,
    ptr_offset_to_attrib);
}

void vertex_attribute_cleanup(
  struct vertex_attribute_t* vertex_attribute,
  unsigned int num)
{
  for (unsigned int i = 0; i < num; i++)
  {
    allocator_free(vertex_attribute[i].attribute_name);
    vertex_attribute[i].attribute_name = 0;
  }
}

#ifdef DEBUG
void vertex_attribute_print(
  struct vertex_attribute_t* vertex_attribute)
{
  fprintf(
    stdout,
    "Vertex Attribute - Name: %s, Index: %u, Size: %u, Data Type: %u, "
    "Normalize: %u, Stride: %u\n",
    vertex_attribute->attribute_name,
    vertex_attribute->index,
    vertex_attribute->size,
    vertex_attribute->data_type,
    vertex_attribute->should_normalize,
    vertex_attribute->stride);
}
#endif
