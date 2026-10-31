#include "vertexarray.h"
#include "../../../core/graphics/datatypes/vertex_array_internal.h"

/*******************************************************************************
 *
 *   Function: vertex_array_internal_platform_init()
 *
 ******************************************************************************/
void vertex_array_internal_platform_init(
  struct vertex_array_t* out_vertex_array,
  int in_num)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: vertex_array_internal_platform_bind()
 *
 ******************************************************************************/
void vertex_array_internal_platform_bind(
  const struct vertex_array_t* in_vertex_array)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: vertex_array_internal_platform_unbind()
 *
 ******************************************************************************/
void vertex_array_internal_platform_unbind(void)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: vertex_array_internal_platform_set_attribute()
 *
 ******************************************************************************/
void vertex_array_internal_platform_set_attribute(
  const struct vertex_array_t* in_vertex_array,
  const struct vertex_attribute_t* in_vertex_attribute)
{
  return 0;
}

/*******************************************************************************
 *
 *   Function: vertex_array_internal_platform_destroy()
 *
 ******************************************************************************/
void vertex_array_internal_platform_destroy(
  struct vertex_array_t* inout_vertex_array,
  int in_num)
{
  return 0;
}
