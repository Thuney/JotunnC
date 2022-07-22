#pragma once

#include "fvector.h"
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
   struct shader_program_t shader;
   struct vertex_array_t vao;
   struct vertex_buffer_t vbo;
   struct vertex_attribute_t vertex_attributes[_ATTRIBUTE_COUNT];
};

void renderable_2d_init(struct renderable_2d_t* renderable_2d);
void renderable_2d_render(struct renderable_2d_t* renderable_2d);
void renderable_2d_cleanup(struct renderable_2d_t* renderable_2d);