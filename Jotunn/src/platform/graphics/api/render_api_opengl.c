#include "render_api.h"
#include "../../../core/graphics/api/render_api_internal.h"
#include "log.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Translates an element data type to OpenGL's
//
//  in:     in_data_type
//  ret:    The OpenGL type; GL_INVALID_ENUM when in_data_type isn't valid
static GLenum render_api_opengl_get_element_data_type(
  const enum render_api_element_data_type_t in_data_type);

// Translates a draw mode to OpenGL's
//
//  in:     in_draw_mode
//  ret:    The OpenGL mode; GL_INVALID_ENUM when in_draw_mode isn't valid
static GLenum render_api_opengl_get_draw_mode(
  const enum render_api_draw_mode_t in_draw_mode);

// Turns on line smoothing in the current context; in_render_api is unused
//
//  in:     in_render_api
static void render_api_opengl_init(
  const struct render_api_implementation_t* in_render_api);

// Applies an implementation's blend, depth and wireframe flags, blend mode and
//  scissor to the current context
//
//  in:     in_render_api
static void render_api_opengl_bind(
  const struct render_api_implementation_t* in_render_api);

// Clears the color and depth buffers the implementation's flags ask for, the
//  color buffer to its clear color
//
//  in:     in_render_api
static void render_api_opengl_clear(
  const struct render_api_implementation_t* in_render_api);

// Draws indexed geometry with glDrawElements()
//
//  in:     in_draw_mode, in_element_count, in_element_data_type, in_indices
static void render_api_opengl_draw_elements(
  const enum render_api_draw_mode_t in_draw_mode,
  const unsigned int in_element_count,
  const enum render_api_element_data_type_t in_element_data_type,
  const void* in_indices);

// Draws the first in_vertex_count vertices of the bound vertex array as a
//  triangle fan
//
//  in:     in_vertex_count
static void render_api_opengl_draw_triangle_fan(
  const unsigned int in_vertex_count);

// Draws the first in_vertex_count vertices of the bound vertex array as
//  separate lines, one per pair
//
//  in:     in_vertex_count
static void render_api_opengl_draw_lines(
  const unsigned int in_vertex_count);

// Sets the width lines are drawn with
//
//  in:     in_line_width_pixels
static void render_api_opengl_set_line_width(
  const float in_line_width_pixels);

// Sets the blend function for a blend mode; an unknown mode blends as
//  RENDER_API_BLEND_MODE_ALPHA
//
//  in:     in_blend_mode
static void _render_api_opengl_apply_blend_mode(
  const enum render_api_blend_mode_t in_blend_mode);

// Restricts drawing to in_scissor's rectangle when it is enabled, and lifts
//  the restriction otherwise
//
//  in:     in_scissor
static void _render_api_opengl_apply_scissor(
  const struct render_api_scissor_t* in_scissor);

/*******************************************************************************
 *
 *   Function: render_api_opengl_get_element_data_type()
 *
 ******************************************************************************/
static GLenum render_api_opengl_get_element_data_type(
  const enum render_api_element_data_type_t in_data_type)
{
  GLenum gl_data_type;

  switch (in_data_type)
  {
    case RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_BYTE:
      gl_data_type = GL_UNSIGNED_BYTE;
      break;
    case RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_SHORT:
      gl_data_type = GL_UNSIGNED_SHORT;
      break;
    case RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_INT:
      gl_data_type = GL_UNSIGNED_INT;
      break;
    default:
      gl_data_type = GL_INVALID_ENUM;
      break;
  }

  return gl_data_type;
}

/*******************************************************************************
 *
 *   Function: render_api_opengl_get_draw_mode()
 *
 ******************************************************************************/
static GLenum render_api_opengl_get_draw_mode(
  const enum render_api_draw_mode_t in_draw_mode)
{
  GLenum gl_draw_mode;

  switch (in_draw_mode)
  {
    case RENDER_API_DRAW_MODE_POINTS:
      gl_draw_mode = GL_POINTS;
      break;
    case RENDER_API_DRAW_MODE_LINE_STRIP:
      gl_draw_mode = GL_LINE_STRIP;
      break;
    case RENDER_API_DRAW_MODE_LINE_LOOP:
      gl_draw_mode = GL_LINE_LOOP;
      break;
    case RENDER_API_DRAW_MODE_LINES:
      gl_draw_mode = GL_LINES;
      break;
    case RENDER_API_DRAW_MODE_LINE_STRIP_ADJACENCY:
      gl_draw_mode = GL_LINE_STRIP_ADJACENCY;
      break;
    case RENDER_API_DRAW_MODE_LINES_ADJACENCY:
      gl_draw_mode = GL_LINES_ADJACENCY;
      break;
    case RENDER_API_DRAW_MODE_TRIANGLE_STRIP:
      gl_draw_mode = GL_TRIANGLE_STRIP;
      break;
    case RENDER_API_DRAW_MODE_TRIANGLE_FAN:
      gl_draw_mode = GL_TRIANGLE_FAN;
      break;
    case RENDER_API_DRAW_MODE_TRIANGLES:
      gl_draw_mode = GL_TRIANGLES;
      break;
    case RENDER_API_DRAW_MODE_TRIANGLE_STRIP_ADJACENCY:
      gl_draw_mode = GL_TRIANGLE_STRIP_ADJACENCY;
      break;
    case RENDER_API_DRAW_MODE_TRIANGLES_ADJACENCY:
      gl_draw_mode = GL_TRIANGLES_ADJACENCY;
      break;
    case RENDER_API_DRAW_MODE_PATCHES:
      gl_draw_mode = GL_PATCHES;
      break;
    default:
      gl_draw_mode = GL_INVALID_ENUM;
      break;
  }

  return gl_draw_mode;
}

