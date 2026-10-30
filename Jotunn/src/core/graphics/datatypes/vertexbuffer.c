#include "vertexbuffer.h"
#include "vertex_buffer_internal.h"

#include "../resource_internal.h"

/*******************************************************************************
 *
 *   Function: vertex_buffer_init()
 *
 ******************************************************************************/
void vertex_buffer_init(
  struct vertex_buffer_t* inout_vertex_buffer,
  const int in_num)
{
  // Each buffer has its own registry entry, so they're created one at a time
  //  rather than with their names packed together
  for (int i = 0; i < in_num; i++)
  {
    vertex_buffer_internal_platform_init(
      &(inout_vertex_buffer[i]),
      1);

    resource_internal_register(
      &(inout_vertex_buffer[i].resource),
      RESOURCE_KIND_VERTEX_BUFFER,
      0);
  }
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_bind()
 *
 ******************************************************************************/
void vertex_buffer_bind(
  const struct vertex_buffer_t* in_vertex_buffer)
{
  vertex_buffer_internal_platform_bind(in_vertex_buffer);
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_set_data()
 *
 ******************************************************************************/
void vertex_buffer_set_data(
  struct vertex_buffer_t* inout_vertex_buffer,
  const void* in_data,
  const unsigned int in_data_bytes,
  const enum vertex_buffer_data_usage_t in_usage_type)
{
  vertex_buffer_internal_platform_set_data(
    inout_vertex_buffer, in_data, in_data_bytes, in_usage_type);

  // The buffer's storage is replaced with data_bytes of it
  resource_internal_set_size(
    &(inout_vertex_buffer->resource),
    in_data_bytes);
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_set_sub_data()
 *
 ******************************************************************************/
void vertex_buffer_set_sub_data(
  const struct vertex_buffer_t* in_vertex_buffer,
  const float* in_data,
  const unsigned int in_data_bytes)
{
  vertex_buffer_internal_platform_set_sub_data(in_vertex_buffer, in_data, in_data_bytes);
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_destroy()
 *
 ******************************************************************************/
void vertex_buffer_destroy(
  struct vertex_buffer_t* inout_vertex_buffer,
  const int in_num)
{
  for (int i = 0; i < in_num; i++)
  {
    vertex_buffer_internal_platform_destroy(
      &(inout_vertex_buffer[i]),
      1);

    resource_internal_unregister(&(inout_vertex_buffer[i].resource));
  }
}


/*******************************************************************************
 *
 *   Function: element_buffer_init()
 *
 ******************************************************************************/
int element_buffer_init(
  struct element_buffer_t* inout_element_buffer,
  const int in_num)
{
  // Each buffer has its own registry entry, so they're created one at a time
  //  rather than with their names packed together
  for (int i = 0; i < in_num; i++)
  {
    element_buffer_internal_platform_init(
      &(inout_element_buffer[i]),
      1);

    resource_internal_register(
      &(inout_element_buffer[i].resource),
      RESOURCE_KIND_ELEMENT_BUFFER,
      0);
  }

  // Creating buffer names can't fail
  return 0;
}

/*******************************************************************************
 *
 *   Function: element_buffer_bind()
 *
 ******************************************************************************/
void element_buffer_bind(
  const struct element_buffer_t* in_element_buffer)
{
  element_buffer_internal_platform_bind(in_element_buffer);
}

/*******************************************************************************
 *
 *   Function: element_buffer_buffer_data()
 *
 ******************************************************************************/
void element_buffer_buffer_data(
  struct element_buffer_t* inout_element_buffer,
  const void* in_index_data,
  const unsigned int in_data_bytes,
  const enum vertex_buffer_data_usage_t in_usage_type)
{
  element_buffer_internal_platform_buffer_data(
    inout_element_buffer, in_index_data, in_data_bytes, in_usage_type);

  // The buffer's storage is replaced with data_bytes of it
  resource_internal_set_size(
    &(inout_element_buffer->resource),
    in_data_bytes);
}

/*******************************************************************************
 *
 *   Function: element_buffer_destroy()
 *
 ******************************************************************************/
void element_buffer_destroy(
  struct element_buffer_t* inout_element_buffer,
  const int in_num)
{
  for (int i = 0; i < in_num; i++)
  {
    element_buffer_internal_platform_destroy(
      &(inout_element_buffer[i]),
      1);

    resource_internal_unregister(&(inout_element_buffer[i].resource));
  }
}
