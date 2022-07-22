#include "renderable_2d.h"

/**************************************************
 * 
 *  Variables
 * 
 **************************************************/

const char* vertex_shader_src =
   "#version 150"
   "\n"
   "in vec2 position;\n"
   "\n"
   "in vec4 color;"
   "\n"
   "out vec4 vColor;"
   "\n"
   "void main()"
   "{"
   "	gl_Position = vec4(position, 0.0, 1.0);"
   "  vColor = color;"
   "};";

const char* fragment_shader_src =
   "#version 150"
   "\n"
   "in vec4 vColor;"
   "\n"
   "out vec4 fColor;"
   "\n"
   "void main()"
   "{"
   "	fColor = vColor;"
   "}";

float triangle_data[] =
{
    0.0f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
   -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f
};

void renderable_2d_init(struct renderable_2d_t* renderable_2d)
{
   struct vertex_array_t* vao            = &renderable_2d->vao;
   struct vertex_buffer_t* vbo           = &renderable_2d->vbo;
   struct shader_program_t* shader       = &renderable_2d->shader;
   struct vertex_attribute_t* attributes = renderable_2d->vertex_attributes;

   // Shader

   struct vertex_shader_t vertex_shader;
   struct fragment_shader_t fragment_shader;

   vertex_shader_init(&vertex_shader, vertex_shader_src);
   fragment_shader_init(&fragment_shader, fragment_shader_src);

   shader_program_init(shader, &vertex_shader, &fragment_shader);
   shader_program_bind_fragment_data_location(shader, 0, "fColor");
   shader_program_link(shader);

   vertex_shader_destroy(&vertex_shader);
   fragment_shader_destroy(&fragment_shader);

   // Vertex Array / Buffer

   vertex_array_init(vao, 1);
   vertex_buffer_init(vbo, 1);

   vertex_array_bind(vao);
   vertex_buffer_bind(vbo);

   vertex_buffer_buffer_data(vbo, triangle_data, sizeof(triangle_data), STATIC_DRAW);

   char* position_attrib_name = "position";
   char* color_attrib_name    = "color";

   int pos_attrib_index   = shader_program_get_attribute_location(shader, position_attrib_name);
   int color_attrib_index = shader_program_get_attribute_location(shader, color_attrib_name);

   vertex_attribute_init(&attributes[0], position_attrib_name, pos_attrib_index,   2, V_FLOAT, 0, 6*sizeof(float), 0);
   vertex_attribute_init(&attributes[1], color_attrib_name,    color_attrib_index, 4, V_FLOAT, 0, 6*sizeof(float), ((void*)(2*sizeof(float))));

   vertex_array_set_attribute(vao, &attributes[0]);
   vertex_array_set_attribute(vao, &attributes[1]);

	vertex_array_unbind();
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void renderable_2d_render(struct renderable_2d_t* renderable_2d)
{
   vertex_array_bind(&renderable_2d->vao);
   shader_program_use(&renderable_2d->shader);
   glDrawArrays(GL_TRIANGLES, 0, 3);
}

void renderable_2d_cleanup(struct renderable_2d_t* renderable_2d)
{
   vertex_array_destroy(&renderable_2d->vao, 1);
   vertex_buffer_destroy(&renderable_2d->vbo, 1);
   shader_program_destroy(&renderable_2d->shader);
}