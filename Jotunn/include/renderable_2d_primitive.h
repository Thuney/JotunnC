#pragma once

#include "fvector.h"
#include "shader.h"
#include "vertexarray.h"
#include "vertexbuffer.h"

// Vertex data

#define _RENDERABLE_2D_CIRCLE_RESOLUTION 16 // Number of triangles used to approximate circle
#define _NUM_CIRCLE_2D_VERTICES (_RENDERABLE_2D_CIRCLE_RESOLUTION + 1)

extern const fvector3 triangle_2d_position_data[3];
extern const fvector3 quad_2d_position_data[4];
extern fvector3 circle_2d_position_data[_NUM_CIRCLE_2D_VERTICES];

enum renderable_2d_triangle_vertex_attribute_t
{
   TRIANGLE_2D_POSITION = 0x00,
   TRIANGLE_2D_COLOR,
   _TRIANGLE_2D_ATTRIBUTE_NUM
};

enum renderable_2d_quad_vertex_attribute_t
{
   QUAD_2D_POSITION = 0x00,
   QUAD_2D_COLOR,
   _QUAD_2D_ATTRIBUTE_NUM
};

enum renderable_2d_circle_vertex_attribute_t
{
   CIRCLE_2D_POSITION = 0x00,
   CIRCLE_2D_COLOR,
   _CIRCLE_2D_ATTRIBUTE_NUM
};

enum renderable_2d_line_vertex_attribute_t
{
   LINE_2D_POSITION = 0x00,
   LINE_2D_COLOR,
   _LINE_2D_ATTRIBUTE_NUM
};

// 

struct renderable_2d_triangle_vertex_t
{
   fvector3 position;
   fvector4 color;
};

struct renderable_2d_quad_vertex_t
{
   fvector3 position;
   fvector4 color;
};

struct renderable_2d_circle_vertex_t
{
   fvector3 position;
   fvector4 color;
};

struct renderable_2d_line_vertex_t
{
   fvector3 position;
   fvector4 color;
};

// 

struct renderable_2d_triangle_data_t
{
   struct vertex_array_t vao;
   struct vertex_buffer_t vbo;
   struct element_buffer_t ebo;
   // 
   unsigned int triangle_index_count;
   // Data pointers
   struct renderable_2d_triangle_vertex_t* vertex_data_base;
   struct renderable_2d_triangle_vertex_t* vertex_data_ptr;
};

struct renderable_2d_quad_data_t
{
   struct vertex_array_t vao;
   struct vertex_buffer_t vbo;
   struct element_buffer_t ebo;
   // 
   unsigned int quad_index_count;
   // Data pointers
   struct renderable_2d_quad_vertex_t* vertex_data_base;
   struct renderable_2d_quad_vertex_t* vertex_data_ptr;
};

struct renderable_2d_circle_data_t
{
   struct vertex_array_t vao;
   struct vertex_buffer_t vbo;
   struct element_buffer_t ebo;
   // 
   unsigned int circle_index_count;
   // Data pointers
   struct renderable_2d_circle_vertex_t* vertex_data_base;
   struct renderable_2d_circle_vertex_t* vertex_data_ptr;
};

struct renderable_2d_line_data_t
{
   struct vertex_array_t vao;
   struct vertex_buffer_t vbo;
   // 
   unsigned int line_vertex_count;
   // Data pointers
   struct renderable_2d_line_vertex_t* vertex_data_base;
   struct renderable_2d_line_vertex_t* vertex_data_ptr;
};

//

void renderable_2d_triangle_data_init(struct renderable_2d_triangle_data_t* triangle_data, const unsigned int max_count, struct shader_program_t* shader_program);
void renderable_2d_quad_data_init(struct renderable_2d_quad_data_t* quad_data, const unsigned int max_count, struct shader_program_t* shader_program);
void renderable_2d_circle_data_init(struct renderable_2d_circle_data_t* circle_data, const unsigned int max_count, struct shader_program_t* shader_program);
void renderable_2d_line_data_init(struct renderable_2d_line_data_t* line_data, const unsigned int max_count, struct shader_program_t* shader_program);

void renderable_2d_triangle_data_cleanup(struct renderable_2d_triangle_data_t* triangle_data);
void renderable_2d_quad_data_cleanup(struct renderable_2d_quad_data_t* quad_data);
void renderable_2d_circle_data_cleanup(struct renderable_2d_circle_data_t* circle_data);
void renderable_2d_line_data_cleanup(struct renderable_2d_line_data_t* line_data);