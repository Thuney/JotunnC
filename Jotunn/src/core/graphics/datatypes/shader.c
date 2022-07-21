#include "shader.h"

extern int platform_vertex_shader_init(struct vertex_shader_t* shader, const char* vertex_shader_src);
extern int platform_fragment_shader_init(struct fragment_shader_t* shader, const char* fragment_shader_src);
extern int platform_shader_program_init(struct shader_program_t* shader_program, struct vertex_shader_t* vertex_shader, struct fragment_shader_t* fragment_shader);

int vertex_shader_init(struct vertex_shader_t* shader, const char* vertex_shader_src)
{
   platform_vertex_shader_init(shader, vertex_shader_src);
}

int fragment_shader_init(struct fragment_shader_t* shader, const char* fragment_shader_src)
{
   platform_fragment_shader_init(shader, fragment_shader_src);
}

int shader_program_init(struct shader_program_t* shader_program, struct vertex_shader_t* vertex_shader, struct fragment_shader_t* fragment_shader)
{
   platform_shader_program_init(shader_program, vertex_shader, fragment_shader);
}