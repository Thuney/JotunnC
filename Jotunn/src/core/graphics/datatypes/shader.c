#include "shader.h"
#include "shader_internal.h"
#include "log.h"
#include "allocator.h"
#include "../resource_internal.h"
#include "util.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************
 *
 *   Function: shader_vertex_init()
 *
 ******************************************************************************/
int shader_vertex_init(
  struct shader_vertex_t* out_shader,
  const char* in_vertex_shader_src)
{
  return vertex_shader_internal_platform_init(
    out_shader,
    in_vertex_shader_src);
}

/*******************************************************************************
 *
 *   Function: shader_fragment_init()
 *
 ******************************************************************************/
int shader_fragment_init(
  struct shader_fragment_t* out_shader,
  const char* in_fragment_shader_src)
{
  return fragment_shader_internal_platform_init(
    out_shader,
    in_fragment_shader_src);
}

/*******************************************************************************
 *
 *   Function: shader_vertex_init_filepath()
 *
 ******************************************************************************/
int shader_vertex_init_filepath(
  struct shader_vertex_t* out_shader,
  const char* in_filepath)
{
  char* file_contents = util_read_raw_file_contents(in_filepath);
  const int error = vertex_shader_internal_platform_init(
    out_shader,
    file_contents);
  allocator_free(file_contents);
  return error;
}

/*******************************************************************************
 *
 *   Function: shader_fragment_init_filepath()
 *
 ******************************************************************************/
int shader_fragment_init_filepath(
  struct shader_fragment_t* out_shader,
  const char* in_filepath)
{
  char* file_contents = util_read_raw_file_contents(in_filepath);
  const int error = fragment_shader_internal_platform_init(
    out_shader,
    file_contents);
  allocator_free(file_contents);
  return error;
}

/*******************************************************************************
 *
 *   Function: shader_program_init_filepath()
 *
 ******************************************************************************/
int shader_program_init_filepath(
  struct shader_program_t* inout_shader_program,
  const char* in_vertex_shader_filepath,
  const char* in_fragment_shader_filepath)
{
  struct shader_vertex_t vertex_shader;
  struct shader_fragment_t fragment_shader;

  int error;

  error = shader_vertex_init_filepath(
    &(vertex_shader),
    in_vertex_shader_filepath);
  if (error) {
    log_write(
      LOG_LEVEL_ERROR,
      "Error occurred during vertex shader init - Filepath = %s",
      in_vertex_shader_filepath);
  }

  error = shader_fragment_init_filepath(
    &(fragment_shader),
    in_fragment_shader_filepath);
  if (error) {
    log_write(
      LOG_LEVEL_ERROR,
      "Error occurred during fragment shader init - Filepath = %s",
      in_fragment_shader_filepath);
  }

  error = shader_program_init(
    inout_shader_program,
    &(vertex_shader),
    &(fragment_shader));
  if (error) {
    log_write(
      LOG_LEVEL_ERROR,
      "Error occurred during shader program init");
  }

  shader_program_bind_fragment_data_location(
    inout_shader_program,
    0,
    "fColor");

  error = shader_program_link(inout_shader_program);
  if (error) {
    log_write(
      LOG_LEVEL_ERROR,
      "Error occurred during shader program linkage");
  }

  shader_vertex_destroy(&(vertex_shader));
  shader_fragment_destroy(&(fragment_shader));

  return error;
}

/*******************************************************************************
 *
 *   Function: shader_program_init()
 *
 ******************************************************************************/
int shader_program_init(
  struct shader_program_t* inout_shader_program,
  const struct shader_vertex_t* in_vertex_shader,
  const struct shader_fragment_t* in_fragment_shader)
{
  const int error = shader_program_internal_platform_init(
    inout_shader_program,
    in_vertex_shader,
    in_fragment_shader);

  // The program exists whether or not its shaders could be attached
  resource_internal_register(
    &(inout_shader_program->resource),
    RESOURCE_KIND_SHADER_PROGRAM,
    0);

  return error;
}

/*******************************************************************************
 *
 *   Function: shader_program_link()
 *
 ******************************************************************************/
int shader_program_link(
  struct shader_program_t* inout_shader_program)
{
  return shader_program_internal_platform_link(inout_shader_program);
}

/*******************************************************************************
 *
 *   Function: shader_program_use()
 *
 ******************************************************************************/
void shader_program_use(
  const struct shader_program_t* in_shader_program)
{
  shader_program_internal_platform_use(in_shader_program);
}

/*******************************************************************************
 *
 *   Function: shader_program_bind_fragment_data_location()
 *
 ******************************************************************************/
void shader_program_bind_fragment_data_location(
  const struct shader_program_t* in_shader_program,
  const unsigned int in_buffer_index,
  const char* in_var_name)
{
  shader_program_internal_platform_bind_fragment_data_location(
    in_shader_program,
    in_buffer_index,
    in_var_name);
}

/*******************************************************************************
 *
 *   Function: shader_program_get_attribute_location()
 *
 ******************************************************************************/
int shader_program_get_attribute_location(
  const struct shader_program_t* in_shader_program,
  const char* in_attribute_name)
{
  return shader_program_internal_platform_get_attribute_location(
    in_shader_program,
    in_attribute_name);
}

/*******************************************************************************
 *
 *   Function: shader_program_set_uniform_fmat4x4()
 *
 ******************************************************************************/
void shader_program_set_uniform_fmat4x4(
  const struct shader_program_t* in_shader_program,
  const char* in_uniform_name,
  const union fmatrix_4x4_t* in_matrix)
{
  shader_program_internal_platform_set_uniform(
    in_shader_program,
    in_uniform_name,
    in_matrix,
    SHADER_PROGRAM_UNIFORM_FMAT4X4);
}

/*******************************************************************************
 *
 *   Function: shader_program_set_uniform_float()
 *
 ******************************************************************************/
void shader_program_set_uniform_float(
  const struct shader_program_t* in_shader_program,
  const char* in_uniform_name,
  const float in_val)
{
  shader_program_internal_platform_set_uniform(
    in_shader_program,
    in_uniform_name,
    &(in_val),
    SHADER_PROGRAM_UNIFORM_FLOAT);
}

/*******************************************************************************
 *
 *   Function: shader_program_set_uniform_int()
 *
 ******************************************************************************/
void shader_program_set_uniform_int(
  const struct shader_program_t* in_shader_program,
  const char* in_uniform_name,
  const int in_val)
{
  shader_program_internal_platform_set_uniform(
    in_shader_program,
    in_uniform_name,
    &(in_val),
    SHADER_PROGRAM_UNIFORM_INT);
}

/*******************************************************************************
 *
 *   Function: shader_vertex_destroy()
 *
 ******************************************************************************/
void shader_vertex_destroy(
  struct shader_vertex_t* inout_shader)
{
  vertex_shader_internal_platform_destroy(inout_shader);
}

/*******************************************************************************
 *
 *   Function: shader_fragment_destroy()
 *
 ******************************************************************************/
void shader_fragment_destroy(
  struct shader_fragment_t* inout_shader)
{
  fragment_shader_internal_platform_destroy(inout_shader);
}

/*******************************************************************************
 *
 *   Function: shader_program_destroy()
 *
 ******************************************************************************/
void shader_program_destroy(
  struct shader_program_t* inout_shader_program)
{
  shader_program_internal_platform_destroy(inout_shader_program);

  resource_internal_unregister(&(inout_shader_program->resource));
}
