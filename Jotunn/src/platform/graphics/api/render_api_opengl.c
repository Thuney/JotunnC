#include "render_api.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

static GLenum get_gl_element_data_type(enum render_api_element_data_type_t data_type)
{
   GLenum gl_data_type;

   switch(data_type)
   {
      case ELEMENT_UNSIGNED_BYTE:
         gl_data_type = GL_UNSIGNED_BYTE;
      break;
      case ELEMENT_UNSIGNED_SHORT:
         gl_data_type = GL_UNSIGNED_SHORT;
      break;
      case ELEMENT_UNSIGNED_INT:
         gl_data_type = GL_UNSIGNED_INT;
      break;
      default:
         gl_data_type = GL_INVALID_ENUM;
      break;
   }
   
   return gl_data_type;
}

static GLenum get_gl_draw_mode(enum render_api_draw_mode_t draw_mode)
{
   GLenum gl_draw_mode;

   switch(draw_mode)
   {
      case DRAW_TYPE_POINTS:
         gl_draw_mode = GL_POINTS;
      break;
      case DRAW_TYPE_LINE_STRIP:
         gl_draw_mode = GL_LINE_STRIP;
      break;
      case DRAW_TYPE_LINE_LOOP:
         gl_draw_mode = GL_LINE_LOOP;
      break;
      case DRAW_TYPE_LINES:
         gl_draw_mode = GL_LINES;
      break;
      case DRAW_TYPE_LINE_STRIP_ADJACENCY:
         gl_draw_mode = GL_LINE_STRIP_ADJACENCY;
      break;
      case DRAW_TYPE_LINES_ADJACENCY:
         gl_draw_mode = GL_LINES_ADJACENCY;
      break;
      case DRAW_TYPE_TRIANGLE_STRIP:
         gl_draw_mode = GL_TRIANGLE_STRIP;
      break;
      case DRAW_TYPE_TRIANGLE_FAN:
         gl_draw_mode = GL_TRIANGLE_FAN;
      break;
      case DRAW_TYPE_TRIANGLES:
         gl_draw_mode = GL_TRIANGLES;
      break;
      case DRAW_TYPE_TRIANGLE_STRIP_ADJACENCY:
         gl_draw_mode = GL_TRIANGLE_STRIP_ADJACENCY;
      break;
      case DRAW_TYPE_TRIANGLES_ADJACENCY:
         gl_draw_mode = GL_TRIANGLES_ADJACENCY;
      break;
      case DRAW_TYPE_PATCHES:
         gl_draw_mode = GL_PATCHES;
      break;
      default:
         gl_draw_mode = GL_INVALID_ENUM;
      break;
   }

   return gl_draw_mode;
}

static void opengl_render_api_init()
{
   #ifdef DEBUG
      fprintf(stdout, "Initializing OpenGL Render API\n");
   #endif

   glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_DEPTH_TEST);

   glEnable(GL_LINE_SMOOTH);

   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

static void opengl_render_api_set_clear_color(const fvector4 color)
{
   glClearColor(color.comp.w, color.comp.x, color.comp.y, color.comp.z);
}

static void opengl_render_api_clear()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void opengl_render_api_draw_elements(enum render_api_draw_mode_t draw_mode, unsigned int element_count, const enum render_api_element_data_type_t element_data_type, const void* indices)
{
   glDrawElements(get_gl_draw_mode(draw_mode), element_count, get_gl_element_data_type(element_data_type), indices);
}

static void opengl_render_api_draw_triangle_fan(unsigned int vertex_count)
{
   glDrawArrays(get_gl_draw_mode(DRAW_TYPE_TRIANGLE_FAN), 0, vertex_count);
}

static void opengl_render_api_draw_lines(unsigned int vertex_count)
{
   glDrawArrays(get_gl_draw_mode(DRAW_TYPE_LINES), 0, vertex_count);
}

static void opengl_render_api_set_line_width(float line_width)
{
   glLineWidth(line_width);
}

//

void platform_render_api_init()
{
   opengl_render_api_init();
}

void platform_render_api_set_clear_color(const fvector4 color)
{
   opengl_render_api_set_clear_color(color);
}

void platform_render_api_clear()
{
   opengl_render_api_clear();
}

void platform_render_api_draw_elements(enum render_api_draw_mode_t draw_mode, unsigned int element_count, const enum render_api_element_data_type_t element_data_type, const void* indices)
{
   opengl_render_api_draw_elements(draw_mode, element_count, element_data_type, indices);
}

void platform_render_api_draw_triangle_fan(unsigned int vertex_count)
{
   opengl_render_api_draw_triangle_fan(vertex_count);
}

void platform_render_api_draw_lines(unsigned int vertex_count)
{
   opengl_render_api_draw_lines(vertex_count);
}

void platform_render_api_set_line_width(float line_width)
{
   opengl_render_api_set_line_width(line_width);
}