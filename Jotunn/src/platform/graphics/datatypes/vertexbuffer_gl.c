#include "vertexbuffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef DEBUG
  #include <stdio.h>
  #include <stdarg.h>

  static void gl_error_check(
    const char* success_string,
    const char* error_string,
    ...);
#endif

static GLenum get_gl_data_usage_type(
  enum buffer_data_usage_t usage_type);

static void gl_vertex_buffer_init(
  struct vertex_buffer_t* vertex_buffer,
  int num);

static void gl_vertex_buffer_bind(
  struct vertex_buffer_t* vertex_buffer);

static void gl_vertex_buffer_buffer_data(
  struct vertex_buffer_t* vertex_buffer,
  float* data,
  unsigned int data_bytes,
  enum buffer_data_usage_t usage_type);

static void gl_vertex_buffer_buffer_sub_data(
  struct vertex_buffer_t* vertex_buffer,
  float* data,
  unsigned int data_bytes);

static void gl_vertex_buffer_destroy(
  struct vertex_buffer_t* vertex_buffer,
  int num);

static void gl_element_buffer_init(
  struct element_buffer_t* element_buffer,
  int num);

static void gl_element_buffer_bind(
  struct element_buffer_t* element_buffer);

static void gl_element_buffer_buffer_data(
  struct element_buffer_t* element_buffer,
  unsigned int* data,
  unsigned int data_bytes,
  enum buffer_data_usage_t usage_type);

static void gl_element_buffer_destroy(
  struct element_buffer_t* element_buffer,
  int num);

// -----------------------------------------------------------------------------

void platform_vertex_buffer_init(
  struct vertex_buffer_t* vertex_buffer,
  int num)
{
  gl_vertex_buffer_init(vertex_buffer, num);
}

void platform_vertex_buffer_bind(
  struct vertex_buffer_t* vertex_buffer)
{
  gl_vertex_buffer_bind(vertex_buffer);
}

void platform_vertex_buffer_buffer_data(
  struct vertex_buffer_t* vertex_buffer,
  float* data,
  unsigned int data_bytes,
  enum buffer_data_usage_t usage_type)
{
  gl_vertex_buffer_buffer_data(vertex_buffer, data, data_bytes, usage_type);
}

void platform_vertex_buffer_buffer_sub_data(
  struct vertex_buffer_t* vertex_buffer,
  float* data,
  unsigned int data_bytes)
{
  gl_vertex_buffer_buffer_sub_data(vertex_buffer, data, data_bytes);
}

void platform_vertex_buffer_destroy(
  struct vertex_buffer_t* vertex_buffer,
  int num)
{
  gl_vertex_buffer_destroy(vertex_buffer, num);
}

//

void platform_element_buffer_init(
  struct element_buffer_t* element_buffer,
  int num)
{
  gl_element_buffer_init(element_buffer, num);
}

void platform_element_buffer_bind(
  struct element_buffer_t* element_buffer)
{
  gl_element_buffer_bind(element_buffer);
}

void platform_element_buffer_buffer_data(
  struct element_buffer_t* element_buffer,
  unsigned int* index_data,
  unsigned int data_bytes,
  enum buffer_data_usage_t usage_type)
{
  gl_element_buffer_buffer_data(
    element_buffer, index_data, data_bytes, usage_type);
}

void platform_element_buffer_destroy(
  struct element_buffer_t* element_buffer,
  int num)
{
  gl_element_buffer_destroy(element_buffer, num);
}

// -----------------------------------------------------------------------------

static void gl_error_check(
  const char* success_string,
  const char* error_string,
  ...)
{
  GLenum _gl_error = GL_NO_ERROR;
  _gl_error = glGetError();

#ifdef DEBUG
  va_list args;
  va_start(args, error_string);

  if (_gl_error == GL_NO_ERROR)
  {
    vprintf(success_string, args);
  }
  else
  {
    vprintf(error_string, args);
    printf("--> Error Code: %d\n", _gl_error);
  }

  va_end(args);
#endif
}

static GLenum get_gl_data_usage_type(
  enum buffer_data_usage_t usage_type)
{
  GLenum gl_usage_type;

  switch (usage_type)
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

static void gl_vertex_buffer_init(
  struct vertex_buffer_t* vertex_buffer,
  int num)
{
  glGenBuffers(num, &(vertex_buffer->vertex_buffer));

  gl_error_check(
    "Vertex Buffer initialized with ID %d\n",
    "Error occurred during Vertex Buffer generation: [ID %d]\n",
    vertex_buffer->vertex_buffer);
}

static void gl_vertex_buffer_bind(
  struct vertex_buffer_t* vertex_buffer)
{
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->vertex_buffer);

  gl_error_check(
    "Bound Vertex Buffer ID %d\n",
    "Error occurred during Vertex Buffer binding: [ID %d]\n",
    vertex_buffer->vertex_buffer);
}

static void gl_vertex_buffer_buffer_data(
  struct vertex_buffer_t* vertex_buffer,
  float* data,
  unsigned int data_bytes,
  enum buffer_data_usage_t usage_type)
{
  glBufferData(
    GL_ARRAY_BUFFER, data_bytes, data, get_gl_data_usage_type(usage_type));

  gl_error_check(
    "Buffered %d bytes of data in Vertex Buffer ID %d\n",
    "Error occurred trying to buffer %d bytes of data in Vertex Buffer: [ID %d]\n",
    data_bytes,
    vertex_buffer->vertex_buffer);
}

static void gl_vertex_buffer_buffer_sub_data(
  struct vertex_buffer_t* vertex_buffer,
  float* data,
  unsigned int data_bytes)
{
  glBufferSubData(GL_ARRAY_BUFFER, 0, data_bytes, data);

  gl_error_check(
    "Buffered %d bytes of sub-data in Vertex Buffer ID %d\n",
    "Error occurred trying to buffer %d bytes of sub-data in Vertex Buffer: [ID %d]\n",
    data_bytes,
    vertex_buffer->vertex_buffer);
}

static void gl_vertex_buffer_destroy(
  struct vertex_buffer_t* vertex_buffer,
  int num)
{
  glDeleteBuffers(num, &(vertex_buffer->vertex_buffer));
}

static void gl_element_buffer_init(
  struct element_buffer_t* element_buffer,
  int num)
{
  glGenBuffers(num, &(element_buffer->element_buffer));

  gl_error_check(
    "Element Buffer initialized with ID %d\n",
    "Error occurred during Element Buffer generation: [ID %d]\n",
    element_buffer->element_buffer);
}

static void gl_element_buffer_bind(
  struct element_buffer_t* element_buffer)
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer->element_buffer);

  gl_error_check(
    "Bound Element Buffer ID %d\n",
    "Error occurred during Element Buffer binding: [ID %d]\n",
    element_buffer->element_buffer);
}

static void gl_element_buffer_buffer_data(
  struct element_buffer_t* element_buffer,
  unsigned int* data,
  unsigned int data_bytes,
  enum buffer_data_usage_t usage_type)
{
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    data_bytes,
    data,
    get_gl_data_usage_type(usage_type));

  gl_error_check(
    "Buffered %d bytes of data in Element Buffer ID %d\n",
    "Error occurred trying to buffer %d bytes of data in Element Buffer: [ID %d]\n",
    data_bytes,
    element_buffer->element_buffer);
}

static void gl_element_buffer_destroy(
  struct element_buffer_t* element_buffer,
  int num)
{
  glDeleteBuffers(num, &element_buffer->element_buffer);
}
