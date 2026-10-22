#include "render_api.h"
#include "log.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>

static void _opengl_render_api_apply_blend_mode(
  const enum render_api_blend_mode_t in_blend_mode);

// Restricts drawing to in_scissor's rectangle when it is enabled, and lifts
//  the restriction otherwise
//
//  in:     in_scissor
static void _opengl_render_api_apply_scissor(
  const struct render_api_scissor_t* in_scissor);

static GLenum get_gl_element_data_type(
  enum render_api_element_data_type_t data_type)
{
  GLenum gl_data_type;

  switch (data_type)
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

static GLenum get_gl_draw_mode(
  enum render_api_draw_mode_t draw_mode)
{
  GLenum gl_draw_mode;

  switch (draw_mode)
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

static void opengl_render_api_init(
  struct render_api_implementation_t* render_api)
{
#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Initializing OpenGL Render API");
#endif

  glEnable(GL_LINE_SMOOTH);
}

static void opengl_render_api_bind(
  struct render_api_implementation_t* render_api)
{
  if (render_api->active_render_flags & BLEND_FLAG)
  {
    glEnable(GL_BLEND);
    _opengl_render_api_apply_blend_mode(render_api->blend_mode);
  }
  else
  {
    glDisable(GL_BLEND);
  }
  //
  if (render_api->active_render_flags & DEPTH_FLAG)
  {
    glEnable(GL_DEPTH_TEST);
  }
  else
  {
    glDisable(GL_DEPTH_TEST);
  }
  //
  if (render_api->active_render_flags & WIREFRAME_FLAG)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  //
  _opengl_render_api_apply_scissor(&(render_api->scissor));
}

static void opengl_render_api_clear(
  struct render_api_implementation_t* render_api)
{
  const fvector4 clear_color = render_api->clear_color;
  glClearColor(
    clear_color.comp.w,
    clear_color.comp.x,
    clear_color.comp.y,
    clear_color.comp.z);

  GLbitfield flags = 0x0000;

  if (render_api->active_render_flags & COLOR_FLAG)
    flags |= GL_COLOR_BUFFER_BIT;
  if (render_api->active_render_flags & DEPTH_FLAG)
    flags |= GL_DEPTH_BUFFER_BIT;

  glClear(flags);
}

static void opengl_render_api_draw_elements(
  enum render_api_draw_mode_t draw_mode,
  unsigned int element_count,
  const enum render_api_element_data_type_t element_data_type,
  const void* indices)
{
  glDrawElements(
    get_gl_draw_mode(draw_mode),
    element_count,
    get_gl_element_data_type(element_data_type),
    indices);
}

static void opengl_render_api_draw_triangle_fan(
  unsigned int vertex_count)
{
  glDrawArrays(get_gl_draw_mode(DRAW_TYPE_TRIANGLE_FAN), 0, vertex_count);
}

static void opengl_render_api_draw_lines(
  unsigned int vertex_count)
{
  glDrawArrays(get_gl_draw_mode(DRAW_TYPE_LINES), 0, vertex_count);
}

static void opengl_render_api_set_line_width(
  float line_width)
{
  glLineWidth(line_width);
}

//

void platform_render_api_init(
  struct render_api_implementation_t* render_api)
{
  opengl_render_api_init(render_api);
}

void platform_render_api_bind(
  struct render_api_implementation_t* render_api)
{
  opengl_render_api_bind(render_api);
}

void platform_render_api_clear(
  struct render_api_implementation_t* render_api)
{
  opengl_render_api_clear(render_api);
}

void platform_render_api_draw_elements(
  enum render_api_draw_mode_t draw_mode,
  unsigned int element_count,
  const enum render_api_element_data_type_t element_data_type,
  const void* indices)
{
  opengl_render_api_draw_elements(
    draw_mode, element_count, element_data_type, indices);
}

/*******************************************************************************
 *
 *   Function: platform_render_api_apply_scissor()
 *
 ******************************************************************************/
void platform_render_api_apply_scissor(
  const struct render_api_implementation_t* in_render_api)
{
  _opengl_render_api_apply_scissor(&(in_render_api->scissor));
}

/*******************************************************************************
 *
 *   Function: platform_render_api_draw_elements_base_vertex()
 *
 ******************************************************************************/
void platform_render_api_draw_elements_base_vertex(
  const enum render_api_draw_mode_t in_draw_mode,
  const uint32_t in_element_count,
  const enum render_api_element_data_type_t in_element_data_type,
  const uintptr_t in_index_offset_bytes,
  const int32_t in_base_vertex)
{
  glDrawElementsBaseVertex(
    get_gl_draw_mode(in_draw_mode),
    (GLsizei)in_element_count,
    get_gl_element_data_type(in_element_data_type),
    (const void*)in_index_offset_bytes,
    in_base_vertex);
}

void platform_render_api_draw_triangle_fan(
  unsigned int vertex_count)
{
  opengl_render_api_draw_triangle_fan(vertex_count);
}

void platform_render_api_draw_lines(
  unsigned int vertex_count)
{
  opengl_render_api_draw_lines(vertex_count);
}

void platform_render_api_set_line_width(
  float line_width)
{
  opengl_render_api_set_line_width(line_width);
}

/*******************************************************************************
 *
 *   Function: platform_render_api_has_timestamps()
 *
 ******************************************************************************/
bool platform_render_api_has_timestamps(void)
{
  // Timestamp queries are core from OpenGL 3.3
  return (GLEW_VERSION_3_3 ||
          GLEW_ARB_timer_query);
}

/*******************************************************************************
 *
 *   Function: platform_render_api_create_timestamps()
 *
 ******************************************************************************/
void platform_render_api_create_timestamps(
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
 *   Function: platform_render_api_delete_timestamps()
 *
 ******************************************************************************/
void platform_render_api_delete_timestamps(
  const uint32_t* in_names,
  const uint32_t in_count)
{
  glDeleteQueries(
    (GLsizei)in_count,
    (const GLuint*)in_names);
}

/*******************************************************************************
 *
 *   Function: platform_render_api_record_timestamp()
 *
 ******************************************************************************/
void platform_render_api_record_timestamp(
  const uint32_t in_name)
{
  glQueryCounter(
    in_name,
    GL_TIMESTAMP);
}

/*******************************************************************************
 *
 *   Function: platform_render_api_read_timestamp()
 *
 ******************************************************************************/
bool platform_render_api_read_timestamp(
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
 *   Function: _opengl_render_api_apply_blend_mode()
 *
 ******************************************************************************/
static void _opengl_render_api_apply_blend_mode(
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
 *   Function: _opengl_render_api_apply_scissor()
 *
 ******************************************************************************/
static void _opengl_render_api_apply_scissor(
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
