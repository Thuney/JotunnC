#pragma once

// The graphics API that renderers draw through. An implementation holds the
//  render state a renderer wants (flags, blend mode, clear color, scissor),
//  applied to the current graphics context when it's bound; the draw calls
//  act on whatever buffers, shaders and textures are bound at the time. The
//  backend is chosen when Jotunn is built

#include "fvector.h"

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// The type of each index in an element buffer
enum render_api_element_data_type_t
{
  RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_BYTE = 0x00,
  RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_SHORT,
  RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_INT
};

// The primitives a draw call assembles its vertices into
enum render_api_draw_mode_t
{
  RENDER_API_DRAW_MODE_POINTS = 0x00,
  RENDER_API_DRAW_MODE_LINE_STRIP,
  RENDER_API_DRAW_MODE_LINE_LOOP,
  RENDER_API_DRAW_MODE_LINES,
  RENDER_API_DRAW_MODE_LINE_STRIP_ADJACENCY,
  RENDER_API_DRAW_MODE_LINES_ADJACENCY,
  RENDER_API_DRAW_MODE_TRIANGLE_STRIP,
  RENDER_API_DRAW_MODE_TRIANGLE_FAN,
  RENDER_API_DRAW_MODE_TRIANGLES,
  RENDER_API_DRAW_MODE_TRIANGLE_STRIP_ADJACENCY,
  RENDER_API_DRAW_MODE_TRIANGLES_ADJACENCY,
  RENDER_API_DRAW_MODE_PATCHES
};

// Render state an implementation turns on, combined as a bitmask
enum render_api_render_flags_t
{
  RENDER_API_RENDER_FLAGS_COLOR = 0x0001,
  RENDER_API_RENDER_FLAGS_DEPTH = 0x0002,
  RENDER_API_RENDER_FLAGS_BLEND = 0x0004,
  RENDER_API_RENDER_FLAGS_WIREFRAME = 0x0008
};

// How a source fragment's color is combined with the color already in the
//  render target when RENDER_API_RENDER_FLAGS_BLEND is active
enum render_api_blend_mode_t
{
  // Source color is straight (not premultiplied); alpha accumulates as
  //  coverage, so a target cleared to transparent black ends up holding
  //  premultiplied color
  RENDER_API_BLEND_MODE_ALPHA = 0x00,
  // Source color is already premultiplied by its alpha, such as the
  //  contents of a render target drawn with RENDER_API_BLEND_MODE_ALPHA
  RENDER_API_BLEND_MODE_PREMULTIPLIED_ALPHA
};

// A rectangle of the render target that drawing is restricted to while
//  is_enabled is set, in pixels with the origin at the bottom-left corner
struct render_api_scissor_t
{
  bool is_enabled;
  int32_t x_pixels;
  int32_t y_pixels;
  int32_t width_pixels;
  int32_t height_pixels;
};

// The render state a renderer draws with; see render_api_init() for its
//  defaults
struct render_api_implementation_t
{
  uint16_t active_render_flags;
  enum render_api_blend_mode_t blend_mode;
  union fvector4_t clear_color;
  struct render_api_scissor_t scissor;
};

/*******************************************************************************
 *
 *   RENDER STATE
 *
 ******************************************************************************/

// Sets up an implementation with the given render flags, alpha blending, a
//  transparent black clear color and no scissor, and prepares the current
//  graphics context for it
//
//  in:     in_render_flags
//  out:    out_render_api
void render_api_init(
  struct render_api_implementation_t* out_render_api,
  const uint16_t in_render_flags);

// Applies an implementation's blend, depth and wireframe flags, blend mode and
//  scissor to the current graphics context
//
//  in:     in_render_api
void render_api_bind(
  const struct render_api_implementation_t* in_render_api);

// Replaces the render flags applied the next time the implementation is bound
//
//  in:     in_render_flags
//  in/out: inout_render_api
void render_api_set_render_flags(
  struct render_api_implementation_t* inout_render_api,
  const uint16_t in_render_flags);

// Sets the blend mode applied the next time the implementation is bound
//
//  in:     in_blend_mode
//  in/out: inout_render_api
void render_api_set_blend_mode(
  struct render_api_implementation_t* inout_render_api,
  const enum render_api_blend_mode_t in_blend_mode);

