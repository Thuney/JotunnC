#include "vertexbuffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

static int gl_vertex_buffer_init(struct vertex_buffer_t* vertex_buffer, int num)
{
   glGenBuffers(num, &vertex_buffer->vertex_buffer);

   return 0;
}

static void gl_vertex_buffer_bind(struct vertex_buffer_t* vertex_buffer)
{
   glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->vertex_buffer);
}

static GLenum get_gl_data_usage_type(enum vertex_buffer_data_usage_t usage_type)
{
   GLenum gl_usage_type;

   switch(usage_type)
   {
      case STREAM_DRAW:
         gl_usage_type = GL_STREAM_DRAW;
      break;
      case STREAM_READ:
         gl_usage_type = GL_STREAM_READ;
      break;
      case STREAM_COPY:
         gl_usage_type = GL_STREAM_COPY;
      break;
      case STATIC_DRAW:
         gl_usage_type = GL_STATIC_DRAW;
      break;
      case STATIC_READ:
         gl_usage_type = GL_STATIC_READ;
      break;
      case STATIC_COPY:
         gl_usage_type = GL_STATIC_COPY;
      break;
      case DYNAMIC_DRAW:
         gl_usage_type = GL_DYNAMIC_DRAW;
      break;
      case DYNAMIC_READ:
         gl_usage_type = GL_DYNAMIC_READ;
      break;
      case DYNAMIC_COPY:
         gl_usage_type = GL_DYNAMIC_COPY;
      break;
      default:
         gl_usage_type = GL_INVALID_ENUM;
      break;
   }
   
   return gl_usage_type;
}

static void gl_vertex_buffer_buffer_data(struct vertex_buffer_t* vertex_buffer, float* data, unsigned int data_bytes, enum vertex_buffer_data_usage_t usage_type)
{
   glBufferData(GL_ARRAY_BUFFER, data_bytes, data, get_gl_data_usage_type(usage_type));
}

static void gl_vertex_buffer_destroy(struct vertex_buffer_t* vertex_buffer, int num)
{
   glDeleteBuffers(num, &vertex_buffer->vertex_buffer);
}

int platform_vertex_buffer_init(struct vertex_buffer_t* vertex_buffer, int num)
{
   return gl_vertex_buffer_init(vertex_buffer, num);
}

void platform_vertex_buffer_bind(struct vertex_buffer_t* vertex_buffer)
{
   gl_vertex_buffer_bind(vertex_buffer);
}

void platform_vertex_buffer_buffer_data(struct vertex_buffer_t* vertex_buffer, float* data, unsigned int data_bytes, enum vertex_buffer_data_usage_t usage_type)
{
   gl_vertex_buffer_buffer_data(vertex_buffer, data, data_bytes, usage_type);
}

void platform_vertex_buffer_destroy(struct vertex_buffer_t* vertex_buffer, int num)
{
   gl_vertex_buffer_destroy(vertex_buffer, num);
}