/*******************************************************************************
 *
 *   Function: render_api_opengl_init()
 *
 ******************************************************************************/
static void render_api_opengl_init(
  const struct render_api_implementation_t* in_render_api)
{
#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Initializing OpenGL Render API");
#endif

  glEnable(GL_LINE_SMOOTH);
}

/*******************************************************************************
 *
 *   Function: render_api_opengl_bind()
 *
 ******************************************************************************/
static void render_api_opengl_bind(
  const struct render_api_implementation_t* in_render_api)
{
  if (in_render_api->active_render_flags & RENDER_API_RENDER_FLAGS_BLEND)
  {
    glEnable(GL_BLEND);
    _render_api_opengl_apply_blend_mode(in_render_api->blend_mode);
  }
  else
  {
    glDisable(GL_BLEND);
  }
  //
  if (in_render_api->active_render_flags & RENDER_API_RENDER_FLAGS_DEPTH)
  {
    glEnable(GL_DEPTH_TEST);
  }
  else
  {
    glDisable(GL_DEPTH_TEST);
  }
  //
  if (in_render_api->active_render_flags & RENDER_API_RENDER_FLAGS_WIREFRAME)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  //
  _render_api_opengl_apply_scissor(&(in_render_api->scissor));
}

/*******************************************************************************
 *
 *   Function: render_api_opengl_clear()
 *
 ******************************************************************************/
static void render_api_opengl_clear(
  const struct render_api_implementation_t* in_render_api)
{
  const union fvector4_t clear_color = in_render_api->clear_color;
  glClearColor(
    clear_color.comp.w,
    clear_color.comp.x,
    clear_color.comp.y,
    clear_color.comp.z);

  GLbitfield flags = 0x0000;

  if (in_render_api->active_render_flags & RENDER_API_RENDER_FLAGS_COLOR)
    flags |= GL_COLOR_BUFFER_BIT;
  if (in_render_api->active_render_flags & RENDER_API_RENDER_FLAGS_DEPTH)
    flags |= GL_DEPTH_BUFFER_BIT;

  glClear(flags);
}

/*******************************************************************************
 *
 *   Function: render_api_opengl_draw_elements()
 *
 ******************************************************************************/
static void render_api_opengl_draw_elements(
  const enum render_api_draw_mode_t in_draw_mode,
  const unsigned int in_element_count,
  const enum render_api_element_data_type_t in_element_data_type,
  const void* in_indices)
{
  glDrawElements(
    render_api_opengl_get_draw_mode(in_draw_mode),
    in_element_count,
    render_api_opengl_get_element_data_type(in_element_data_type),
    in_indices);
}

/*******************************************************************************
 *
 *   Function: render_api_opengl_draw_triangle_fan()
 *
 ******************************************************************************/
static void render_api_opengl_draw_triangle_fan(
  const unsigned int in_vertex_count)
{
  glDrawArrays(render_api_opengl_get_draw_mode(RENDER_API_DRAW_MODE_TRIANGLE_FAN), 0, in_vertex_count);
}

/*******************************************************************************
 *
 *   Function: render_api_opengl_draw_lines()
 *
 ******************************************************************************/
static void render_api_opengl_draw_lines(
  const unsigned int in_vertex_count)
{
  glDrawArrays(render_api_opengl_get_draw_mode(RENDER_API_DRAW_MODE_LINES), 0, in_vertex_count);
}

/*******************************************************************************
 *
 *   Function: render_api_opengl_set_line_width()
 *
 ******************************************************************************/
static void render_api_opengl_set_line_width(
  const float in_line_width_pixels)
{
  glLineWidth(in_line_width_pixels);
}

//

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_init()
 *
 ******************************************************************************/
