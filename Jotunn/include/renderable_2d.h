#pragma once

#include "fmatrix.h"
#include "vertexarray.h"
#include "vertexbuffer.h"
#include "shader.h" 

enum renderable_2d_attribute_t
{
   POSITION = 0x00,
   COLOR,
   _ATTRIBUTE_COUNT
};

struct renderable_2d_t
{
   // 
   struct shader_program_t* shader_program;
   //
   float* vertex_data;
   unsigned int num_vertices;
   unsigned int* index_data;
   unsigned int num_indices;
   //
   struct vertex_array_t vao;
   struct vertex_buffer_t vbo;
   struct element_buffer_t ebo;
   struct vertex_attribute_t vertex_attributes[_ATTRIBUTE_COUNT];
   //
   fmatrix_4x4 model_matrix;
};

void renderable_2d_init(struct renderable_2d_t* renderable_2d, float* vertex_data, unsigned int num_vertices, unsigned int* index_data, unsigned int num_indices);
void renderable_2d_cleanup(struct renderable_2d_t* renderable_2d);