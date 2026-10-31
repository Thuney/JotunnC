#pragma once

// Private to Jotunn: the backend hooks behind vertexbuffer.h. Each rendering
//  backend defines these, and vertexbuffer.c calls them, handling the
//  resource registry itself

#include "vertexbuffer.h"

/*******************************************************************************
 *
 *   PLATFORM VERTEX BUFFERS
 *
 ******************************************************************************/

// Creates a vertex buffer in out_vertex_buffer. The backend writes in_num
//  names from out_vertex_buffer->vertex_buffer on, so anything but 1 overruns
//  it
//
//  in:     in_num
//  out:    out_vertex_buffer
void vertex_buffer_internal_platform_init(
  struct vertex_buffer_t* out_vertex_buffer,
  int in_num);

// Makes a vertex buffer the one vertex data is set in and read from
//
//  in:     in_vertex_buffer
void vertex_buffer_internal_platform_bind(
  const struct vertex_buffer_t* in_vertex_buffer);

// Replaces the data of the bound vertex buffer, which must be
//  in_vertex_buffer, with a copy of in_data_bytes from in_data, or with
//  uninitialized storage when in_data is NULL
//
//  in:     in_vertex_buffer, in_data, in_data_bytes, in_usage_type
void vertex_buffer_internal_platform_set_data(
  const struct vertex_buffer_t* in_vertex_buffer,
  const void* in_data,
  unsigned int in_data_bytes,
  enum vertex_buffer_data_usage_t in_usage_type);

// Overwrites the start of the bound vertex buffer's data, which must be
//  in_vertex_buffer's, with in_data_bytes from in_data
//
//  in:     in_vertex_buffer, in_data, in_data_bytes
void vertex_buffer_internal_platform_set_sub_data(
  const struct vertex_buffer_t* in_vertex_buffer,
  const float* in_data,
  unsigned int in_data_bytes);

// Deletes the vertex buffer in inout_vertex_buffer. The backend reads in_num
//  names from inout_vertex_buffer->vertex_buffer on, so anything but 1
//  overruns it
//
//  in:     in_num
//  in/out: inout_vertex_buffer
void vertex_buffer_internal_platform_destroy(
  struct vertex_buffer_t* inout_vertex_buffer,
  int in_num);

/*******************************************************************************
 *
 *   PLATFORM ELEMENT BUFFERS
 *
 ******************************************************************************/

// Creates an element buffer in out_element_buffer. The backend writes in_num
//  names from out_element_buffer->element_buffer on, so anything but 1
//  overruns it
//
//  in:     in_num
//  out:    out_element_buffer
void element_buffer_internal_platform_init(
  struct element_buffer_t* out_element_buffer,
  int in_num);

// Makes an element buffer the one indices are set in and read from
//
//  in:     in_element_buffer
void element_buffer_internal_platform_bind(
  const struct element_buffer_t* in_element_buffer);

// Replaces the indices of the bound element buffer, which must be
//  in_element_buffer, with a copy of in_data_bytes from in_index_data, or with
//  uninitialized storage when in_index_data is NULL
//
//  in:     in_element_buffer, in_index_data, in_data_bytes, in_usage_type
void element_buffer_internal_platform_buffer_data(
  const struct element_buffer_t* in_element_buffer,
  const void* in_index_data,
  unsigned int in_data_bytes,
  enum vertex_buffer_data_usage_t in_usage_type);

// Overwrites in_data_bytes of the bound element buffer's indices, which must
//  be in_element_buffer's, from in_offset_bytes on, with those in
//  in_index_data
//
//  in:     in_element_buffer, in_offset_bytes, in_index_data, in_data_bytes
void element_buffer_internal_platform_set_sub_data(
  const struct element_buffer_t* in_element_buffer,
  unsigned int in_offset_bytes,
  const void* in_index_data,
  unsigned int in_data_bytes);

// Deletes the element buffer in inout_element_buffer. The backend reads
//  in_num names from inout_element_buffer->element_buffer on, so anything but
//  1 overruns it
//
//  in:     in_num
//  in/out: inout_element_buffer
void element_buffer_internal_platform_destroy(
  struct element_buffer_t* inout_element_buffer,
  int in_num);
