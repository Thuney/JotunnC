#include "vertexbuffer.h"

int platform_vertex_buffer_init(struct vertex_buffer_t* vertex_buffer, int num)
{
   return 0;
}

void platform_vertex_buffer_bind(struct vertex_buffer_t* vertex_buffer)
{
   return 0;
}

void platform_vertex_buffer_buffer_data(struct vertex_buffer_t* vertex_buffer, float* data, unsigned int data_bytes, enum buffer_data_usage_t usage_type)
{
   return 0;
}

void platform_vertex_buffer_destroy(struct vertex_buffer_t* vertex_buffer, int num)
{
   return 0;
}

int platform_element_buffer_init(struct element_buffer_t* element_buffer, int num)
{
   return 0;
}

void platform_element_buffer_bind(struct element_buffer_t* element_buffer)
{
   return 0;
}

void platform_element_buffer_buffer_data(struct element_buffer_t* element_buffer, unsigned int* index_data, unsigned int data_bytes, enum buffer_data_usage_t usage_type)
{
   return 0;
}

void platform_element_buffer_destroy(struct element_buffer_t* element_buffer, int num)
{
   return 0;
}