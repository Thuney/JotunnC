#pragma once

#include "fvector.h"

#include <stdbool.h>
#include <stdint.h>

enum render_api_element_data_type_t
{
  ELEMENT_UNSIGNED_BYTE = 0x00,
  ELEMENT_UNSIGNED_SHORT,
  ELEMENT_UNSIGNED_INT
};

enum render_api_draw_mode_t
{
  DRAW_TYPE_POINTS = 0x00,
  DRAW_TYPE_LINE_STRIP,
  DRAW_TYPE_LINE_LOOP,
  DRAW_TYPE_LINES,
  DRAW_TYPE_LINE_STRIP_ADJACENCY,
  DRAW_TYPE_LINES_ADJACENCY,
  DRAW_TYPE_TRIANGLE_STRIP,
  DRAW_TYPE_TRIANGLE_FAN,
  DRAW_TYPE_TRIANGLES,
  DRAW_TYPE_TRIANGLE_STRIP_ADJACENCY,
  DRAW_TYPE_TRIANGLES_ADJACENCY,
  DRAW_TYPE_PATCHES
};

enum render_api_render_flags_t
{
  COLOR_FLAG = 0x0001,
  DEPTH_FLAG = 0x0002,
  BLEND_FLAG = 0x0004,
  WIREFRAME_FLAG = 0x0008
};

// How a source fragment's color is combined with the color already in the
//  render target when BLEND_FLAG is active
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

struct render_api_implementation_t
{
  uint16_t active_render_flags;
  enum render_api_blend_mode_t blend_mode;
  fvector4 clear_color;
  struct render_api_scissor_t scissor;
};

void render_api_init(
  struct render_api_implementation_t* render_api,
  const uint16_t render_flags);
void render_api_bind(struct render_api_implementation_t* render_api);

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
  const fvector4 in_color);
void render_api_clear(struct render_api_implementation_t* render_api);

// Restricts drawing to a rectangle of the render target, in pixels with the
//  origin at the bottom-left corner, until render_api_disable_scissor()
//
//  in:     in_x_pixels, in_y_pixels, in_width_pixels, in_height_pixels
//  in/out: inout_render_api
void render_api_set_scissor(
  struct render_api_implementation_t* inout_render_api,
  const int32_t in_x_pixels,
  const int32_t in_y_pixels,
  const int32_t in_width_pixels,
  const int32_t in_height_pixels);

// Lifts the restriction set by render_api_set_scissor()
//
//  in/out: inout_render_api
void render_api_disable_scissor(
  struct render_api_implementation_t* inout_render_api);

void render_api_draw_elements(
  enum render_api_draw_mode_t draw_mode,
  unsigned int element_count,
  const enum render_api_element_data_type_t element_data_type,
  const void* indices);
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
void render_api_draw_triangle_fan(unsigned int vertex_count);
void render_api_draw_lines(unsigned int vertex_count);

void render_api_set_line_width(const float width);

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
