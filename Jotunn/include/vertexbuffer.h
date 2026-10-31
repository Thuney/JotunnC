#pragma once

// Vertex buffers, which hold vertex data, and element buffers, which hold the
//  indices of the vertices to draw. Each is registered as a resource from its
//  init function until its destroy function, and its size is tracked as data
//  is set

#include "resource.h"

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// How a buffer's data will be used, as a hint to the backend
enum vertex_buffer_data_usage_t
{
  VERTEX_BUFFER_DATA_USAGE_STREAM_DRAW = 0x00,
  VERTEX_BUFFER_DATA_USAGE_STREAM_READ,
  VERTEX_BUFFER_DATA_USAGE_STREAM_COPY,
  VERTEX_BUFFER_DATA_USAGE_STATIC_DRAW,
  VERTEX_BUFFER_DATA_USAGE_STATIC_READ,
  VERTEX_BUFFER_DATA_USAGE_STATIC_COPY,
  VERTEX_BUFFER_DATA_USAGE_DYNAMIC_DRAW,
  VERTEX_BUFFER_DATA_USAGE_DYNAMIC_READ,
  VERTEX_BUFFER_DATA_USAGE_DYNAMIC_COPY
};

struct vertex_buffer_t
{
  unsigned int vertex_buffer;
  // Its entry in the resource registry
  struct resource_t resource;
};

/*******************************************************************************
 *
 *   VERTEX BUFFERS
 *
 ******************************************************************************/

// Creates in_num vertex buffers, one in each element of the array
//  inout_vertex_buffer points to, and registers each as a resource holding
//  no data
//
//  in:     in_num
//  in/out: inout_vertex_buffer
void vertex_buffer_init(
  struct vertex_buffer_t* inout_vertex_buffer,
  int in_num);

// Makes a vertex buffer the one vertex data is set in and read from
//
//  in:     in_vertex_buffer
void vertex_buffer_bind(
  const struct vertex_buffer_t* in_vertex_buffer);

// Replaces the data of the bound vertex buffer, which must be
//  inout_vertex_buffer, with a copy of in_data_bytes from in_data. A NULL
//  in_data leaves the new storage uninitialized
//
//  in:     in_data, in_data_bytes, in_usage_type
//  in/out: inout_vertex_buffer
void vertex_buffer_set_data(
  struct vertex_buffer_t* inout_vertex_buffer,
  const void* in_data,
  unsigned int in_data_bytes,
  enum vertex_buffer_data_usage_t in_usage_type);

// Overwrites the start of the bound vertex buffer's data, which must be
//  in_vertex_buffer's, with in_data_bytes from in_data. The buffer must
//  already hold at least that many bytes
//
//  in:     in_vertex_buffer, in_data, in_data_bytes
void vertex_buffer_set_sub_data(
  const struct vertex_buffer_t* in_vertex_buffer,
  const float* in_data,
  unsigned int in_data_bytes);

// Deletes in_num vertex buffers, one in each element of the array
//  inout_vertex_buffer points to, and removes each from the resource registry
//
//  in:     in_num
//  in/out: inout_vertex_buffer
void vertex_buffer_destroy(
  struct vertex_buffer_t* inout_vertex_buffer,
  int in_num);

/*******************************************************************************
 *
 *   ELEMENT BUFFERS
 *
 ******************************************************************************/

struct element_buffer_t
{
  unsigned int element_buffer;
  // Its entry in the resource registry
  struct resource_t resource;
};

// Creates in_num element buffers, one in each element of the array
//  inout_element_buffer points to, and registers each as a resource holding
//  no data
//
//  in:     in_num
//  in/out: inout_element_buffer
//  ret:    0, as creating buffers can't fail
int element_buffer_init(
  struct element_buffer_t* inout_element_buffer,
  int in_num);

// Makes an element buffer the one indices are set in and read from. The
//  binding is recorded in the bound vertex array
//
//  in:     in_element_buffer
void element_buffer_bind(
  const struct element_buffer_t* in_element_buffer);

// Replaces the indices of the bound element buffer, which must be
//  inout_element_buffer, with a copy of in_data_bytes from in_index_data. A
//  NULL in_index_data leaves the new storage uninitialized
//
//  in:     in_index_data, in_data_bytes, in_usage_type
//  in/out: inout_element_buffer
void element_buffer_buffer_data(
  struct element_buffer_t* inout_element_buffer,
  const void* in_index_data,
  unsigned int in_data_bytes,
  enum vertex_buffer_data_usage_t in_usage_type);

// Deletes in_num element buffers, one in each element of the array
//  inout_element_buffer points to, and removes each from the resource
//  registry
//
//  in:     in_num
//  in/out: inout_element_buffer
void element_buffer_destroy(
  struct element_buffer_t* inout_element_buffer,
  int in_num);
