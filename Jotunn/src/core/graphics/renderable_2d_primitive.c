#include <memory.h>
#include <stdlib.h>

#include "renderable_2d_primitive.h"

const fvector3 triangle_2d_position_data[3] = 
{
   [0] = 
   {
      { 0.0f, 0.0f, 0.0f }
   },
   [1] = 
   {
      { 5.0f, 10.0f, 0.0f }
   },
   [2] = 
   {
      { 10.0f, 0.0f, 0.0f }
   }
};
const fvector3 quad_2d_position_data[4] =
{ 
   [0] = 
   {
      { 0.0f, 0.0f, 0.0f }
   },
   [1] = 
   {
      { 10.0f, 0.0f, 0.0f }
   },
   [2] = 
   {
      { 10.0f, 10.0f, 0.0f }
   },
   [3] = 
   {
      { 0.0f, 10.0f, 0.0f }
   }
};

// Triangle 2D vertex attributes
static struct vertex_attribute_t triangle_vertex_attributes[2] = {
   [0] = 
   {
      .attribute_name       = "position",
      .index                = (unsigned int)-1, // To be filled by shader program
      .size                 = 3,
      .data_type            = V_FLOAT,
      .should_normalize     = 0,
      .stride               = sizeof(struct renderable_2d_triangle_vertex_t),
      .ptr_offset_to_attrib = 0
   },
   [1] = 
   {
      .attribute_name       = "color",
      .index                = (unsigned int)-1, // To be filled by shader program
      .size                 = 4,
      .data_type            = V_FLOAT,
      .should_normalize     = 0,
      .stride               = sizeof(struct renderable_2d_triangle_vertex_t),
      .ptr_offset_to_attrib = (void*)sizeof(fvector3)
   }
};

// Quad 2D vertex attributes
static struct vertex_attribute_t quad_vertex_attributes[2] = {
   [0] = 
   {
      .attribute_name       = "position",
      .index                = (unsigned int)-1, // To be filled by shader program
      .size                 = 3,
      .data_type            = V_FLOAT,
      .should_normalize     = 0,
      .stride               = sizeof(struct renderable_2d_quad_vertex_t),
      .ptr_offset_to_attrib = 0
   },
   [1] = 
   {
      .attribute_name       = "color",
      .index                = (unsigned int)-1, // To be filled by shader program
      .size                 = 4,
      .data_type            = V_FLOAT,
      .should_normalize     = 0,
      .stride               = sizeof(struct renderable_2d_quad_vertex_t),
      .ptr_offset_to_attrib = (void*)sizeof(fvector3)
   }
};

// Circle 2D vertex attributes
static struct vertex_attribute_t circle_vertex_attributes[2] = {
   [0] = 
   {
      .attribute_name       = "position",
      .index                = (unsigned int)-1, // To be filled by shader program
      .size                 = 3,
      .data_type            = V_FLOAT,
      .should_normalize     = 0,
      .stride               = sizeof(struct renderable_2d_circle_vertex_t),
      .ptr_offset_to_attrib = 0
   },
   [1] = 
   {
      .attribute_name       = "color",
      .index                = (unsigned int)-1, // To be filled by shader program
      .size                 = 4,
      .data_type            = V_FLOAT,
      .should_normalize     = 0,
      .stride               = sizeof(struct renderable_2d_circle_vertex_t),
      .ptr_offset_to_attrib = (void*)sizeof(fvector3)
   }
};

// Line 2D vertex attributes
static struct vertex_attribute_t line_vertex_attributes[2] = {
   [0] = 
   {
      .attribute_name       = "position",
      .index                = (unsigned int)-1, // To be filled by shader program
      .size                 = 4,
      .data_type            = V_FLOAT,
      .should_normalize     = 0,
      .stride               = sizeof(struct renderable_2d_line_vertex_t),
      .ptr_offset_to_attrib = 0
   },
   [1] = 
   {
      .attribute_name       = "color",
      .index                = (unsigned int)-1, // To be filled by shader program
      .size                 = 4,
      .data_type            = V_FLOAT,
      .should_normalize     = 0,
      .stride               = sizeof(struct renderable_2d_line_vertex_t),
      .ptr_offset_to_attrib = (void*)sizeof(fvector3)
   }
};

