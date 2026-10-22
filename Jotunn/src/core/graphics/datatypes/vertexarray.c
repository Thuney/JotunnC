#include "vertexarray.h"

#include "../resource_internal.h"

extern void platform_vertex_array_init(struct vertex_array_t* vertex_array, int num);
extern void platform_vertex_array_bind(struct vertex_array_t* vertex_array);
extern void platform_vertex_array_unbind();
extern void platform_vertex_array_set_attribute(struct vertex_array_t* vertex_array, struct vertex_attribute_t* vertex_attribute);
extern void platform_vertex_array_destroy(struct vertex_array_t* vertex_array, int num);

void vertex_array_init(
  struct vertex_array_t* vertex_array,
  int num)
{
  // Each array has its own registry entry, so they're created one at a time
  //  rather than with their names packed together
  for (int i = 0; i < num; i++)
  {
    platform_vertex_array_init(
      &(vertex_array[i]),
      1);

    // A vertex array only records how its buffers are read
    resource_internal_register(
      &(vertex_array[i].resource),
      RESOURCE_KIND_VERTEX_ARRAY,
      0);
  }
}

void vertex_array_bind(
  struct vertex_array_t* vertex_array)
{
  platform_vertex_array_bind(vertex_array);
}

void vertex_array_unbind()
{
  platform_vertex_array_unbind();
}

void vertex_array_set_attribute(
  struct vertex_array_t* vertex_array,
  struct vertex_attribute_t* vertex_attribute)
{
  platform_vertex_array_set_attribute(vertex_array, vertex_attribute);
}

void vertex_array_destroy(
  struct vertex_array_t* vertex_array,
  int num)
{
  for (int i = 0; i < num; i++)
  {
    platform_vertex_array_destroy(
      &(vertex_array[i]),
      1);

    resource_internal_unregister(&(vertex_array[i].resource));
  }
}
