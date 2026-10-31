#include "vertexarray.h"
#include "../../../core/graphics/datatypes/vertex_array_internal.h"
#include "log.h"

#include <GL/glew.h>

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
static void vertex_array_gl_check_error(
  const char* in_success_string,
  const char* in_error_string,
  ...);

// Creates in_num vertex array names from out_vertex_array->vertex_array on
//
//  in:     in_num
//  out:    out_vertex_array
static void vertex_array_gl_init(
  struct vertex_array_t* out_vertex_array,
  const int in_num);

// Makes a vertex array the one drawn from and configured
//
//  in:     in_vertex_array
static void vertex_array_gl_bind(
  const struct vertex_array_t* in_vertex_array);

// Leaves no vertex array bound
static void vertex_array_gl_unbind(void);

// Converts a vertex attribute's data type to GL's
//
//  in:     in_data_type
//  ret:    The GL data type; GL_INVALID_ENUM when in_data_type isn't valid
static GLenum vertex_array_gl_get_attribute_data_type(
  const enum vertex_attribute_data_type_t in_data_type);

// Sets where and how a vertex attribute is read, and enables it, in the bound
//  vertex array, reading from the buffer bound to GL_ARRAY_BUFFER.
//  in_vertex_array is only named in the commented-out error checks
//
//  in:     in_vertex_array, in_vertex_attribute
static void vertex_array_gl_set_attribute(
  const struct vertex_array_t* in_vertex_array,
  const struct vertex_attribute_t* in_vertex_attribute);

// Deletes in_num vertex array names from inout_vertex_array->vertex_array on
//
//  in:     in_num
//  in/out: inout_vertex_array
static void vertex_array_gl_destroy(
  struct vertex_array_t* inout_vertex_array,
  const int in_num);

/*******************************************************************************
 *
 *   Function: vertex_array_internal_platform_init()
 *
 ******************************************************************************/
void vertex_array_internal_platform_init(
  struct vertex_array_t* out_vertex_array,
  const int in_num)
{
  vertex_array_gl_init(
    out_vertex_array,
    in_num);
}

/*******************************************************************************
 *
 *   Function: vertex_array_internal_platform_bind()
 *
 ******************************************************************************/
void vertex_array_internal_platform_bind(
  const struct vertex_array_t* in_vertex_array)
{
  vertex_array_gl_bind(in_vertex_array);
}

/*******************************************************************************
 *
 *   Function: vertex_array_internal_platform_unbind()
 *
 ******************************************************************************/
void vertex_array_internal_platform_unbind(void)
{
  vertex_array_gl_unbind();
}

/*******************************************************************************
 *
 *   Function: vertex_array_internal_platform_set_attribute()
 *
 ******************************************************************************/
void vertex_array_internal_platform_set_attribute(
  const struct vertex_array_t* in_vertex_array,
  const struct vertex_attribute_t* in_vertex_attribute)
{
  vertex_array_gl_set_attribute(
    in_vertex_array,
    in_vertex_attribute);
}

/*******************************************************************************
 *
 *   Function: vertex_array_internal_platform_destroy()
 *
 ******************************************************************************/
void vertex_array_internal_platform_destroy(
  struct vertex_array_t* inout_vertex_array,
  const int in_num)
{
  vertex_array_gl_destroy(
    inout_vertex_array,
    in_num);
}

/*******************************************************************************
 *
 *   Function: vertex_array_gl_check_error()
 *
 ******************************************************************************/
