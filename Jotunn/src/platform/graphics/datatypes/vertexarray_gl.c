#include "vertexarray.h"

#include <GL/glew.h>

#ifdef DEBUG
  #include <stdio.h>
  #include <stdarg.h>

  static void gl_error_check(
    const char* success_string,
    const char* error_string,
    ...);
#endif

static void gl_vertex_array_init(
  struct vertex_array_t* vertex_array,
  int num);

static void gl_vertex_array_bind(
  struct vertex_array_t* vertex_array);

void gl_vertex_array_unbind();

static GLenum get_gl_attribute_data_type(
  enum vertex_attribute_data_type_t data_type);

static void gl_vertex_array_set_attribute(
  struct vertex_array_t* vertex_array,
  struct vertex_attribute_t* vertex_attribute);

static void gl_vertex_array_destroy(
  struct vertex_array_t* vertex_array,
  int num);

// -----------------------------------------------------------------------------

void platform_vertex_array_init(
  struct vertex_array_t* vertex_array,
  int num)
{
  gl_vertex_array_init(vertex_array, num);
}

void platform_vertex_array_bind(
  struct vertex_array_t* vertex_array)
{
  gl_vertex_array_bind(vertex_array);
}

void platform_vertex_array_unbind()
{
  gl_vertex_array_unbind();
}

void platform_vertex_array_set_attribute(
  struct vertex_array_t* vertex_array,
  struct vertex_attribute_t* vertex_attribute)
{
  gl_vertex_array_set_attribute(vertex_array, vertex_attribute);
}

void platform_vertex_array_destroy(
  struct vertex_array_t* vertex_array,
  int num)
{
  gl_vertex_array_destroy(vertex_array, num);
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

static void gl_vertex_array_init(
  struct vertex_array_t* vertex_array,
  int num)
{
  glGenVertexArrays(num, &(vertex_array->vertex_array));

  gl_error_check(
    "Vertex Array initialized with ID %d\n",
    "Error occurred during Vertex Array generation: [ID %d]\n",
    vertex_array->vertex_array);
}

static void gl_vertex_array_bind(
  struct vertex_array_t* vertex_array)
{
  gl_error_check(
    "No error before vertex array bind\n",
    "Error occurred prior to vertex array bind\n");

  glBindVertexArray(vertex_array->vertex_array);

  gl_error_check(
    "Bound Vertex Array ID %d\n",
    "Error occurred during Vertex Array binding: [ID %d]\n",
    vertex_array->vertex_array);
}

void gl_vertex_array_unbind()
{
  glBindVertexArray(0);
}

static GLenum get_gl_attribute_data_type(
  enum vertex_attribute_data_type_t data_type)
{
  GLenum gl_data_type;

  switch (data_type)
  {
    case V_BYTE:
      gl_data_type = GL_BYTE;
      break;
    case V_UNSIGNED_BYTE:
      gl_data_type = GL_UNSIGNED_BYTE;
      break;
    case V_SHORT:
      gl_data_type = GL_SHORT;
      break;
    case V_UNSIGNED_SHORT:
      gl_data_type = GL_UNSIGNED_SHORT;
      break;
    case V_INT:
      gl_data_type = GL_INT;
      break;
    case V_UNSIGNED_INT:
      gl_data_type = GL_UNSIGNED_INT;
      break;
    case V_HALF_FLOAT:
      gl_data_type = GL_HALF_FLOAT;
      break;
    case V_FLOAT:
      gl_data_type = GL_FLOAT;
      break;
    case V_DOUBLE:
      gl_data_type = GL_DOUBLE;
      break;
    case V_FIXED:
      gl_data_type = GL_FIXED;
      break;
    default:
      gl_data_type = GL_INVALID_ENUM;
      break;
  }

  return gl_data_type;
}

static void gl_vertex_array_set_attribute(
  struct vertex_array_t* vertex_array,
  struct vertex_attribute_t* vertex_attribute)
{
  // Specify how to interpret vertex data in the array
  glVertexAttribPointer(
    vertex_attribute->index,
    vertex_attribute->size,
    get_gl_attribute_data_type(vertex_attribute->data_type),
    vertex_attribute->should_normalize,
    vertex_attribute->stride,
    vertex_attribute->ptr_offset_to_attrib);

  gl_error_check(
    "Set attribute '%s' in Vertex Array ID %d\n",
    "Error occurred in setting of vertex attribute '%s' in Vertex Array ID %d\n",
    vertex_attribute->attribute_name,
    vertex_array->vertex_array);

  glEnableVertexAttribArray(vertex_attribute->index);

  gl_error_check(
    "Enabled Vertex Attribute '%s' in Vertex Array ID %d\n",
    "Error occurred in enabling of vertex attribute '%s' in Vertex Array ID %d\n",
    vertex_attribute->attribute_name,
    vertex_array->vertex_array);
}

static void gl_vertex_array_destroy(
  struct vertex_array_t* vertex_array,
  int num)
{
  glDeleteVertexArrays(num, &vertex_array->vertex_array);
}