static void renderable_2d_primitive_data_init(struct vertex_array_t* vao, struct vertex_buffer_t* vbo, struct element_buffer_t* ebo, unsigned int use_ebo, struct vertex_attribute_t* attributes, const unsigned int num_attributes, struct shader_program_t* shader_program)
{
   #ifdef DEBUG
      fprintf(stdout, "Initializing vertex array\n");
   #endif

   vertex_array_init(vao, 1);

   #ifdef DEBUG
      fprintf(stdout, "Initializing vertex buffer\n");
   #endif

   vertex_buffer_init(vbo, 1);

   if (use_ebo) 
   {
      #ifdef DEBUG
         fprintf(stdout, "Initializing element buffer\n");
      #endif
      element_buffer_init(ebo, 1);
   }

   #ifdef DEBUG
      fprintf(stdout, "Binding vertex array\n");
   #endif

   vertex_array_bind(vao);

   #ifdef DEBUG
      fprintf(stdout, "Binding vertex buffer\n");
   #endif

   vertex_buffer_bind(vbo);

   if (use_ebo)
   {
      #ifdef DEBUG
         fprintf(stdout, "Binding element buffer\n");
      #endif
      element_buffer_bind(ebo);
   }

   unsigned int attribute_index, temp_shader_index;
   char* attribute_name;

   for (attribute_index = 0; attribute_index < num_attributes; attribute_index++)
   {
      attribute_name = attributes[attribute_index].attribute_name;
      temp_shader_index = shader_program_get_attribute_location(shader_program, attribute_name);

      #ifdef DEBUG
         fprintf(stdout, "Setting shader attribute at index %u with name %s\n", temp_shader_index, attribute_name);
      #endif

      attributes[attribute_index].index = temp_shader_index;

      #ifdef DEBUG
         vertex_attribute_print(&attributes[attribute_index]);
      #endif

      vertex_array_set_attribute(vao, &attributes[attribute_index]);
   }
}

// Triangle
void renderable_2d_triangle_data_init(struct renderable_2d_triangle_data_t* triangle_data, const unsigned int max_count, struct shader_program_t* shader_program)
{
   const unsigned int max_vertices = 3*max_count;
   const unsigned int max_indices  = 3*max_count;

   renderable_2d_primitive_data_init(&triangle_data->vao, &triangle_data->vbo, &triangle_data->ebo, 1, triangle_vertex_attributes, _TRIANGLE_2D_ATTRIBUTE_NUM, shader_program);

   triangle_data->vertex_data_base = (struct renderable_2d_triangle_vertex_t*) calloc(max_vertices, sizeof(struct renderable_2d_triangle_vertex_t));
   triangle_data->vertex_data_ptr  = triangle_data->vertex_data_base;

   unsigned int triangle_indices[max_indices];

   unsigned int offset = 0;
   for (unsigned int i = 0; i < max_indices; i+= 3)
   {
      triangle_indices[i + 0] = offset + 0;
      triangle_indices[i + 1] = offset + 1;
      triangle_indices[i + 2] = offset + 2;

      offset += 3;
   }

   element_buffer_buffer_data(&triangle_data->ebo, triangle_indices, max_indices*sizeof(unsigned int), STATIC_DRAW);
   vertex_buffer_buffer_data(&triangle_data->vbo, (float*)triangle_data->vertex_data_base, max_vertices*sizeof(struct renderable_2d_triangle_vertex_t), DYNAMIC_DRAW);
   vertex_array_unbind();
}

// Quad
void renderable_2d_quad_data_init(struct renderable_2d_quad_data_t* quad_data, const unsigned int max_count, struct shader_program_t* shader_program)
{
   const unsigned int max_vertices = 4*max_count;
   const unsigned int max_indices  = 6*max_count;

   renderable_2d_primitive_data_init(&quad_data->vao, &quad_data->vbo, &quad_data->ebo, 1, quad_vertex_attributes, _QUAD_2D_ATTRIBUTE_NUM, shader_program);

   quad_data->vertex_data_base = (struct renderable_2d_quad_vertex_t*) calloc(max_vertices, sizeof(struct renderable_2d_quad_vertex_t));
   quad_data->vertex_data_ptr  = quad_data->vertex_data_base;

   unsigned int quad_indices[max_indices];

   unsigned int offset = 0;
   for (unsigned int i = 0; i < max_indices; i+= 6)
   {
      quad_indices[i + 0] = offset + 0;
      quad_indices[i + 1] = offset + 1;
      quad_indices[i + 2] = offset + 2;

      quad_indices[i + 3] = offset + 0;
      quad_indices[i + 4] = offset + 3;
      quad_indices[i + 5] = offset + 2;

      offset += 4;
   }

   element_buffer_buffer_data(&quad_data->ebo, quad_indices, max_indices*sizeof(unsigned int), STATIC_DRAW);
   vertex_buffer_buffer_data(&quad_data->vbo, (float*)quad_data->vertex_data_base, max_vertices*sizeof(struct renderable_2d_quad_vertex_t), DYNAMIC_DRAW);
   vertex_array_unbind();
}

