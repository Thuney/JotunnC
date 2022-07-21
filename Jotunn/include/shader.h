#pragma once

#include <stdint.h>

struct shader_program_t
{
   unsigned int shader_program;

   #ifdef DEBUG
      int link_status;
      char link_log[512];
   #endif
};

struct vertex_shader_t
{
   unsigned int vertex_shader;

   #ifdef DEBUG
      int compilation_status;
      char compilation_log[512];
   #endif
};

struct fragment_shader_t
{
   unsigned int fragment_shader;

   #ifdef DEBUG
      int compilation_status;
      char compilation_log[512];
   #endif
};

int vertex_shader_init(struct vertex_shader_t* shader, const char* vertex_shader_src);
int fragment_shader_init(struct fragment_shader_t* shader, const char* fragment_shader_src);
int shader_program_init(struct shader_program_t* shader_program, struct vertex_shader_t* vertex_shader, struct fragment_shader_t* fragment_shader);