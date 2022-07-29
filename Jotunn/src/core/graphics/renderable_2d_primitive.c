#include "renderable_2d_primitive.h"

void rgba_triangle_2d_init(struct rgba_triangle_2d_t* triangle, const fvector4 color)
{
   // Triangle data
   // fvector3_set(&triangle->vertices[0].position, 0.0f, 0.0f, 0.0f);
   // fvector3_set(&triangle->vertices[1].position, 0.5f, 1.0f, 0.0f);
   // fvector3_set(&triangle->vertices[2].position, 1.0f, 0.0f, 0.0f);

   fvector2_set(&triangle->vertices[0].position, 0.0f, 0.0f);
   fvector2_set(&triangle->vertices[1].position, 0.5f, 1.0f);
   fvector2_set(&triangle->vertices[2].position, 1.0f, 0.0f);

   fvector4_set(&triangle->vertices[0].color_rgba, color.comp.w, color.comp.x, color.comp.y, color.comp.z);
   fvector4_set(&triangle->vertices[1].color_rgba, color.comp.w, color.comp.x, color.comp.y, color.comp.z);
   fvector4_set(&triangle->vertices[2].color_rgba, color.comp.w, color.comp.x, color.comp.y, color.comp.z);

   triangle->indices[0] = 0;
   triangle->indices[1] = 1;
   triangle->indices[2] = 2;

   renderable_2d_init(&triangle->renderable_data, (float*)&triangle->vertices, 3, (unsigned int*)triangle->indices, 3);
}

void rgba_square_2d_init(struct rgba_square_2d_t* square, const fvector4 color)
{
   // Square data
   // fvector3_set(&square->vertices[0].position, 0.0f, 0.0f, 0.0f);
   // fvector3_set(&square->vertices[1].position, 1.0f, 0.0f, 0.0f);
   // fvector3_set(&square->vertices[2].position, 1.0f, 1.0f, 0.0f);
   // fvector3_set(&square->vertices[3].position, 0.0f, 1.0f, 0.0f);

   fvector2_set(&square->vertices[0].position, 0.0f, 0.0f);
   fvector2_set(&square->vertices[1].position, 1.0f, 0.0f);
   fvector2_set(&square->vertices[2].position, 1.0f, 1.0f);
   fvector2_set(&square->vertices[3].position, 0.0f, 1.0f);

   fvector4_set(&square->vertices[0].color_rgba, color.comp.w, color.comp.x, color.comp.y, color.comp.z);
   fvector4_set(&square->vertices[1].color_rgba, color.comp.w, color.comp.x, color.comp.y, color.comp.z);
   fvector4_set(&square->vertices[2].color_rgba, color.comp.w, color.comp.x, color.comp.y, color.comp.z);
   fvector4_set(&square->vertices[3].color_rgba, color.comp.w, color.comp.x, color.comp.y, color.comp.z);

   square->indices[0] = 0;
   square->indices[1] = 1;
   square->indices[2] = 2;
   square->indices[3] = 0;
   square->indices[4] = 3;
   square->indices[5] = 2;

   renderable_2d_init(&square->renderable_data, (float*)&square->vertices, 4, (unsigned int*)square->indices, 6);
}