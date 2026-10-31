#include "vertexattribute.h"
#include "../../../core/graphics/datatypes/vertex_attribute_internal.h"

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Fills in every field of a vertex attribute but its name
//
//  in:     in_index, in_size, in_data_type, in_should_normalize, in_stride,
//            in_ptr_offset_to_attrib
//  out:    out_vertex_attribute
static void vertex_attribute_gl_init(
  struct vertex_attribute_t* out_vertex_attribute,
  const unsigned int in_index,
  const int in_size,
  const enum vertex_attribute_data_type_t in_data_type,
  const unsigned int in_should_normalize,
  const unsigned int in_stride,
  void* in_ptr_offset_to_attrib);

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
  vertex_attribute_gl_init(
    out_vertex_attribute,
    in_index,
    in_size,
    in_data_type,
    in_should_normalize,
    in_stride,
    in_ptr_offset_to_attrib);
}

/*******************************************************************************
 *
 *   Function: vertex_attribute_gl_init()
 *
 ******************************************************************************/
static void vertex_attribute_gl_init(
  struct vertex_attribute_t* out_vertex_attribute,
  const unsigned int in_index,
  const int in_size,
  const enum vertex_attribute_data_type_t in_data_type,
  const unsigned int in_should_normalize,
  const unsigned int in_stride,
  void* in_ptr_offset_to_attrib)
{
  out_vertex_attribute->index = in_index;
  out_vertex_attribute->size = in_size;
  out_vertex_attribute->data_type = in_data_type;
  out_vertex_attribute->should_normalize = in_should_normalize;
  out_vertex_attribute->stride = in_stride;
  out_vertex_attribute->ptr_offset_to_attrib = in_ptr_offset_to_attrib;
}
