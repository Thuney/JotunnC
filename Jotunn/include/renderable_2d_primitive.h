#pragma once

#include "fvector.h"
#include "renderable_2d.h"

struct vertex_data_2d_t
{
   fvector2 position;
   fvector4 color_rgba;
};

struct rgba_triangle_2d_t
{
   struct vertex_data_2d_t vertices[3];
   unsigned int indices[3];
   struct renderable_2d_t renderable_data;
};

struct rgba_square_2d_t
{
   struct vertex_data_2d_t vertices[4];
   unsigned int indices[6];
   struct renderable_2d_t renderable_data;
};

void rgba_triangle_2d_init(struct rgba_triangle_2d_t* triangle, const fvector4 color);
void rgba_square_2d_init(struct rgba_square_2d_t* square, const fvector4 color);