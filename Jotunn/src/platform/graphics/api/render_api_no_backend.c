#include "render_api.h"
#include "../../../core/graphics/api/render_api_internal.h"

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_init()
 *
 ******************************************************************************/
void render_api_internal_platform_init(
  const struct render_api_implementation_t* in_render_api)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_bind()
 *
 ******************************************************************************/
void render_api_internal_platform_bind(
  const struct render_api_implementation_t* in_render_api)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_clear()
 *
 ******************************************************************************/
void render_api_internal_platform_clear(
  const struct render_api_implementation_t* in_render_api)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_draw_elements()
 *
 ******************************************************************************/
void render_api_internal_platform_draw_elements(
  enum render_api_draw_mode_t in_draw_mode,
  unsigned int in_element_count,
  enum render_api_element_data_type_t in_element_data_type,
  const void* in_indices)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_draw_triangle_fan()
 *
 ******************************************************************************/
void render_api_internal_platform_draw_triangle_fan(
  unsigned int in_vertex_count)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_draw_lines()
 *
 ******************************************************************************/
void render_api_internal_platform_draw_lines(
  unsigned int in_vertex_count)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_set_line_width()
 *
 ******************************************************************************/
void render_api_internal_platform_set_line_width(
  float in_line_width_pixels)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_apply_scissor()
 *
 ******************************************************************************/
void render_api_internal_platform_apply_scissor(
  const struct render_api_implementation_t* in_render_api)
{
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_draw_elements_base_vertex()
 *
 ******************************************************************************/
void render_api_internal_platform_draw_elements_base_vertex(
  enum render_api_draw_mode_t in_draw_mode,
  uint32_t in_element_count,
  enum render_api_element_data_type_t in_element_data_type,
  uintptr_t in_index_offset_bytes,
  int32_t in_base_vertex)
{
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_has_timestamps()
 *
 ******************************************************************************/
bool render_api_internal_platform_has_timestamps(void)
{
  return false;
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_create_timestamps()
 *
 ******************************************************************************/
void render_api_internal_platform_create_timestamps(
  uint32_t* out_names,
  uint32_t in_count)
{
  for (uint32_t i = 0; i < in_count; i++) {
    out_names[i] = 0;
  }
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_delete_timestamps()
 *
 ******************************************************************************/
void render_api_internal_platform_delete_timestamps(
  const uint32_t* in_names,
  uint32_t in_count)
{
  (void)in_names;
  (void)in_count;
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_record_timestamp()
 *
 ******************************************************************************/
void render_api_internal_platform_record_timestamp(
  uint32_t in_name)
{
  (void)in_name;
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_read_timestamp()
 *
 ******************************************************************************/
bool render_api_internal_platform_read_timestamp(
  uint32_t in_name,
  uint64_t* out_time_nanoseconds)
{
  (void)in_name;
  (void)out_time_nanoseconds;

  return false;
}