static void vertex_array_gl_check_error(
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
 *   Function: vertex_array_gl_init()
 *
 ******************************************************************************/
static void vertex_array_gl_init(
  struct vertex_array_t* out_vertex_array,
  const int in_num)
{
  glGenVertexArrays(
    in_num,
    &(out_vertex_array->vertex_array));

  // vertex_array_gl_check_error(
  //   "Vertex Array initialized with ID %d\n",
  //   "Error occurred during Vertex Array generation: [ID %d]\n",
  //   vertex_array->vertex_array);
}

/*******************************************************************************
 *
 *   Function: vertex_array_gl_bind()
 *
 ******************************************************************************/
static void vertex_array_gl_bind(
  const struct vertex_array_t* in_vertex_array)
{
  // vertex_array_gl_check_error(
  //   "No error before vertex array bind\n",
  //   "Error occurred prior to vertex array bind\n");

  glBindVertexArray(in_vertex_array->vertex_array);

  // vertex_array_gl_check_error(
  //   "Bound Vertex Array ID %d\n",
  //   "Error occurred during Vertex Array binding: [ID %d]\n",
  //   vertex_array->vertex_array);
}

/*******************************************************************************
 *
 *   Function: vertex_array_gl_unbind()
 *
 ******************************************************************************/
static void vertex_array_gl_unbind(void)
{
  glBindVertexArray(0);
}

/*******************************************************************************
 *
 *   Function: vertex_array_gl_get_attribute_data_type()
 *
 ******************************************************************************/
static GLenum vertex_array_gl_get_attribute_data_type(
  const enum vertex_attribute_data_type_t in_data_type)
{
  GLenum gl_data_type;

  switch (in_data_type)
  {
    case VERTEX_ATTRIBUTE_DATA_TYPE_BYTE:
      gl_data_type = GL_BYTE;
      break;
    case VERTEX_ATTRIBUTE_DATA_TYPE_UNSIGNED_BYTE:
      gl_data_type = GL_UNSIGNED_BYTE;
      break;
    case VERTEX_ATTRIBUTE_DATA_TYPE_SHORT:
      gl_data_type = GL_SHORT;
      break;
    case VERTEX_ATTRIBUTE_DATA_TYPE_UNSIGNED_SHORT:
      gl_data_type = GL_UNSIGNED_SHORT;
      break;
    case VERTEX_ATTRIBUTE_DATA_TYPE_INT:
      gl_data_type = GL_INT;
      break;
    case VERTEX_ATTRIBUTE_DATA_TYPE_UNSIGNED_INT:
      gl_data_type = GL_UNSIGNED_INT;
      break;
    case VERTEX_ATTRIBUTE_DATA_TYPE_HALF_FLOAT:
      gl_data_type = GL_HALF_FLOAT;
      break;
    case VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT:
      gl_data_type = GL_FLOAT;
      break;
    case VERTEX_ATTRIBUTE_DATA_TYPE_DOUBLE:
      gl_data_type = GL_DOUBLE;
      break;
    case VERTEX_ATTRIBUTE_DATA_TYPE_FIXED:
      gl_data_type = GL_FIXED;
      break;
    default:
      gl_data_type = GL_INVALID_ENUM;
      break;
  }

  return gl_data_type;
}

/*******************************************************************************
 *
 *   Function: vertex_array_gl_set_attribute()
 *
 ******************************************************************************/
static void vertex_array_gl_set_attribute(
  const struct vertex_array_t* in_vertex_array,
  const struct vertex_attribute_t* in_vertex_attribute)
{
  // Specify how to interpret vertex data in the array
  glVertexAttribPointer(
    in_vertex_attribute->index,
    in_vertex_attribute->size,
    vertex_array_gl_get_attribute_data_type(in_vertex_attribute->data_type),
    in_vertex_attribute->should_normalize,
    in_vertex_attribute->stride,
    in_vertex_attribute->ptr_offset_to_attrib);

  // vertex_array_gl_check_error(
  //   "Set attribute '%s' in Vertex Array ID %d\n",
  //  "Error occurred in setting of vertex attribute '%s' in Vertex Array ID
  //  %d\n",
  //   vertex_attribute->attribute_name,
  //   vertex_array->vertex_array);

  glEnableVertexAttribArray(in_vertex_attribute->index);

  // vertex_array_gl_check_error(
  //   "Enabled Vertex Attribute '%s' in Vertex Array ID %d\n",
  //  "Error occurred in enabling of vertex attribute '%s' in Vertex Array ID
  //  %d\n",
  //   vertex_attribute->attribute_name,
  //   vertex_array->vertex_array);
}

/*******************************************************************************
 *
 *   Function: vertex_array_gl_destroy()
 *
 ******************************************************************************/
static void vertex_array_gl_destroy(
  struct vertex_array_t* inout_vertex_array,
  const int in_num)
{
  glDeleteVertexArrays(
    in_num,
    &(inout_vertex_array->vertex_array));
}
