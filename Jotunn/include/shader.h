#pragma once

// Vertex and fragment shaders, and the shader programs that link them. A
//  program is registered as a resource from shader_program_init() until
//  shader_program_destroy(); its shaders can be destroyed once it's linked.
//  DEBUG builds check compilation and linking, logging what went wrong

#include <stdint.h>

#include "fmatrix.h"
#include "resource.h"
#include "vertexattribute.h"

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

enum shader_program_uniform_t
{
  SHADER_PROGRAM_UNIFORM_BOOL = 0x00,
  SHADER_PROGRAM_UNIFORM_INT,
  SHADER_PROGRAM_UNIFORM_FLOAT,
  SHADER_PROGRAM_UNIFORM_FVEC2,
  SHADER_PROGRAM_UNIFORM_FVEC3,
  SHADER_PROGRAM_UNIFORM_FVEC4,
  SHADER_PROGRAM_UNIFORM_FMAT2X2,
  SHADER_PROGRAM_UNIFORM_FMAT3X3,
  SHADER_PROGRAM_UNIFORM_FMAT4X4
};

struct shader_program_t
{
  unsigned int shader_program;
  // Its entry in the resource registry
  struct resource_t resource;

#ifdef DEBUG
  int link_status;
  char link_log[512];
#endif
};

struct shader_vertex_t
{
  unsigned int vertex_shader;

#ifdef DEBUG
  int compilation_status;
  char compilation_log[512];
#endif
};

struct shader_fragment_t
{
  unsigned int fragment_shader;

#ifdef DEBUG
  int compilation_status;
  char compilation_log[512];
#endif
};

/*******************************************************************************
 *
 *   INITIALIZATION
 *
 ******************************************************************************/

// Creates a vertex shader and compiles it from source
//
//  in:     in_vertex_shader_src
//  out:    out_shader
//  ret:    0 when the shader compiled; 1 when it didn't. Only DEBUG builds
//            check, so other builds always return 0
int shader_vertex_init(
  struct shader_vertex_t* out_shader,
  const char* in_vertex_shader_src);

// Creates a fragment shader and compiles it from source
//
//  in:     in_fragment_shader_src
//  out:    out_shader
//  ret:    0 when the shader compiled; 1 when it didn't. Only DEBUG builds
//            check, so other builds always return 0
int shader_fragment_init(
  struct shader_fragment_t* out_shader,
  const char* in_fragment_shader_src);

// Creates a shader program with a vertex and a fragment shader attached, and
//  registers it as a resource. The program is registered even when it fails,
//  so it must be destroyed either way. Link it before use
//
//  in:     in_vertex_shader, in_fragment_shader
//  in/out: inout_shader_program
//  ret:    0 on success; 1 when the shaders couldn't be attached
int shader_program_init(
  struct shader_program_t* inout_shader_program,
  const struct shader_vertex_t* in_vertex_shader,
  const struct shader_fragment_t* in_fragment_shader);

// Creates a vertex shader and compiles it from the source in a file, which
//  must be readable
//
//  in:     in_filepath
//  out:    out_shader
//  ret:    0 when the shader compiled; 1 when it didn't. Only DEBUG builds
//            check, so other builds always return 0
int shader_vertex_init_filepath(
  struct shader_vertex_t* out_shader,
  const char* in_filepath);

// Creates a fragment shader and compiles it from the source in a file, which
//  must be readable
//
//  in:     in_filepath
//  out:    out_shader
//  ret:    0 when the shader compiled; 1 when it didn't. Only DEBUG builds
//            check, so other builds always return 0
int shader_fragment_init_filepath(
  struct shader_fragment_t* out_shader,
  const char* in_filepath);

// Builds a linked shader program from the vertex and fragment shader sources
//  in two files, binding fragment output "fColor" to buffer 0. The shaders
//  are destroyed once linked. A failing step is logged and the rest go ahead,
//  so the program is registered as a resource and must be destroyed either way
//
//  in:     in_vertex_shader_filepath, in_fragment_shader_filepath
//  in/out: inout_shader_program
//  ret:    0 when the program linked; 1 when it didn't. Only DEBUG builds
//            check linking, so other builds always return 0
int shader_program_init_filepath(
  struct shader_program_t* inout_shader_program,
  const char* in_vertex_shader_filepath,
  const char* in_fragment_shader_filepath);

// Links the shaders attached to a program
//
//  in/out: inout_shader_program
//  ret:    0 when the program linked; 1 when it didn't. Only DEBUG builds
//            check, so other builds always return 0
int shader_program_link(
  struct shader_program_t* inout_shader_program);

/*******************************************************************************
 *
 *   USE
 *
 ******************************************************************************/

// Makes a linked program the one used for drawing
//
//  in:     in_shader_program
void shader_program_use(
  const struct shader_program_t* in_shader_program);

// Binds a fragment shader output variable to a draw buffer. Takes effect when
//  the program is next linked
//
//  in:     in_shader_program, in_buffer_index, in_var_name
void shader_program_bind_fragment_data_location(
  const struct shader_program_t* in_shader_program,
  unsigned int in_buffer_index,
  const char* in_var_name);

// Finds the location of a vertex attribute in a linked program
//
//  in:     in_shader_program, in_attribute_name
//  ret:    The attribute's location; -1 when the program has no active
//            attribute of that name
int shader_program_get_attribute_location(
  const struct shader_program_t* in_shader_program,
  const char* in_attribute_name);

/*******************************************************************************
 *
 *   UNIFORMS
 *
 ******************************************************************************/

// Sets a 4x4 matrix uniform. The program must be the one in use
//
//  in:     in_shader_program, in_uniform_name, in_matrix
void shader_program_set_uniform_fmat4x4(
  const struct shader_program_t* in_shader_program,
  const char* in_uniform_name,
  const union fmatrix_4x4_t* in_matrix);

// Sets a float uniform. The program must be the one in use
//
//  in:     in_shader_program, in_uniform_name, in_val
void shader_program_set_uniform_float(
  const struct shader_program_t* in_shader_program,
  const char* in_uniform_name,
  float in_val);

// Sets an int uniform, such as a sampler's texture unit. The program must be
//  the one in use
//
//  in:     in_shader_program, in_uniform_name, in_val
void shader_program_set_uniform_int(
  const struct shader_program_t* in_shader_program,
  const char* in_uniform_name,
  int in_val);

/*******************************************************************************
 *
 *   DESTRUCTION
 *
 ******************************************************************************/

// Deletes a vertex shader. One attached to a program lives on until the
//  program is destroyed
//
//  in/out: inout_shader
void shader_vertex_destroy(
  struct shader_vertex_t* inout_shader);

// Deletes a fragment shader. One attached to a program lives on until the
//  program is destroyed
//
//  in/out: inout_shader
void shader_fragment_destroy(
  struct shader_fragment_t* inout_shader);

// Deletes a shader program and removes it from the resource registry
//
//  in/out: inout_shader_program
void shader_program_destroy(
  struct shader_program_t* inout_shader_program);
