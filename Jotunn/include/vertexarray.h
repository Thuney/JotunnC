#pragma once

#include "vertexattribute.h"

struct vertex_array_t
{
   unsigned int vertex_array;
};

int vertex_array_init(struct vertex_array_t* vertex_array, int num);
void vertex_array_bind(struct vertex_array_t* vertex_array);
void vertex_array_unbind();
void vertex_array_set_attribute(struct vertex_array_t* vertex_array, struct vertex_attribute_t* vertex_attribute);
void vertex_array_destroy(struct vertex_array_t* vertex_array, int num);