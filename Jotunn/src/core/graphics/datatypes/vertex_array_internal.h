#pragma once

// Private to Jotunn: the backend hooks behind vertexarray.h. Each rendering
//  backend defines these, and vertexarray.c calls them, handling the resource
//  registry itself

#include "vertexarray.h"

/*******************************************************************************
 *
 *   PLATFORM VERTEX ARRAYS
 *
 ******************************************************************************/

// Creates a vertex array in out_vertex_array. The backend writes in_num names
//  from out_vertex_array->vertex_array on, so anything but 1 overruns it
//
//  in:     in_num
//  out:    out_vertex_array
void vertex_array_internal_platform_init(
  struct vertex_array_t* out_vertex_array,
  int in_num);

// Makes a vertex array the one drawn from and configured
//
//  in:     in_vertex_array
void vertex_array_internal_platform_bind(
  const struct vertex_array_t* in_vertex_array);

// Leaves no vertex array bound
void vertex_array_internal_platform_unbind(void);

// Sets where and how a vertex attribute is read, and enables it, in the bound
//  vertex array, reading from the vertex buffer bound at the time
//
//  in:     in_vertex_array, in_vertex_attribute
void vertex_array_internal_platform_set_attribute(
  const struct vertex_array_t* in_vertex_array,
  const struct vertex_attribute_t* in_vertex_attribute);

// Deletes the vertex array in inout_vertex_array. The backend reads in_num
//  names from inout_vertex_array->vertex_array on, so anything but 1 overruns
//  it
//
//  in:     in_num
//  in/out: inout_vertex_array
void vertex_array_internal_platform_destroy(
  struct vertex_array_t* inout_vertex_array,
  int in_num);
