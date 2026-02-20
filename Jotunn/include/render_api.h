#pragma once

#include "fvector.h"

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
   COLOR_FLAG     = 0x0001,
   DEPTH_FLAG     = 0x0002,
   BLEND_FLAG     = 0x0004,
   WIREFRAME_FLAG = 0x0008
};

struct render_api_implementation_t
{
   uint16_t active_render_flags;
};

void render_api_init(struct render_api_implementation_t* render_api, const uint16_t render_flags);
void render_api_bind(struct render_api_implementation_t* render_api);


void render_api_set_clear_color(const fvector4 color);
void render_api_clear(struct render_api_implementation_t* render_api);

void render_api_draw_elements(enum render_api_draw_mode_t draw_mode, unsigned int element_count, const enum render_api_element_data_type_t element_data_type, const void* indices);
void render_api_draw_triangle_fan(unsigned int vertex_count);
void render_api_draw_lines(unsigned int vertex_count);

void render_api_set_line_width(const float width);