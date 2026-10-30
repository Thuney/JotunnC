#pragma once

// Private to Jotunn: the backend hook behind vertexattribute.h. Each
//  rendering backend defines it, and vertexattribute.c calls it, copying the
//  attribute's name itself

#include "vertexattribute.h"

/*******************************************************************************
 *
 *   PLATFORM VERTEX ATTRIBUTES
 *
 ******************************************************************************/

// Fills in every field of a vertex attribute but its name, as described for
//  vertex_attribute_init()
//
//  in:     in_index, in_size, in_data_type, in_should_normalize, in_stride,
//            in_ptr_offset_to_attrib
//  out:    out_vertex_attribute
void vertex_attribute_internal_platform_init(
  struct vertex_attribute_t* out_vertex_attribute,
  const unsigned int in_index,
  const int in_size,
  const enum vertex_attribute_data_type_t in_data_type,
  const unsigned int in_should_normalize,
  const unsigned int in_stride,
  void* in_ptr_offset_to_attrib);
