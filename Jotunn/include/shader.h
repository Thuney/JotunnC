#pragma once

#include <stdint.h>

#include "fmatrix.h"
#include "vertexattribute.h"

enum shader_program_uniform_t
{
   SHADER_BOOL = 0x00,
   SHADER_INT,
   SHADER_FLOAT,
   SHADER_FVEC2,
   SHADER_FVEC3,
   SHADER_FVEC4,
   SHADER_FMAT2x2,
   SHADER_FMAT3x3,
   SHADER_FMAT4x4
};

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

int vertex_shader_init_filepath(struct vertex_shader_t* shader, const char* filepath);
int fragment_shader_init_filepath(struct fragment_shader_t* shader, const char* filepath);
int shader_program_init_filepath(struct shader_program_t* shader_program, const char* vertex_shader_filepath, const char* fragment_shader_filepath);

int shader_program_link(struct shader_program_t* shader_program);

void shader_program_use(struct shader_program_t* shader_program);

void shader_program_bind_fragment_data_location(struct shader_program_t* shader_program, unsigned int buffer_index, const char* var_name);
int shader_program_get_attribute_location(struct shader_program_t* shader_program, const char* attribute_name);

void shader_program_set_uniform_fmat4x4(struct shader_program_t* shader_program, const char* uniform_name, const fmatrix_4x4* matrix);
void shader_program_set_uniform_float(struct shader_program_t* shader_program, const char* uniform_name, const float* val);

void vertex_shader_destroy(struct vertex_shader_t* shader);
void fragment_shader_destroy(struct fragment_shader_t* shader);
void shader_program_destroy(struct shader_program_t* shader_program);