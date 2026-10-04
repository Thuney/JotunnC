#include "vertexarray.h"

extern void platform_vertex_array_init(
  struct vertex_array_t* vertex_array,
  int num);
extern void platform_vertex_array_bind(
  struct vertex_array_t* vertex_array);
extern void platform_vertex_array_unbind();
extern void platform_vertex_array_set_attribute(
  struct vertex_array_t* vertex_array,
  struct vertex_attribute_t* vertex_attribute);
extern void platform_vertex_array_destroy(
  struct vertex_array_t* vertex_array,
  int num);

/*******************************************************************************
 *
 * Function: vertex_array_init()
 *
 ******************************************************************************/
void vertex_array_init(
  struct vertex_array_t* vertex_array,
  int num)
{
  platform_vertex_array_init(
    vertex_array,
    num);
}

/*******************************************************************************
 *
 * Function: vertex_array_bind()
 *
 ******************************************************************************/
void vertex_array_bind(
  struct vertex_array_t* vertex_array)
{
  platform_vertex_array_bind(vertex_array);
}

/*******************************************************************************
 *
 * Function: vertex_array_unbind()
 *
 ******************************************************************************/
void vertex_array_unbind()
{
  platform_vertex_array_unbind();
}

/*******************************************************************************
 *
 * Function: vertex_array_set_attribute()
 *
 ******************************************************************************/
void vertex_array_set_attribute(
  struct vertex_array_t* vertex_array,
  struct vertex_attribute_t* vertex_attribute)
{
  platform_vertex_array_set_attribute(
    vertex_array,
    vertex_attribute);
}

/*******************************************************************************
 *
 * Function: vertex_array_destroy()
 *
 ******************************************************************************/
void vertex_array_destroy(
  struct vertex_array_t* vertex_array,
  int num)
{
  platform_vertex_array_destroy(
    vertex_array,
    num);
}
