#pragma once

// Vertex arrays, which record how vertex buffers are read into a shader's
//  vertex attributes. Each is registered as a resource from
//  vertex_array_init() until vertex_array_destroy()

#include "resource.h"
#include "vertexattribute.h"

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

struct vertex_array_t
{
  unsigned int vertex_array;
  // Its entry in the resource registry
  struct resource_t resource;
};

/*******************************************************************************
 *
 *   VERTEX ARRAYS
 *
 ******************************************************************************/

// Creates in_num vertex arrays, one in each element of the array
//  inout_vertex_array points to, and registers each as a resource
//
//  in:     in_num
//  in/out: inout_vertex_array
void vertex_array_init(
  struct vertex_array_t* inout_vertex_array,
  const int in_num);

// Makes a vertex array the one drawn from and configured
//
//  in:     in_vertex_array
void vertex_array_bind(const struct vertex_array_t* in_vertex_array);

// Leaves no vertex array bound
void vertex_array_unbind(void);

// Sets where and how a vertex attribute is read, and enables it. It applies
//  to the bound vertex array, which must be in_vertex_array, and reads from
//  the vertex buffer bound at the time
//
//  in:     in_vertex_array, in_vertex_attribute
void vertex_array_set_attribute(
  const struct vertex_array_t* in_vertex_array,
  const struct vertex_attribute_t* in_vertex_attribute);

// Deletes in_num vertex arrays, one in each element of the array
//  inout_vertex_array points to, and removes each from the resource registry
//
//  in:     in_num
//  in/out: inout_vertex_array
void vertex_array_destroy(
  struct vertex_array_t* inout_vertex_array,
  const int in_num);
