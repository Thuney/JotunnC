#include "vertexbuffer.h"

extern int platform_vertex_buffer_init(struct vertex_buffer_t* vertex_buffer, int num);
extern void platform_vertex_buffer_bind(struct vertex_buffer_t* vertex_buffer);
extern void platform_vertex_buffer_buffer_data(struct vertex_buffer_t* vertex_buffer, float* data, unsigned int data_bytes, enum vertex_buffer_data_usage_t usage_type);
extern void platform_vertex_buffer_destroy(struct vertex_buffer_t* vertex_buffer, int num);

int vertex_buffer_init(struct vertex_buffer_t* vertex_buffer, int num)
{
   return platform_vertex_buffer_init(vertex_buffer, num);
}

void vertex_buffer_bind(struct vertex_buffer_t* vertex_buffer)
{
   platform_vertex_buffer_bind(vertex_buffer);
}

void vertex_buffer_buffer_data(struct vertex_buffer_t* vertex_buffer, float* data, unsigned int data_bytes, enum vertex_buffer_data_usage_t usage_type)
{
   platform_vertex_buffer_buffer_data(vertex_buffer, data, data_bytes, usage_type);
}

void vertex_buffer_destroy(struct vertex_buffer_t* vertex_buffer, int num)
{
   platform_vertex_buffer_destroy(vertex_buffer, num);
}