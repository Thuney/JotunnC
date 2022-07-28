#include "shader.h"

int platform_vertex_shader_init(struct vertex_shader_t* shader, const char* vertex_shader_src)
{
   return 0;
}

int platform_fragment_shader_init(struct fragment_shader_t* shader, const char* fragment_shader_src)
{
   return 0;
}

int platform_shader_program_init(struct shader_program_t* shader_program, struct vertex_shader_t* vertex_shader, struct fragment_shader_t* fragment_shader)
{
   return 0;
}

int platform_shader_program_link(struct shader_program_t* shader_program)
{
   return 0;
}

void platform_shader_program_use(struct shader_program_t* shader_program)
{
   return 0;
}

void platform_shader_program_bind_fragment_data_location(struct shader_program_t* shader_program, unsigned int buffer_index, const char* var_name)
{
   return 0;
}

int platform_shader_program_get_attribute_location(struct shader_program_t* shader_program, const char* attribute_name)
{
   return 0;
}

void platform_shader_program_set_uniform(struct shader_program_t* shader_program, const char* uniform_name, void* uniform_data, const enum shader_program_uniform_t uniform_type)
{
   return 0;  
}

void platform_vertex_shader_destroy(struct vertex_shader_t* shader)
{
   return 0;
}

void platform_fragment_shader_destroy(struct fragment_shader_t* shader)
{
   return 0;
}

void platform_shader_program_destroy(struct shader_program_t* shader_program)
{
   return 0;
}