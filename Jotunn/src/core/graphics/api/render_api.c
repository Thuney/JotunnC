#include "render_api.h"

extern void platform_render_api_init(
  struct render_api_implementation_t* render_api);
extern void platform_render_api_bind(
  struct render_api_implementation_t* render_api);
extern void platform_render_api_set_clear_color(
  const fvector4 color);
extern void platform_render_api_clear(
  struct render_api_implementation_t* render_api);

extern void platform_render_api_draw_elements(
  enum render_api_draw_mode_t draw_mode,
  unsigned int element_count,
  const enum render_api_element_data_type_t element_data_type,
  const void* indices);
extern void platform_render_api_draw_triangle_fan(
  unsigned int vertex_count);
extern void platform_render_api_draw_lines(
  unsigned int vertex_count);

extern void platform_render_api_set_line_width(
  const float width);

//

/*******************************************************************************
 *
 * Function: render_api_init()
 *
 ******************************************************************************/
void render_api_init(
  struct render_api_implementation_t* render_api,
  const uint16_t render_flags)
{
  render_api->active_render_flags = render_flags;

  platform_render_api_init(render_api);
}

/*******************************************************************************
 *
 * Function: render_api_bind()
 *
 ******************************************************************************/
void render_api_bind(
  struct render_api_implementation_t* render_api)
{
  platform_render_api_bind(render_api);
}

/*******************************************************************************
 *
 * Function: render_api_set_clear_color()
 *
 ******************************************************************************/
void render_api_set_clear_color(
  const fvector4 color)
{
  platform_render_api_set_clear_color(color);
}

/*******************************************************************************
 *
 * Function: render_api_clear()
 *
 ******************************************************************************/
void render_api_clear(
  struct render_api_implementation_t* render_api)
{
  platform_render_api_clear(render_api);
}

/*******************************************************************************
 *
 * Function: render_api_draw_elements()
 *
 ******************************************************************************/
void render_api_draw_elements(
  enum render_api_draw_mode_t draw_mode,
  unsigned int element_count,
  const enum render_api_element_data_type_t element_data_type,
  const void* indices)
{
  platform_render_api_draw_elements(
    draw_mode,
    element_count,
    element_data_type,
    indices);
}

/*******************************************************************************
 *
 * Function: render_api_draw_triangle_fan()
 *
 ******************************************************************************/
void render_api_draw_triangle_fan(
  unsigned int vertex_count)
{
  platform_render_api_draw_triangle_fan(vertex_count);
}

/*******************************************************************************
 *
 * Function: render_api_draw_lines()
 *
 ******************************************************************************/
void render_api_draw_lines(
  unsigned int vertex_count)
{
  platform_render_api_draw_lines(vertex_count);
}

/*******************************************************************************
 *
 * Function: render_api_set_line_width()
 *
 ******************************************************************************/
void render_api_set_line_width(
  const float width)
{
  platform_render_api_set_line_width(width);
}
