#pragma once

// Private to Jotunn: the hooks render_api.c calls into the graphics backend.
//  Exactly one platform file defines them, chosen by the build: OpenGL in
//  render_api_opengl.c, or the stubs in render_api_no_backend.c. Each acts on
//  the graphics context current on the calling thread

#include "render_api.h"

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 *
 *   RENDER STATE
 *
 ******************************************************************************/

// Prepares the current graphics context for an implementation once its
//  fields are set
//
//  in:     in_render_api
void render_api_internal_platform_init(
  const struct render_api_implementation_t* in_render_api);

// Applies an implementation's blend, depth and wireframe flags, blend mode and
//  scissor to the current graphics context
//
//  in:     in_render_api
void render_api_internal_platform_bind(
  const struct render_api_implementation_t* in_render_api);

// Clears the render target to the implementation's clear color, clearing the
//  color buffer when RENDER_API_RENDER_FLAGS_COLOR is set and the depth
//  buffer when RENDER_API_RENDER_FLAGS_DEPTH is set
//
//  in:     in_render_api
void render_api_internal_platform_clear(
  const struct render_api_implementation_t* in_render_api);

// Restricts drawing to the implementation's scissor rectangle when it is
//  enabled, and lifts the restriction otherwise
//
//  in:     in_render_api
void render_api_internal_platform_apply_scissor(
  const struct render_api_implementation_t* in_render_api);

/*******************************************************************************
 *
 *   DRAWING
 *
 ******************************************************************************/

// Draws indexed geometry. in_indices points to the indices, or is a byte
//  offset into the bound element buffer when one is bound
//
//  in:     in_draw_mode, in_element_count, in_element_data_type, in_indices
void render_api_internal_platform_draw_elements(
  enum render_api_draw_mode_t in_draw_mode,
  unsigned int in_element_count,
  enum render_api_element_data_type_t in_element_data_type,
  const void* in_indices);

// Draws indexed geometry from the bound element buffer, adding
//  in_base_vertex to every index before it selects a vertex
//
//  in:     in_draw_mode, in_element_count, in_element_data_type,
//            in_index_offset_bytes, in_base_vertex
void render_api_internal_platform_draw_elements_base_vertex(
  enum render_api_draw_mode_t in_draw_mode,
  uint32_t in_element_count,
  enum render_api_element_data_type_t in_element_data_type,
  uintptr_t in_index_offset_bytes,
  int32_t in_base_vertex);

// Draws the first in_vertex_count vertices of the bound vertex array as a
//  triangle fan
//
//  in:     in_vertex_count
void render_api_internal_platform_draw_triangle_fan(
  unsigned int in_vertex_count);

// Draws the first in_vertex_count vertices of the bound vertex array as
//  separate lines, one per pair
//
//  in:     in_vertex_count
void render_api_internal_platform_draw_lines(
  unsigned int in_vertex_count);

// Sets the width lines are drawn with
//
//  in:     in_line_width_pixels
void render_api_internal_platform_set_line_width(
  float in_line_width_pixels);

/*******************************************************************************
 *
 *   GPU TIMESTAMPS
 *
 ******************************************************************************/

// Checks for timestamp support in the current graphics context
//
//  ret:    true when timestamps are supported; otherwise false
bool render_api_internal_platform_has_timestamps(void);

// Creates timestamps. Without a graphics backend every name is zero
//
//  in:     in_count
//  out:    out_names
void render_api_internal_platform_create_timestamps(
  uint32_t* out_names,
  uint32_t in_count);

// Deletes timestamps
//
//  in:     in_names, in_count
void render_api_internal_platform_delete_timestamps(
  const uint32_t* in_names,
  uint32_t in_count);

// Has the GPU record the time when it reaches this point in its commands
//
//  in:     in_name
void render_api_internal_platform_record_timestamp(
  uint32_t in_name);

// Reads a recorded timestamp, without waiting for the GPU
//
//  in:     in_name
//  out:    out_time_nanoseconds
//  ret:    true when the GPU has reached the timestamp; otherwise false, with
//            out_time_nanoseconds untouched
bool render_api_internal_platform_read_timestamp(
  uint32_t in_name,
  uint64_t* out_time_nanoseconds);
