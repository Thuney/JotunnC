#include "render_api.h"

extern void platform_render_api_init(struct render_api_implementation_t* render_api);
extern void platform_render_api_bind(struct render_api_implementation_t* render_api);
extern void platform_render_api_clear(struct render_api_implementation_t* render_api);
// Restricts drawing to the implementation's scissor rectangle when it is
//  enabled, and lifts the restriction otherwise
//
//  in:     in_render_api
extern void platform_render_api_apply_scissor(
  const struct render_api_implementation_t* in_render_api);

extern void platform_render_api_draw_elements(
  enum render_api_draw_mode_t draw_mode,
  unsigned int element_count,
  const enum render_api_element_data_type_t element_data_type,
  const void* indices);
extern void platform_render_api_draw_elements_base_vertex(
  const enum render_api_draw_mode_t in_draw_mode,
  const uint32_t in_element_count,
  const enum render_api_element_data_type_t in_element_data_type,
  const uintptr_t in_index_offset_bytes,
  const int32_t in_base_vertex);
extern void platform_render_api_draw_triangle_fan(unsigned int vertex_count);
extern void platform_render_api_draw_lines(unsigned int vertex_count);

extern void platform_render_api_set_line_width(const float width);

//

void render_api_init(
  struct render_api_implementation_t* render_api,
  const uint16_t render_flags)
{
  render_api->active_render_flags = render_flags;
  render_api->blend_mode = RENDER_API_BLEND_MODE_ALPHA;
  fvector4_init(&(render_api->clear_color));
  render_api->scissor =
    (struct render_api_scissor_t){
      .is_enabled = false};

  platform_render_api_init(render_api);
}

void render_api_bind(
  struct render_api_implementation_t* render_api)
{
  platform_render_api_bind(render_api);
}

/*******************************************************************************
 *
 *   Function: render_api_set_render_flags()
 *
 ******************************************************************************/
void render_api_set_render_flags(
  struct render_api_implementation_t* inout_render_api,
  const uint16_t in_render_flags)
{
  inout_render_api->active_render_flags = in_render_flags;
}

/*******************************************************************************
 *
 *   Function: render_api_set_blend_mode()
 *
 ******************************************************************************/
void render_api_set_blend_mode(
  struct render_api_implementation_t* inout_render_api,
  const enum render_api_blend_mode_t in_blend_mode)
{
  inout_render_api->blend_mode = in_blend_mode;
}

/*******************************************************************************
 *
 *   Function: render_api_set_clear_color()
 *
 ******************************************************************************/
void render_api_set_clear_color(
  struct render_api_implementation_t* inout_render_api,
  const fvector4 in_color)
{
  inout_render_api->clear_color = in_color;
}

void render_api_clear(
  struct render_api_implementation_t* render_api)
{
  platform_render_api_clear(render_api);
}

void render_api_draw_elements(
  enum render_api_draw_mode_t draw_mode,
  unsigned int element_count,
  const enum render_api_element_data_type_t element_data_type,
  const void* indices)
{
  platform_render_api_draw_elements(
    draw_mode, element_count, element_data_type, indices);
}

/*******************************************************************************
 *
 *   Function: render_api_set_scissor()
 *
 ******************************************************************************/
void render_api_set_scissor(
  struct render_api_implementation_t* inout_render_api,
  const int32_t in_x_pixels,
  const int32_t in_y_pixels,
  const int32_t in_width_pixels,
  const int32_t in_height_pixels)
{
  inout_render_api->scissor =
    (struct render_api_scissor_t){
      .is_enabled = true,
      .x_pixels = in_x_pixels,
      .y_pixels = in_y_pixels,
      .width_pixels = in_width_pixels,
      .height_pixels = in_height_pixels};

  platform_render_api_apply_scissor(inout_render_api);
}

/*******************************************************************************
 *
 *   Function: render_api_disable_scissor()
 *
 ******************************************************************************/
void render_api_disable_scissor(
  struct render_api_implementation_t* inout_render_api)
{
  inout_render_api->scissor.is_enabled = false;

  platform_render_api_apply_scissor(inout_render_api);
}

/*******************************************************************************
 *
 *   Function: render_api_draw_elements_base_vertex()
 *
 ******************************************************************************/
void render_api_draw_elements_base_vertex(
  const enum render_api_draw_mode_t in_draw_mode,
  const uint32_t in_element_count,
  const enum render_api_element_data_type_t in_element_data_type,
  const uintptr_t in_index_offset_bytes,
  const int32_t in_base_vertex)
{
  platform_render_api_draw_elements_base_vertex(
    in_draw_mode,
    in_element_count,
    in_element_data_type,
    in_index_offset_bytes,
    in_base_vertex);
}

void render_api_draw_triangle_fan(
  unsigned int vertex_count)
{
  platform_render_api_draw_triangle_fan(vertex_count);
}

void render_api_draw_lines(
  unsigned int vertex_count)
{
  platform_render_api_draw_lines(vertex_count);
}

void render_api_set_line_width(
  const float width)
{
  platform_render_api_set_line_width(width);
}
