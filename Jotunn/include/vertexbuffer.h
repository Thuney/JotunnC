#pragma once

enum vertex_buffer_data_usage_t
{
   STREAM_DRAW = 0x00,
   STREAM_READ,
   STREAM_COPY,
   STATIC_DRAW,
   STATIC_READ,
   STATIC_COPY,
   DYNAMIC_DRAW,
   DYNAMIC_READ,
   DYNAMIC_COPY
};

struct vertex_buffer_t
{
   unsigned int vertex_buffer;
};

int vertex_buffer_init(struct vertex_buffer_t* vertex_buffer, int num);
void vertex_buffer_bind(struct vertex_buffer_t* vertex_buffer);
void vertex_buffer_buffer_data(struct vertex_buffer_t* vertex_buffer, float* data, unsigned int data_bytes, enum vertex_buffer_data_usage_t usage_type);
void vertex_buffer_destroy(struct vertex_buffer_t* vertex_buffer, int num);