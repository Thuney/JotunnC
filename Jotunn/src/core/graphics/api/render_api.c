#include "render_api.h"
#include "render_api_internal.h"

#include "../../diagnostics/diagnostics_internal.h"

/*******************************************************************************
 *
 *   Function: render_api_init()
 *
 ******************************************************************************/
void render_api_init(
  struct render_api_implementation_t* out_render_api,
  uint16_t in_render_flags)
{
  out_render_api->active_render_flags = in_render_flags;
  out_render_api->blend_mode = RENDER_API_BLEND_MODE_ALPHA;
  fvector4_init(&(out_render_api->clear_color));
  out_render_api->scissor =
    (struct render_api_scissor_t){
      .is_enabled = false};

  render_api_internal_platform_init(out_render_api);
}

/*******************************************************************************
 *
 *   Function: render_api_bind()
 *
 ******************************************************************************/
void render_api_bind(
  const struct render_api_implementation_t* in_render_api)
{
  render_api_internal_platform_bind(in_render_api);
}

/*******************************************************************************
 *
 *   Function: render_api_set_render_flags()
 *
 ******************************************************************************/
void render_api_set_render_flags(
  struct render_api_implementation_t* inout_render_api,
  uint16_t in_render_flags)
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
  enum render_api_blend_mode_t in_blend_mode)
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
  union fvector4_t in_color)
{
  inout_render_api->clear_color = in_color;
}

/*******************************************************************************
 *
 *   Function: render_api_clear()
 *
 ******************************************************************************/
void render_api_clear(
  const struct render_api_implementation_t* in_render_api)
{
  render_api_internal_platform_clear(in_render_api);
}

/*******************************************************************************
 *
 *   Function: render_api_draw_elements()
 *
 ******************************************************************************/
void render_api_draw_elements(
  enum render_api_draw_mode_t in_draw_mode,
  unsigned int in_element_count,
  enum render_api_element_data_type_t in_element_data_type,
  const void* in_indices)
{
  render_api_internal_platform_draw_elements(
    in_draw_mode,
    in_element_count,
    in_element_data_type,
    in_indices);

  diagnostics_internal_count_draw(in_element_count);
}

/*******************************************************************************
 *
 *   Function: render_api_set_scissor()
 *
 ******************************************************************************/
void render_api_set_scissor(
  struct render_api_implementation_t* inout_render_api,
  int32_t in_x_pixels,
  int32_t in_y_pixels,
  int32_t in_width_pixels,
  int32_t in_height_pixels)
{
  inout_render_api->scissor =
    (struct render_api_scissor_t){
      .is_enabled = true,
      .x_pixels = in_x_pixels,
      .y_pixels = in_y_pixels,
      .width_pixels = in_width_pixels,
      .height_pixels = in_height_pixels};

  render_api_internal_platform_apply_scissor(inout_render_api);
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

  render_api_internal_platform_apply_scissor(inout_render_api);
}

/*******************************************************************************
 *
 *   Function: render_api_draw_elements_base_vertex()
 *
 ******************************************************************************/
void render_api_draw_elements_base_vertex(
  enum render_api_draw_mode_t in_draw_mode,
  uint32_t in_element_count,
  enum render_api_element_data_type_t in_element_data_type,
  uintptr_t in_index_offset_bytes,
  int32_t in_base_vertex)
{
  render_api_internal_platform_draw_elements_base_vertex(
    in_draw_mode,
    in_element_count,
    in_element_data_type,
    in_index_offset_bytes,
    in_base_vertex);

  diagnostics_internal_count_draw(in_element_count);
}

/*******************************************************************************
 *
 *   Function: render_api_draw_triangle_fan()
 *
 ******************************************************************************/
void render_api_draw_triangle_fan(
  unsigned int in_vertex_count)
{
  render_api_internal_platform_draw_triangle_fan(in_vertex_count);

  diagnostics_internal_count_draw(in_vertex_count);
}

/*******************************************************************************
 *
 *   Function: render_api_draw_lines()
 *
 ******************************************************************************/
void render_api_draw_lines(
  unsigned int in_vertex_count)
{
  render_api_internal_platform_draw_lines(in_vertex_count);

  diagnostics_internal_count_draw(in_vertex_count);
}

/*******************************************************************************
 *
 *   Function: render_api_set_line_width()
 *
 ******************************************************************************/
void render_api_set_line_width(
  float in_line_width_pixels)
{
  render_api_internal_platform_set_line_width(in_line_width_pixels);
}

/*******************************************************************************
 *
 *   Function: render_api_has_timestamps()
 *
 ******************************************************************************/
bool render_api_has_timestamps(void)
{
  return render_api_internal_platform_has_timestamps();
}

/*******************************************************************************
 *
 *   Function: render_api_create_timestamps()
 *
 ******************************************************************************/
void render_api_create_timestamps(
  uint32_t* out_names,
  uint32_t in_count)
{
  render_api_internal_platform_create_timestamps(
    out_names,
    in_count);
}

/*******************************************************************************
 *
 *   Function: render_api_delete_timestamps()
 *
 ******************************************************************************/
void render_api_delete_timestamps(
  const uint32_t* in_names,
  uint32_t in_count)
{
  render_api_internal_platform_delete_timestamps(
    in_names,
    in_count);
}

/*******************************************************************************
 *
 *   Function: render_api_record_timestamp()
 *
 ******************************************************************************/
void render_api_record_timestamp(
  uint32_t in_name)
{
  render_api_internal_platform_record_timestamp(in_name);
}

/*******************************************************************************
 *
 *   Function: render_api_read_timestamp()
 *
 ******************************************************************************/
bool render_api_read_timestamp(
  uint32_t in_name,
  uint64_t* out_time_nanoseconds)
{
  return render_api_internal_platform_read_timestamp(
    in_name,
    out_time_nanoseconds);
}
