#include "render_api.h"

void platform_render_api_init(struct render_api_implementation_t* render_api)
{
   return 0;
}

void platform_render_api_bind(struct render_api_implementation_t* render_api)
{
   return 0;
}

void platform_render_api_set_clear_color(const fvector4 color)
{
   return 0;
}

void platform_render_api_clear(struct render_api_implementation_t* render_api)
{
   return 0;
}

void platform_render_api_draw_elements(enum render_api_draw_mode_t draw_mode, unsigned int element_count, const enum render_api_element_data_type_t element_data_type, const void* indices)
{
   return 0;
}

void platform_render_api_draw_triangle_fan(unsigned int vertex_count)
{
   return 0;
}

void platform_render_api_draw_lines(unsigned int vertex_count)
{
   return 0;
}

void platform_render_api_set_line_width(float line_width)
{
   return 0;
}