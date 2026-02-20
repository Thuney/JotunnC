#include "render_api.h"

extern void platform_render_api_init(struct render_api_implementation_t* render_api);
extern void platform_render_api_bind(struct render_api_implementation_t* render_api);
extern void platform_render_api_set_clear_color(const fvector4 color);
extern void platform_render_api_clear(struct render_api_implementation_t* render_api);

extern void platform_render_api_draw_elements(enum render_api_draw_mode_t draw_mode, unsigned int element_count, const enum render_api_element_data_type_t element_data_type, const void* indices);
extern void platform_render_api_draw_triangle_fan(unsigned int vertex_count);
extern void platform_render_api_draw_lines(unsigned int vertex_count);

extern void platform_render_api_set_line_width(const float width);

//

void render_api_init(struct render_api_implementation_t* render_api, const uint16_t render_flags)
{
   render_api->active_render_flags = render_flags;

   platform_render_api_init(render_api);
}

void render_api_bind(struct render_api_implementation_t* render_api)
{
   platform_render_api_bind(render_api);
}

void render_api_set_clear_color(const fvector4 color)
{
   platform_render_api_set_clear_color(color);
}

void render_api_clear(struct render_api_implementation_t* render_api)
{
   platform_render_api_clear(render_api);
}

void render_api_draw_elements(enum render_api_draw_mode_t draw_mode, unsigned int element_count, const enum render_api_element_data_type_t element_data_type, const void* indices)
{
   platform_render_api_draw_elements(draw_mode, element_count, element_data_type, indices);
}

void render_api_draw_triangle_fan(unsigned int vertex_count)
{
   platform_render_api_draw_triangle_fan(vertex_count);
}

void render_api_draw_lines(unsigned int vertex_count)
{
   platform_render_api_draw_lines(vertex_count);
}

void render_api_set_line_width(const float width)
{
   platform_render_api_set_line_width(width);
}