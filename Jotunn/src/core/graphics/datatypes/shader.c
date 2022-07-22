#include "shader.h"

extern int platform_vertex_shader_init(struct vertex_shader_t* shader, const char* vertex_shader_src);
extern int platform_fragment_shader_init(struct fragment_shader_t* shader, const char* fragment_shader_src);
extern int platform_shader_program_init(struct shader_program_t* shader_program, struct vertex_shader_t* vertex_shader, struct fragment_shader_t* fragment_shader);

extern int platform_shader_program_link(struct shader_program_t* shader_program);

extern void platform_shader_program_use(struct shader_program_t* shader_program);
extern void platform_shader_program_bind_fragment_data_location(struct shader_program_t* shader_program, unsigned int buffer_index, const char* var_name);
extern int platform_shader_program_get_attribute_location(struct shader_program_t* shader_program, const char* attribute_name);

extern void platform_vertex_shader_destroy(struct vertex_shader_t* shader);
extern void platform_fragment_shader_destroy(struct fragment_shader_t* shader);
extern void platform_shader_program_destroy(struct shader_program_t* shader_program);

int vertex_shader_init(struct vertex_shader_t* shader, const char* vertex_shader_src)
{
   return platform_vertex_shader_init(shader, vertex_shader_src);
}

int fragment_shader_init(struct fragment_shader_t* shader, const char* fragment_shader_src)
{
   return platform_fragment_shader_init(shader, fragment_shader_src);
}

int shader_program_init(struct shader_program_t* shader_program, struct vertex_shader_t* vertex_shader, struct fragment_shader_t* fragment_shader)
{
   return platform_shader_program_init(shader_program, vertex_shader, fragment_shader);
}

int shader_program_link(struct shader_program_t* shader_program)
{
   return platform_shader_program_link(shader_program);
}

void shader_program_use(struct shader_program_t* shader_program)
{
   platform_shader_program_use(shader_program);
}

void shader_program_bind_fragment_data_location(struct shader_program_t* shader_program, unsigned int buffer_index, const char* var_name)
{
   platform_shader_program_bind_fragment_data_location(shader_program, buffer_index, var_name);
}

int shader_program_get_attribute_location(struct shader_program_t* shader_program, const char* attribute_name)
{
   return platform_shader_program_get_attribute_location(shader_program, attribute_name);
}

void vertex_shader_destroy(struct vertex_shader_t* shader)
{
   platform_vertex_shader_destroy(shader);
}

void fragment_shader_destroy(struct fragment_shader_t* shader)
{
   platform_fragment_shader_destroy(shader);
}

void shader_program_destroy(struct shader_program_t* shader_program)
{
   platform_shader_program_destroy(shader_program);
}