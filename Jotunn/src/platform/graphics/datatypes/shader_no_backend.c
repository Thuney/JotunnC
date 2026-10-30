#include "shader.h"
#include "../../../core/graphics/datatypes/shader_internal.h"

/*******************************************************************************
 *
 *   Function: vertex_shader_internal_platform_init()
 *
 ******************************************************************************/
int vertex_shader_internal_platform_init(
  struct shader_vertex_t* out_shader,
  const char* in_vertex_shader_src)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: fragment_shader_internal_platform_init()
 *
 ******************************************************************************/
int fragment_shader_internal_platform_init(
  struct shader_fragment_t* out_shader,
  const char* in_fragment_shader_src)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_init()
 *
 ******************************************************************************/
int shader_program_internal_platform_init(
  struct shader_program_t* out_shader_program,
  const struct shader_vertex_t* in_vertex_shader,
  const struct shader_fragment_t* in_fragment_shader)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_link()
 *
 ******************************************************************************/
int shader_program_internal_platform_link(
  struct shader_program_t* inout_shader_program)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_use()
 *
 ******************************************************************************/
void shader_program_internal_platform_use(
  const struct shader_program_t* in_shader_program)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_bind_fragment_data_location()
 *
 ******************************************************************************/
void shader_program_internal_platform_bind_fragment_data_location(
  const struct shader_program_t* in_shader_program,
  const unsigned int in_buffer_index,
  const char* in_var_name)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_get_attribute_location()
 *
 ******************************************************************************/
int shader_program_internal_platform_get_attribute_location(
  const struct shader_program_t* in_shader_program,
  const char* in_attribute_name)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_set_uniform()
 *
 ******************************************************************************/
void shader_program_internal_platform_set_uniform(
  const struct shader_program_t* in_shader_program,
  const char* in_uniform_name,
  const void* in_uniform_data,
  const enum shader_program_uniform_t in_uniform_type)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: vertex_shader_internal_platform_destroy()
 *
 ******************************************************************************/
void vertex_shader_internal_platform_destroy(
  struct shader_vertex_t* inout_shader)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: fragment_shader_internal_platform_destroy()
 *
 ******************************************************************************/
void fragment_shader_internal_platform_destroy(
  struct shader_fragment_t* inout_shader)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_destroy()
 *
 ******************************************************************************/
void shader_program_internal_platform_destroy(
  struct shader_program_t* inout_shader_program)
{
  return 0;
}
