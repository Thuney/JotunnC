#include "vertexbuffer.h"
#include "../../../core/graphics/datatypes/vertex_buffer_internal.h"
#include "log.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef DEBUG
  #include <stdio.h>
  #include <stdarg.h>
#endif

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Takes the oldest GL error flag and, in DEBUG builds, logs in_success_string
//  when it's clear or in_error_string and the error code when it isn't. Both
//  are formats for the variadic arguments. Other builds only clear the flag
//
//  in:     in_success_string, in_error_string
static void vertex_buffer_gl_check_error(
  const char* in_success_string,
  const char* in_error_string,
  ...);

// Converts a buffer data usage to GL's
//
//  in:     in_usage_type
//  ret:    The GL usage; GL_INVALID_ENUM when in_usage_type isn't valid
static GLenum vertex_buffer_gl_get_data_usage(
  enum vertex_buffer_data_usage_t in_usage_type);

// Creates in_num buffer names from out_vertex_buffer->vertex_buffer on
//
//  in:     in_num
//  out:    out_vertex_buffer
static void vertex_buffer_gl_init(
  struct vertex_buffer_t* out_vertex_buffer,
  int in_num);

// Binds a vertex buffer to GL_ARRAY_BUFFER
//
//  in:     in_vertex_buffer
static void vertex_buffer_gl_bind(
  const struct vertex_buffer_t* in_vertex_buffer);

// Replaces the data of the buffer bound to GL_ARRAY_BUFFER with a copy of
//  in_data_bytes from in_data, or with uninitialized storage when in_data is
//  NULL. in_vertex_buffer is only named in the commented-out error checks
//
//  in:     in_vertex_buffer, in_data, in_data_bytes, in_usage_type
static void vertex_buffer_gl_set_data(
  const struct vertex_buffer_t* in_vertex_buffer,
  const void* in_data,
  unsigned int in_data_bytes,
  enum vertex_buffer_data_usage_t in_usage_type);

// Overwrites the start of the data of the buffer bound to GL_ARRAY_BUFFER
//  with in_data_bytes from in_data. in_vertex_buffer is only named in the
//  commented-out error checks
//
//  in:     in_vertex_buffer, in_data, in_data_bytes
static void vertex_buffer_gl_set_sub_data(
  const struct vertex_buffer_t* in_vertex_buffer,
  const float* in_data,
  unsigned int in_data_bytes);

// Deletes in_num buffer names from inout_vertex_buffer->vertex_buffer on
//
//  in:     in_num
//  in/out: inout_vertex_buffer
static void vertex_buffer_gl_destroy(
  struct vertex_buffer_t* inout_vertex_buffer,
  int in_num);

// Creates in_num buffer names from out_element_buffer->element_buffer on
//
//  in:     in_num
//  out:    out_element_buffer
static void element_buffer_gl_init(
  struct element_buffer_t* out_element_buffer,
  int in_num);

// Binds an element buffer to GL_ELEMENT_ARRAY_BUFFER, which the bound vertex
//  array records
//
//  in:     in_element_buffer
static void element_buffer_gl_bind(
  const struct element_buffer_t* in_element_buffer);

// Replaces the data of the buffer bound to GL_ELEMENT_ARRAY_BUFFER with a
//  copy of in_data_bytes from in_index_data, or with uninitialized storage
//  when in_index_data is NULL. in_element_buffer is only named in the
//  commented-out error checks
//
//  in:     in_element_buffer, in_index_data, in_data_bytes, in_usage_type
static void element_buffer_gl_buffer_data(
  const struct element_buffer_t* in_element_buffer,
  const void* in_index_data,
  unsigned int in_data_bytes,
  enum vertex_buffer_data_usage_t in_usage_type);

// Deletes in_num buffer names from inout_element_buffer->element_buffer on
//
//  in:     in_num
//  in/out: inout_element_buffer
static void element_buffer_gl_destroy(
  struct element_buffer_t* inout_element_buffer,
  int in_num);

/*******************************************************************************
 *
 *   Function: vertex_buffer_internal_platform_init()
 *
 ******************************************************************************/
