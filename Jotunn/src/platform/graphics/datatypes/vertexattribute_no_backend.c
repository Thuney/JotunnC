#include "vertexattribute.h"
#include "../../../core/graphics/datatypes/vertex_attribute_internal.h"

/*******************************************************************************
 *
 *   Function: vertex_attribute_internal_platform_init()
 *
 ******************************************************************************/
void vertex_attribute_internal_platform_init(
  struct vertex_attribute_t* out_vertex_attribute,
  const unsigned int in_index,
  const int in_size,
  const enum vertex_attribute_data_type_t in_data_type,
  const unsigned int in_should_normalize,
  const unsigned int in_stride,
  void* in_ptr_offset_to_attrib)
{
  return 0;
}
