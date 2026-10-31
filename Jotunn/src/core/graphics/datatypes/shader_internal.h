#pragma once

// Private to Jotunn: the backend hooks behind shader.h. Each rendering
//  backend defines these, and shader.c calls them, handling the resource
//  registry itself

#include "shader.h"

/*******************************************************************************
 *
 *   PLATFORM INITIALIZATION
 *
 ******************************************************************************/

// Creates a vertex shader and compiles it from source, logging the
//  compiler's messages in DEBUG builds when it fails
//
//  in:     in_vertex_shader_src
//  out:    out_shader
//  ret:    0 when the shader compiled; 1 when it didn't, or there is no
//            backend. Only DEBUG builds check, so other builds always return 0
//            with a backend
int vertex_shader_internal_platform_init(
  struct shader_vertex_t* out_shader,
  const char* in_vertex_shader_src);

// Creates a fragment shader and compiles it from source, logging the
//  compiler's messages in DEBUG builds when it fails
//
//  in:     in_fragment_shader_src
//  out:    out_shader
//  ret:    0 when the shader compiled; 1 when it didn't, or there is no
//            backend. Only DEBUG builds check, so other builds always return 0
//            with a backend
int fragment_shader_internal_platform_init(
  struct shader_fragment_t* out_shader,
  const char* in_fragment_shader_src);

// Creates a shader program and attaches a vertex and a fragment shader to it
//
//  in:     in_vertex_shader, in_fragment_shader
//  out:    out_shader_program
//  ret:    0 on success; 1 when the shaders couldn't be attached, or there
//            is no backend
int shader_program_internal_platform_init(
  struct shader_program_t* out_shader_program,
  const struct shader_vertex_t* in_vertex_shader,
  const struct shader_fragment_t* in_fragment_shader);

// Links the shaders attached to a program, logging the linker's messages in
//  DEBUG builds when it fails
//
//  in/out: inout_shader_program
//  ret:    0 when the program linked; 1 when it didn't, or there is no
//            backend. Only DEBUG builds check, so other builds always return 0
//            with a backend
int shader_program_internal_platform_link(
  struct shader_program_t* inout_shader_program);

/*******************************************************************************
 *
 *   PLATFORM USE
 *
 ******************************************************************************/

// Makes a linked program the one used for drawing
//
//  in:     in_shader_program
void shader_program_internal_platform_use(
  const struct shader_program_t* in_shader_program);

// Binds a fragment shader output variable to a draw buffer, taking effect
//  when the program is next linked
//
//  in:     in_shader_program, in_buffer_index, in_var_name
void shader_program_internal_platform_bind_fragment_data_location(
  const struct shader_program_t* in_shader_program,
  const unsigned int in_buffer_index,
  const char* in_var_name);

// Finds the location of a vertex attribute in a linked program
//
//  in:     in_shader_program, in_attribute_name
//  ret:    The attribute's location; -1 when the program has no active
//            attribute of that name
int shader_program_internal_platform_get_attribute_location(
  const struct shader_program_t* in_shader_program,
  const char* in_attribute_name);

// Sets a uniform of the program in use, looking it up by name in
//  in_shader_program. in_uniform_data points to a value of the C type that
//  matches in_uniform_type (an int for SHADER_PROGRAM_UNIFORM_BOOL); an
//  unknown type is logged in DEBUG builds and otherwise ignored
//
//  in:     in_shader_program, in_uniform_name, in_uniform_data,
//            in_uniform_type
void shader_program_internal_platform_set_uniform(
  const struct shader_program_t* in_shader_program,
  const char* in_uniform_name,
  const void* in_uniform_data,
  const enum shader_program_uniform_t in_uniform_type);

/*******************************************************************************
 *
 *   PLATFORM DESTRUCTION
 *
 ******************************************************************************/

// Deletes a vertex shader
//
//  in/out: inout_shader
void vertex_shader_internal_platform_destroy(
  struct shader_vertex_t* inout_shader);

// Deletes a fragment shader
//
//  in/out: inout_shader
void fragment_shader_internal_platform_destroy(
  struct shader_fragment_t* inout_shader);

// Deletes a shader program
//
//  in/out: inout_shader_program
void shader_program_internal_platform_destroy(
  struct shader_program_t* inout_shader_program);