void vertex_buffer_internal_platform_init(
  struct vertex_buffer_t* out_vertex_buffer,
  int in_num)
{
  vertex_buffer_gl_init(
    out_vertex_buffer,
    in_num);
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_internal_platform_bind()
 *
 ******************************************************************************/
void vertex_buffer_internal_platform_bind(
  const struct vertex_buffer_t* in_vertex_buffer)
{
  vertex_buffer_gl_bind(in_vertex_buffer);
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_internal_platform_set_data()
 *
 ******************************************************************************/
void vertex_buffer_internal_platform_set_data(
  const struct vertex_buffer_t* in_vertex_buffer,
  const void* in_data,
  unsigned int in_data_bytes,
  enum vertex_buffer_data_usage_t in_usage_type)
{
  vertex_buffer_gl_set_data(
    in_vertex_buffer,
    in_data,
    in_data_bytes,
    in_usage_type);
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_internal_platform_set_sub_data()
 *
 ******************************************************************************/
void vertex_buffer_internal_platform_set_sub_data(
  const struct vertex_buffer_t* in_vertex_buffer,
  const float* in_data,
  unsigned int in_data_bytes)
{
  vertex_buffer_gl_set_sub_data(
    in_vertex_buffer,
    in_data,
    in_data_bytes);
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_internal_platform_destroy()
 *
 ******************************************************************************/
void vertex_buffer_internal_platform_destroy(
  struct vertex_buffer_t* inout_vertex_buffer,
  int in_num)
{
  vertex_buffer_gl_destroy(
    inout_vertex_buffer,
    in_num);
}


/*******************************************************************************
 *
 *   Function: element_buffer_internal_platform_init()
 *
 ******************************************************************************/
void element_buffer_internal_platform_init(
  struct element_buffer_t* out_element_buffer,
  int in_num)
{
  element_buffer_gl_init(
    out_element_buffer,
    in_num);
}

/*******************************************************************************
 *
 *   Function: element_buffer_internal_platform_bind()
 *
 ******************************************************************************/
void element_buffer_internal_platform_bind(
  const struct element_buffer_t* in_element_buffer)
{
  element_buffer_gl_bind(in_element_buffer);
}

/*******************************************************************************
 *
 *   Function: element_buffer_internal_platform_buffer_data()
 *
 ******************************************************************************/
void element_buffer_internal_platform_buffer_data(
  const struct element_buffer_t* in_element_buffer,
  const void* in_index_data,
  unsigned int in_data_bytes,
  enum vertex_buffer_data_usage_t in_usage_type)
{
  element_buffer_gl_buffer_data(
    in_element_buffer,
    in_index_data,
    in_data_bytes,
    in_usage_type);
}

/*******************************************************************************
 *
 *   Function: element_buffer_internal_platform_destroy()
 *
 ******************************************************************************/
void element_buffer_internal_platform_destroy(
  struct element_buffer_t* inout_element_buffer,
  int in_num)
{
  element_buffer_gl_destroy(
    inout_element_buffer,
    in_num);
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_gl_check_error()
 *
 ******************************************************************************/
static void vertex_buffer_gl_check_error(
  const char* in_success_string,
  const char* in_error_string,
  ...)
{
  GLenum _gl_error = GL_NO_ERROR;
  _gl_error = glGetError();

#ifdef DEBUG
  va_list args;
  va_start(
    args,
    in_error_string);

  if (_gl_error == GL_NO_ERROR) {
    log_write_va(
      LOG_LEVEL_DEBUG,
      in_success_string,
      args);
  }
  else {
    log_write_va(
      LOG_LEVEL_ERROR,
      in_error_string,
      args);
    log_write(
      LOG_LEVEL_ERROR,
      "--> Error Code: %d",
      _gl_error);
  }

  va_end(args);
#endif
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_gl_get_data_usage()
 *
 ******************************************************************************/
static GLenum vertex_buffer_gl_get_data_usage(
  enum vertex_buffer_data_usage_t in_usage_type)
{
  GLenum gl_usage_type;

  switch (in_usage_type)
  {
    case VERTEX_BUFFER_DATA_USAGE_STREAM_DRAW:
      gl_usage_type = GL_STREAM_DRAW;
      break;
    case VERTEX_BUFFER_DATA_USAGE_STREAM_READ:
      gl_usage_type = GL_STREAM_READ;
      break;
    case VERTEX_BUFFER_DATA_USAGE_STREAM_COPY:
      gl_usage_type = GL_STREAM_COPY;
      break;
    case VERTEX_BUFFER_DATA_USAGE_STATIC_DRAW:
      gl_usage_type = GL_STATIC_DRAW;
      break;
    case VERTEX_BUFFER_DATA_USAGE_STATIC_READ:
      gl_usage_type = GL_STATIC_READ;
      break;
    case VERTEX_BUFFER_DATA_USAGE_STATIC_COPY:
      gl_usage_type = GL_STATIC_COPY;
      break;
    case VERTEX_BUFFER_DATA_USAGE_DYNAMIC_DRAW:
      gl_usage_type = GL_DYNAMIC_DRAW;
      break;
    case VERTEX_BUFFER_DATA_USAGE_DYNAMIC_READ:
      gl_usage_type = GL_DYNAMIC_READ;
      break;
    case VERTEX_BUFFER_DATA_USAGE_DYNAMIC_COPY:
      gl_usage_type = GL_DYNAMIC_COPY;
      break;
    default:
      gl_usage_type = GL_INVALID_ENUM;
      break;
  }

  return gl_usage_type;
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_gl_init()
 *
 ******************************************************************************/
static void vertex_buffer_gl_init(
  struct vertex_buffer_t* out_vertex_buffer,
  int in_num)
{
  glGenBuffers(
    in_num,
    &(out_vertex_buffer->vertex_buffer));

  // vertex_buffer_gl_check_error(
  //   "Vertex Buffer initialized with ID %d\n",
  //   "Error occurred during Vertex Buffer generation: [ID %d]\n",
  //   vertex_buffer->vertex_buffer);
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_gl_bind()
 *
 ******************************************************************************/
static void vertex_buffer_gl_bind(
  const struct vertex_buffer_t* in_vertex_buffer)
{
  glBindBuffer(
    GL_ARRAY_BUFFER,
    in_vertex_buffer->vertex_buffer);

  // vertex_buffer_gl_check_error(
  //   "Bound Vertex Buffer ID %d\n",
  //   "Error occurred during Vertex Buffer binding: [ID %d]\n",
  //   vertex_buffer->vertex_buffer);
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_gl_set_data()
 *
 ******************************************************************************/
static void vertex_buffer_gl_set_data(
  const struct vertex_buffer_t* in_vertex_buffer,
  const void* in_data,
  unsigned int in_data_bytes,
  enum vertex_buffer_data_usage_t in_usage_type)
{
  glBufferData(
    GL_ARRAY_BUFFER,
    in_data_bytes,
    in_data,
    vertex_buffer_gl_get_data_usage(in_usage_type));

  // vertex_buffer_gl_check_error(
  //   "Buffered %d bytes of data in Vertex Buffer ID %d\n",
  //  "Error occurred trying to buffer %d bytes of data in Vertex Buffer: [ID
  //  %d]\n",
  //   data_bytes,
  //   vertex_buffer->vertex_buffer);
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_gl_set_sub_data()
 *
 ******************************************************************************/
static void vertex_buffer_gl_set_sub_data(
  const struct vertex_buffer_t* in_vertex_buffer,
  const float* in_data,
  unsigned int in_data_bytes)
{
  glBufferSubData(
    GL_ARRAY_BUFFER,
    0,
    in_data_bytes,
    in_data);

  // vertex_buffer_gl_check_error(
  //   "Buffered %d bytes of sub-data in Vertex Buffer ID %d\n",
  //  "Error occurred trying to buffer %d bytes of sub-data in Vertex Buffer:
  //  [ID %d]\n",
  //   data_bytes,
  //   vertex_buffer->vertex_buffer);
}

/*******************************************************************************
 *
 *   Function: vertex_buffer_gl_destroy()
 *
 ******************************************************************************/
static void vertex_buffer_gl_destroy(
  struct vertex_buffer_t* inout_vertex_buffer,
  int in_num)
{
  glDeleteBuffers(
    in_num,
    &(inout_vertex_buffer->vertex_buffer));
}

/*******************************************************************************
 *
 *   Function: element_buffer_gl_init()
 *
 ******************************************************************************/
static void element_buffer_gl_init(
  struct element_buffer_t* out_element_buffer,
  int in_num)
{
  glGenBuffers(
    in_num,
    &(out_element_buffer->element_buffer));

  // vertex_buffer_gl_check_error(
  //   "Element Buffer initialized with ID %d\n",
  //   "Error occurred during Element Buffer generation: [ID %d]\n",
  //   element_buffer->element_buffer);
}

/*******************************************************************************
 *
 *   Function: element_buffer_gl_bind()
 *
 ******************************************************************************/
static void element_buffer_gl_bind(
  const struct element_buffer_t* in_element_buffer)
{
  glBindBuffer(
    GL_ELEMENT_ARRAY_BUFFER,
    in_element_buffer->element_buffer);

  // vertex_buffer_gl_check_error(
  //   "Bound Element Buffer ID %d\n",
  //   "Error occurred during Element Buffer binding: [ID %d]\n",
  //   element_buffer->element_buffer);
}

/*******************************************************************************
 *
 *   Function: element_buffer_gl_buffer_data()
 *
 ******************************************************************************/
static void element_buffer_gl_buffer_data(
  const struct element_buffer_t* in_element_buffer,
  const void* in_index_data,
  unsigned int in_data_bytes,
  enum vertex_buffer_data_usage_t in_usage_type)
{
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    in_data_bytes,
    in_index_data,
    vertex_buffer_gl_get_data_usage(in_usage_type));

  // vertex_buffer_gl_check_error(
  //   "Buffered %d bytes of data in Element Buffer ID %d\n",
  //  "Error occurred trying to buffer %d bytes of data in Element Buffer: [ID
  //  %d]\n",
  //   data_bytes,
  //   element_buffer->element_buffer);
}

/*******************************************************************************
 *
 *   Function: element_buffer_gl_destroy()
 *
 ******************************************************************************/
static void element_buffer_gl_destroy(
  struct element_buffer_t* inout_element_buffer,
  int in_num)
{
  glDeleteBuffers(
    in_num,
    &(inout_element_buffer->element_buffer));
}