// Circle
void renderable_2d_circle_data_init(struct renderable_2d_circle_data_t* circle_data, const unsigned int max_count, struct shader_program_t* shader_program)
{
   const unsigned int max_vertices = 4*max_count;
   const unsigned int max_indices  = 6*max_count;

   renderable_2d_primitive_data_init(&circle_data->vao, &circle_data->vbo, &circle_data->ebo, 1, circle_vertex_attributes, _CIRCLE_2D_ATTRIBUTE_NUM, shader_program);

   circle_data->vertex_data_base = (struct renderable_2d_circle_vertex_t*) calloc(max_vertices, sizeof(struct renderable_2d_circle_vertex_t));
   circle_data->vertex_data_ptr  = circle_data->vertex_data_base;

   unsigned int circle_indices[max_indices]; // Actually the same as our quad indices, and will cull pixels in the shader to form circle

   unsigned int offset = 0;
   for (unsigned int i = 0; i < max_indices; i+= 6)
   {
      circle_indices[i + 0] = offset + 0;
      circle_indices[i + 1] = offset + 1;
      circle_indices[i + 2] = offset + 2;

      circle_indices[i + 3] = offset + 0;
      circle_indices[i + 4] = offset + 3;
      circle_indices[i + 5] = offset + 2;

      offset += 4;
   }

   element_buffer_buffer_data(&circle_data->ebo, circle_indices, max_indices*sizeof(unsigned int), STATIC_DRAW);
   vertex_buffer_buffer_data(&circle_data->vbo, (float*)circle_data->vertex_data_base, max_vertices*sizeof(struct renderable_2d_circle_vertex_t), DYNAMIC_DRAW);
   vertex_array_unbind();
}

// Line
void renderable_2d_line_data_init(struct renderable_2d_line_data_t* line_data, const unsigned int max_count, struct shader_program_t* shader_program)
{
   const unsigned int max_vertices = 2*max_count;

   renderable_2d_primitive_data_init(&line_data->vao, &line_data->vbo, NULL, 0, line_vertex_attributes, _LINE_2D_ATTRIBUTE_NUM, shader_program);

   line_data->vertex_data_base = (struct renderable_2d_line_vertex_t*) calloc(max_vertices, sizeof(struct renderable_2d_line_vertex_t));
   line_data->vertex_data_ptr  = line_data->vertex_data_base;

   vertex_buffer_buffer_data(&line_data->vbo, 0, max_vertices*sizeof(struct renderable_2d_line_vertex_t), DYNAMIC_DRAW);

   vertex_array_unbind();
}

void renderable_2d_triangle_data_cleanup(struct renderable_2d_triangle_data_t* triangle_data)
{
   vertex_array_destroy(&triangle_data->vao, 1);
   vertex_buffer_destroy(&triangle_data->vbo, 1);
   element_buffer_destroy(&triangle_data->ebo, 1);

   free(triangle_data->vertex_data_base);
}

void renderable_2d_quad_data_cleanup(struct renderable_2d_quad_data_t* quad_data)
{
   vertex_array_destroy(&quad_data->vao, 1);
   vertex_buffer_destroy(&quad_data->vbo, 1);
   element_buffer_destroy(&quad_data->ebo, 1);

   free(quad_data->vertex_data_base);
}

void renderable_2d_circle_data_cleanup(struct renderable_2d_circle_data_t* circle_data)
{
   vertex_array_destroy(&circle_data->vao, 1);
   vertex_buffer_destroy(&circle_data->vbo, 1);
   element_buffer_destroy(&circle_data->ebo, 1);

   free(circle_data->vertex_data_base);
}

void renderable_2d_line_data_cleanup(struct renderable_2d_line_data_t* line_data)
{
   vertex_array_destroy(&line_data->vao, 1);
   vertex_buffer_destroy(&line_data->vbo, 1);

   free(line_data->vertex_data_base);
}