void render_api_internal_platform_init(
  const struct render_api_implementation_t* in_render_api)
{
  render_api_opengl_init(in_render_api);
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_bind()
 *
 ******************************************************************************/
void render_api_internal_platform_bind(
  const struct render_api_implementation_t* in_render_api)
{
  render_api_opengl_bind(in_render_api);
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_clear()
 *
 ******************************************************************************/
void render_api_internal_platform_clear(
  const struct render_api_implementation_t* in_render_api)
{
  render_api_opengl_clear(in_render_api);
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_draw_elements()
 *
 ******************************************************************************/
void render_api_internal_platform_draw_elements(
  const enum render_api_draw_mode_t in_draw_mode,
  const unsigned int in_element_count,
  const enum render_api_element_data_type_t in_element_data_type,
  const void* in_indices)
{
  render_api_opengl_draw_elements(
    in_draw_mode, in_element_count, in_element_data_type, in_indices);
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_apply_scissor()
 *
 ******************************************************************************/
void render_api_internal_platform_apply_scissor(
  const struct render_api_implementation_t* in_render_api)
{
  _render_api_opengl_apply_scissor(&(in_render_api->scissor));
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_draw_elements_base_vertex()
 *
 ******************************************************************************/
void render_api_internal_platform_draw_elements_base_vertex(
  const enum render_api_draw_mode_t in_draw_mode,
  const uint32_t in_element_count,
  const enum render_api_element_data_type_t in_element_data_type,
  const uintptr_t in_index_offset_bytes,
  const int32_t in_base_vertex)
{
  glDrawElementsBaseVertex(
    render_api_opengl_get_draw_mode(in_draw_mode),
    (GLsizei)in_element_count,
    render_api_opengl_get_element_data_type(in_element_data_type),
    (const void*)in_index_offset_bytes,
    in_base_vertex);
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_draw_triangle_fan()
 *
 ******************************************************************************/
void render_api_internal_platform_draw_triangle_fan(
  const unsigned int in_vertex_count)
{
  render_api_opengl_draw_triangle_fan(in_vertex_count);
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_draw_lines()
 *
 ******************************************************************************/
void render_api_internal_platform_draw_lines(
  const unsigned int in_vertex_count)
{
  render_api_opengl_draw_lines(in_vertex_count);
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_set_line_width()
 *
 ******************************************************************************/
void render_api_internal_platform_set_line_width(
  const float in_line_width_pixels)
{
  render_api_opengl_set_line_width(in_line_width_pixels);
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_has_timestamps()
 *
 ******************************************************************************/
bool render_api_internal_platform_has_timestamps(void)
{
  // Timestamp queries are core from OpenGL 3.3
  return (GLEW_VERSION_3_3 ||
          GLEW_ARB_timer_query);
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_create_timestamps()
 *
 ******************************************************************************/
void render_api_internal_platform_create_timestamps(
  uint32_t* out_names,
  const uint32_t in_count)
{
  static_assert(
    sizeof(GLuint) == sizeof(uint32_t),
    "Query names are expected to be 32 bits wide");

  glGenQueries(
    (GLsizei)in_count,
    (GLuint*)out_names);
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_delete_timestamps()
 *
 ******************************************************************************/
void render_api_internal_platform_delete_timestamps(
  const uint32_t* in_names,
  const uint32_t in_count)
{
  glDeleteQueries(
    (GLsizei)in_count,
    (const GLuint*)in_names);
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_record_timestamp()
 *
 ******************************************************************************/
void render_api_internal_platform_record_timestamp(
  const uint32_t in_name)
{
  glQueryCounter(
    in_name,
    GL_TIMESTAMP);
}

/*******************************************************************************
 *
 *   Function: render_api_internal_platform_read_timestamp()
 *
 ******************************************************************************/
bool render_api_internal_platform_read_timestamp(
  const uint32_t in_name,
  uint64_t* out_time_nanoseconds)
{
  GLint is_available = GL_FALSE;
  glGetQueryObjectiv(
    in_name,
    GL_QUERY_RESULT_AVAILABLE,
    &(is_available));

  if (is_available == GL_FALSE)
  {
    return false;
  }

  GLuint64 time_nanoseconds = 0;
  glGetQueryObjectui64v(
    in_name,
    GL_QUERY_RESULT,
    &(time_nanoseconds));

  *out_time_nanoseconds = (uint64_t)time_nanoseconds;

  return true;
}

/*******************************************************************************
 *
 *   Function: _render_api_opengl_apply_blend_mode()
 *
 ******************************************************************************/
static void _render_api_opengl_apply_blend_mode(
  const enum render_api_blend_mode_t in_blend_mode)
{
  switch (in_blend_mode)
  {
    case RENDER_API_BLEND_MODE_PREMULTIPLIED_ALPHA:
    {
      glBlendFunc(
        GL_ONE,
        GL_ONE_MINUS_SRC_ALPHA);
    }
    break;
    case RENDER_API_BLEND_MODE_ALPHA:
    default:
    {
      // Alpha accumulates as coverage rather than being scaled by itself, so
      //  partially transparent results stay correct when composited later
      glBlendFuncSeparate(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_ONE,
        GL_ONE_MINUS_SRC_ALPHA);
    }
    break;
  }
}

/*******************************************************************************
 *
 *   Function: _render_api_opengl_apply_scissor()
 *
 ******************************************************************************/
static void _render_api_opengl_apply_scissor(
  const struct render_api_scissor_t* in_scissor)
{
  if (in_scissor->is_enabled)
  {
    glEnable(GL_SCISSOR_TEST);
    glScissor(
      in_scissor->x_pixels,
      in_scissor->y_pixels,
      in_scissor->width_pixels,
      in_scissor->height_pixels);
  }
  else
  {
    glDisable(GL_SCISSOR_TEST);
  }
}
