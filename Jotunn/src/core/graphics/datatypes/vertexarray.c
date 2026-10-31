#include "vertexarray.h"
#include "vertex_array_internal.h"

#include "../resource_internal.h"

/*******************************************************************************
 *
 *   Function: vertex_array_init()
 *
 ******************************************************************************/
void vertex_array_init(
  struct vertex_array_t* inout_vertex_array,
  int in_num)
{
  // Each array has its own registry entry, so they're created one at a time
  //  rather than with their names packed together
  for (int i = 0; i < in_num; i++) {
    vertex_array_internal_platform_init(
      &(inout_vertex_array[i]),
      1);

    // A vertex array only records how its buffers are read
    resource_internal_register(
      &(inout_vertex_array[i].resource),
      RESOURCE_KIND_VERTEX_ARRAY,
      0);
  }
}

/*******************************************************************************
 *
 *   Function: vertex_array_bind()
 *
 ******************************************************************************/
void vertex_array_bind(
  const struct vertex_array_t* in_vertex_array)
{
  vertex_array_internal_platform_bind(in_vertex_array);
}

/*******************************************************************************
 *
 *   Function: vertex_array_unbind()
 *
 ******************************************************************************/
void vertex_array_unbind(void)
{
  vertex_array_internal_platform_unbind();
}

/*******************************************************************************
 *
 *   Function: vertex_array_set_attribute()
 *
 ******************************************************************************/
void vertex_array_set_attribute(
  const struct vertex_array_t* in_vertex_array,
  const struct vertex_attribute_t* in_vertex_attribute)
{
  vertex_array_internal_platform_set_attribute(
    in_vertex_array,
    in_vertex_attribute);
}

/*******************************************************************************
 *
 *   Function: vertex_array_destroy()
 *
 ******************************************************************************/
void vertex_array_destroy(
  struct vertex_array_t* inout_vertex_array,
  int in_num)
{
  for (int i = 0; i < in_num; i++) {
    vertex_array_internal_platform_destroy(
      &(inout_vertex_array[i]),
      1);

    resource_internal_unregister(&(inout_vertex_array[i].resource));
  }
}
