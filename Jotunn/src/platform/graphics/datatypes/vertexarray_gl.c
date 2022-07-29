#include "vertexarray.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef DEBUG
   #include <stdio.h>
#endif

static int gl_vertex_array_init(struct vertex_array_t* vertex_array, int num)
{
   glGenVertexArrays(num, &vertex_array->vertex_array);

   return 0;
}

static void gl_vertex_array_bind(struct vertex_array_t* vertex_array)
{
   glBindVertexArray(vertex_array->vertex_array);
}

void gl_vertex_array_unbind()
{
   glBindVertexArray(0);
}

static GLenum get_gl_attribute_data_type(enum vertex_attribute_data_type_t data_type)
{
   GLenum gl_data_type;

   switch(data_type)
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

static void gl_vertex_array_set_attribute(struct vertex_array_t* vertex_array, struct vertex_attribute_t* vertex_attribute)
{
   #ifdef DEBUG

      unsigned int index = vertex_attribute->index;
      int size           = vertex_attribute->size;
      GLenum data_type   = get_gl_attribute_data_type(vertex_attribute->data_type);
      unsigned int should_normalize = vertex_attribute->should_normalize;
      unsigned int stride           = vertex_attribute->stride;
      void* ptr_offset_to_attrib    = vertex_attribute->ptr_offset_to_attrib;

      unsigned int offset_ptr_provided = (ptr_offset_to_attrib != 0);

      // fprintf(stdout, "Index = %u, Size = %d, Data Type = %u, Should Normalize = %u, Stride = %u, Offset Provided = %u\n", index, size, data_type, should_normalize, stride, offset_ptr_provided);

      glVertexAttribPointer(index, size, data_type, should_normalize, stride, ptr_offset_to_attrib);

   #else
      //Specify how to interpret the vertex data for our position attribute
      glVertexAttribPointer(vertex_attribute->index, vertex_attribute->size, get_gl_attribute_data_type(vertex_attribute->data_type), vertex_attribute->should_normalize, vertex_attribute->stride, vertex_attribute->ptr_offset_to_attrib);
   #endif

	glEnableVertexAttribArray(vertex_attribute->index);
}

static void gl_vertex_array_destroy(struct vertex_array_t* vertex_array, int num)
{
   glDeleteVertexArrays(num, &vertex_array->vertex_array);
}

int platform_vertex_array_init(struct vertex_array_t* vertex_array, int num)
{
   return gl_vertex_array_init(vertex_array, num);
}

void platform_vertex_array_bind(struct vertex_array_t* vertex_array)
{
   gl_vertex_array_bind(vertex_array);
}

void platform_vertex_array_unbind()
{
   gl_vertex_array_unbind();
}

void platform_vertex_array_set_attribute(struct vertex_array_t* vertex_array, struct vertex_attribute_t* vertex_attribute)
{
   gl_vertex_array_set_attribute(vertex_array, vertex_attribute);
}

void platform_vertex_array_destroy(struct vertex_array_t* vertex_array, int num)
{
   gl_vertex_array_destroy(vertex_array, num);
}