// Sets the color the implementation clears its render target to; defaults to
//  transparent black
//
//  in:     in_color
//  in/out: inout_render_api
void render_api_set_clear_color(
  struct render_api_implementation_t* inout_render_api,
  const union fvector4_t in_color);

// Clears the render target to the implementation's clear color, clearing the
//  color buffer when RENDER_API_RENDER_FLAGS_COLOR is set and the depth
//  buffer when RENDER_API_RENDER_FLAGS_DEPTH is set
//
//  in:     in_render_api
void render_api_clear(
  const struct render_api_implementation_t* in_render_api);

// Restricts drawing to a rectangle of the render target, in pixels with the
//  origin at the bottom-left corner, until render_api_disable_scissor(). It
//  takes effect at once, and again each time the implementation is bound
//
//  in:     in_x_pixels, in_y_pixels, in_width_pixels, in_height_pixels
//  in/out: inout_render_api
void render_api_set_scissor(
  struct render_api_implementation_t* inout_render_api,
  const int32_t in_x_pixels,
  const int32_t in_y_pixels,
  const int32_t in_width_pixels,
  const int32_t in_height_pixels);

// Lifts the restriction set by render_api_set_scissor(), at once
//
//  in/out: inout_render_api
void render_api_disable_scissor(
  struct render_api_implementation_t* inout_render_api);

/*******************************************************************************
 *
 *   DRAWING
 *
 ******************************************************************************/

// Each draw call is counted, for diagnostics, against the renderer whose scene
//  is open

// Draws indexed geometry. in_indices points to the indices, or is a byte
//  offset into the bound element buffer when one is bound
//
//  in:     in_draw_mode, in_element_count, in_element_data_type, in_indices
void render_api_draw_elements(
  const enum render_api_draw_mode_t in_draw_mode,
  const unsigned int in_element_count,
  const enum render_api_element_data_type_t in_element_data_type,
  const void* in_indices);

// Draws indexed geometry from the bound element buffer, adding
//  in_base_vertex to every index before it selects a vertex
//
//  in:     in_draw_mode, in_element_count, in_element_data_type,
//            in_index_offset_bytes, in_base_vertex
void render_api_draw_elements_base_vertex(
  const enum render_api_draw_mode_t in_draw_mode,
  const uint32_t in_element_count,
  const enum render_api_element_data_type_t in_element_data_type,
  const uintptr_t in_index_offset_bytes,
  const int32_t in_base_vertex);

// Draws the first in_vertex_count vertices of the bound vertex array as a
//  triangle fan
//
//  in:     in_vertex_count
void render_api_draw_triangle_fan(
  const unsigned int in_vertex_count);

// Draws the first in_vertex_count vertices of the bound vertex array as
//  separate lines, one per pair
//
//  in:     in_vertex_count
void render_api_draw_lines(
  const unsigned int in_vertex_count);

// Sets the width lines are drawn with, in the current graphics context
//
//  in:     in_line_width_pixels
void render_api_set_line_width(
  const float in_line_width_pixels);

/*******************************************************************************
 *
 *   GPU TIMESTAMPS
 *
 ******************************************************************************/

// Timestamps mark when the GPU reaches a point in the commands submitted to
//  it; the difference between two is how long the GPU spent on the commands
//  between them. Their names belong to the graphics context current when
//  they were created, which must be current to use them

// Checks for timestamp support in the current graphics context
//
//  ret:    true when timestamps are supported; otherwise false
bool render_api_has_timestamps(void);

// Creates timestamps
//
//  in:     in_count
//  out:    out_names
void render_api_create_timestamps(
  uint32_t* out_names,
  const uint32_t in_count);

// Deletes timestamps
//
//  in:     in_names, in_count
void render_api_delete_timestamps(
  const uint32_t* in_names,
  const uint32_t in_count);

// Has the GPU record the time when it reaches this point in its commands
//
//  in:     in_name
void render_api_record_timestamp(
  const uint32_t in_name);

// Reads a recorded timestamp, without waiting for the GPU
//
//  in:     in_name
//  out:    out_time_nanoseconds
//  ret:    true when the GPU has reached the timestamp; otherwise false, with
//            out_time_nanoseconds untouched
bool render_api_read_timestamp(
  const uint32_t in_name,
  uint64_t* out_time_nanoseconds);
