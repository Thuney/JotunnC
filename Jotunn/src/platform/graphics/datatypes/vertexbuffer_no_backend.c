#include "vertexbuffer.h"
#include "../../../core/graphics/datatypes/vertex_buffer_internal.h"

/*******************************************************************************
 *
 *   Function: vertex_buffer_internal_platform_init()
 *
 ******************************************************************************/
void vertex_buffer_internal_platform_init(
  struct vertex_buffer_t* out_vertex_buffer,
  int in_num)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_internal_platform_bind()
 *
 ******************************************************************************/
void vertex_buffer_internal_platform_bind(
  const struct vertex_buffer_t* in_vertex_buffer)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_internal_platform_set_data()
 *
 ******************************************************************************/
void vertex_buffer_internal_platform_set_data(
  const struct vertex_buffer_t* in_vertex_buffer,
  const void* in_data,
  unsigned int in_data_bytes,
  enum vertex_buffer_data_usage_t in_usage_type)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_internal_platform_destroy()
 *
 ******************************************************************************/
void vertex_buffer_internal_platform_destroy(
  struct vertex_buffer_t* inout_vertex_buffer,
  int in_num)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: element_buffer_internal_platform_init()
 *
 ******************************************************************************/
void element_buffer_internal_platform_init(
  struct element_buffer_t* out_element_buffer,
  int in_num)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: element_buffer_internal_platform_bind()
 *
 ******************************************************************************/
void element_buffer_internal_platform_bind(
  const struct element_buffer_t* in_element_buffer)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: element_buffer_internal_platform_buffer_data()
 *
 ******************************************************************************/
void element_buffer_internal_platform_buffer_data(
  const struct element_buffer_t* in_element_buffer,
  const void* in_index_data,
  unsigned int in_data_bytes,
  enum vertex_buffer_data_usage_t in_usage_type)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: element_buffer_internal_platform_set_sub_data()
 *
 ******************************************************************************/
void element_buffer_internal_platform_set_sub_data(
  const struct element_buffer_t* in_element_buffer,
  unsigned int in_offset_bytes,
  const void* in_index_data,
  unsigned int in_data_bytes)
{
}

/*******************************************************************************
 *
 *   Function: element_buffer_internal_platform_destroy()
 *
 ******************************************************************************/
void element_buffer_internal_platform_destroy(
  struct element_buffer_t* inout_element_buffer,
  int in_num)
{
  return 0;
}
