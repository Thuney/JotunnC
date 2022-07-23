#include "renderable_2d.h"

/**************************************************
 * 
 *  Variables
 * 
 **************************************************/

static const char* vertex_shader_src =
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

static const char* fragment_shader_src =
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

static unsigned int initialized = 0;
static unsigned int instances   = 0;

static struct shader_program_t s_renderable_2d_shader_program;

static void renderable_2d_shader_init(struct shader_program_t* shader)
{
   struct vertex_shader_t vertex_shader;
   struct fragment_shader_t fragment_shader;

   vertex_shader_init(&vertex_shader, vertex_shader_src);
   fragment_shader_init(&fragment_shader, fragment_shader_src);

   shader_program_init(shader, &vertex_shader, &fragment_shader);
   shader_program_bind_fragment_data_location(shader, 0, "fColor");
   shader_program_link(shader);

   vertex_shader_destroy(&vertex_shader);
   fragment_shader_destroy(&fragment_shader);
}

static void renderable_2d_vao_init(struct renderable_2d_t* renderable_2d, float* vertex_data, unsigned int num_vertices, unsigned int* index_data, unsigned int num_indices)
{
   struct vertex_array_t* vao              = &renderable_2d->vao;
   struct vertex_buffer_t* vbo             = &renderable_2d->vbo;
   struct element_buffer_t* ebo            = &renderable_2d->ebo;
   struct vertex_attribute_t* attributes   = renderable_2d->vertex_attributes;
   struct shader_program_t* shader_program = renderable_2d->shader_program;

   vertex_array_init(vao, 1);
   vertex_buffer_init(vbo, 1);
   element_buffer_init(ebo, 1);

   vertex_array_bind(vao);
   vertex_buffer_bind(vbo);
   element_buffer_bind(ebo);

   char* position_attrib_name = "position";
   char* color_attrib_name    = "color";

   int pos_attrib_index   = shader_program_get_attribute_location(shader_program, position_attrib_name);
   int color_attrib_index = shader_program_get_attribute_location(shader_program, color_attrib_name);

   int pos_attrib_size   = 2;
   int color_attrib_size = 4;
   int vertex_size_bytes = (pos_attrib_size+color_attrib_size)*sizeof(float);

   vertex_attribute_init(&attributes[POSITION], position_attrib_name, pos_attrib_index, pos_attrib_size, V_FLOAT, 0, vertex_size_bytes, 0);
   vertex_attribute_init(&attributes[COLOR], color_attrib_name, color_attrib_index, color_attrib_size, V_FLOAT, 0, vertex_size_bytes, ((void*)(pos_attrib_size*sizeof(float))));

   vertex_buffer_buffer_data(vbo, vertex_data, vertex_size_bytes*num_vertices, STATIC_DRAW);
   element_buffer_buffer_data(ebo, index_data, num_indices*sizeof(unsigned int), STATIC_DRAW);

   vertex_array_set_attribute(vao, &attributes[POSITION]);
   vertex_array_set_attribute(vao, &attributes[COLOR]);

	vertex_array_unbind();
}

void renderable_2d_init(struct renderable_2d_t* renderable_2d, float* vertex_data, unsigned int num_vertices, unsigned int* index_data, unsigned int num_indices)
{
   renderable_2d->vertex_data  = vertex_data;
   renderable_2d->num_vertices = num_vertices;
   renderable_2d->index_data   = index_data;
   renderable_2d->num_indices  = num_indices;

   if(!initialized)
   {
      renderable_2d_shader_init(&s_renderable_2d_shader_program);
      instances = 0;
      initialized = 1;
   }
   instances++;
   renderable_2d->shader_program = &s_renderable_2d_shader_program;

   // Vertex Array / Buffer

   renderable_2d_vao_init(renderable_2d, vertex_data, num_vertices, index_data, num_indices);
}

void renderable_2d_cleanup(struct renderable_2d_t* renderable_2d)
{
   vertex_array_destroy(&renderable_2d->vao, 1);
   vertex_buffer_destroy(&renderable_2d->vbo, 1);

   if(--instances == 0) 
   {
      shader_program_destroy(&s_renderable_2d_shader_program);
      initialized = 0;
   }
}