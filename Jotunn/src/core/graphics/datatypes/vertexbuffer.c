#include "vertexbuffer.h"

extern int platform_vertex_buffer_init(struct vertex_buffer_t* vertex_buffer, int num);
extern void platform_vertex_buffer_bind(struct vertex_buffer_t* vertex_buffer);
extern void platform_vertex_buffer_buffer_data(struct vertex_buffer_t* vertex_buffer, float* data, unsigned int data_bytes, enum buffer_data_usage_t usage_type);
extern void platform_vertex_buffer_destroy(struct vertex_buffer_t* vertex_buffer, int num);

extern int platform_element_buffer_init(struct element_buffer_t* element_buffer, int num);
extern void platform_element_buffer_bind(struct element_buffer_t* element_buffer);
extern void platform_element_buffer_buffer_data(struct element_buffer_t* element_buffer, unsigned int* data, unsigned int data_bytes, enum buffer_data_usage_t usage_type);
extern void platform_element_buffer_destroy(struct element_buffer_t* element_buffer, int num);

//

int vertex_buffer_init(struct vertex_buffer_t* vertex_buffer, int num)
{
   return platform_vertex_buffer_init(vertex_buffer, num);
}

void vertex_buffer_bind(struct vertex_buffer_t* vertex_buffer)
{
   platform_vertex_buffer_bind(vertex_buffer);
}

void vertex_buffer_buffer_data(struct vertex_buffer_t* vertex_buffer, float* data, unsigned int data_bytes, enum buffer_data_usage_t usage_type)
{
   platform_vertex_buffer_buffer_data(vertex_buffer, data, data_bytes, usage_type);
}

void vertex_buffer_destroy(struct vertex_buffer_t* vertex_buffer, int num)
{
   platform_vertex_buffer_destroy(vertex_buffer, num);
}

//

int element_buffer_init(struct element_buffer_t* element_buffer, int num)
{
   return platform_element_buffer_init(element_buffer, num);
}

void element_buffer_bind(struct element_buffer_t* element_buffer)
{
   platform_element_buffer_bind(element_buffer);
}

void element_buffer_buffer_data(struct element_buffer_t* element_buffer, unsigned int* index_data, unsigned int data_bytes, enum buffer_data_usage_t usage_type)
{
   platform_element_buffer_buffer_data(element_buffer, index_data, data_bytes, usage_type);
}

void element_buffer_destroy(struct element_buffer_t* element_buffer, int num)
{
   platform_element_buffer_destroy(element_buffer, num);
}