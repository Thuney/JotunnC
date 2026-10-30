#include "framebuffer.h"
#include "log.h"

#include "../../../core/graphics/datatypes/framebuffer_internal.h"

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

// Takes OpenGL's pending error, clearing it. In DEBUG builds, logs
//  in_success_string at debug level when there was none, and otherwise
//  in_error_string and the error code at error level, formatting either with
//  the remaining arguments. Its calls are all commented out at present
//
//  in:     in_success_string, in_error_string
static void framebuffer_gl_check_error(
  const char* in_success_string,
  const char* in_error_string,
  ...);

// Generates a framebuffer object
//
//  out:    out_frame_buffer
static void framebuffer_gl_init(
  struct framebuffer_t* out_frame_buffer);

// Deletes a framebuffer object and its depth-stencil render buffer
//
//  in:     in_frame_buffer
static void framebuffer_gl_cleanup(
  const struct framebuffer_t* in_frame_buffer);

// Binds a framebuffer to GL_FRAMEBUFFER, for both drawing and reading
//
//  in:     in_frame_buffer
static void framebuffer_gl_bind(
  const struct framebuffer_t* in_frame_buffer);

// Binds framebuffer 0, the default framebuffer of the first window created
static void framebuffer_gl_unbind(void);

// Attaches a color buffer's texture as color attachment 0 of the bound
//  framebuffer; in_frame_buffer only appears in a commented out error check
//
//  in:     in_frame_buffer, in_color_buffer
static void framebuffer_gl_attach_color_buffer(
  const struct framebuffer_t* in_frame_buffer,
  const struct framebuffer_color_buffer_t* in_color_buffer);

// Attaches a render buffer as the depth-stencil attachment of the bound
//  framebuffer; in_frame_buffer only appears in a commented out error check
//
//  in:     in_frame_buffer, in_render_buffer
static void framebuffer_gl_attach_render_buffer(
  const struct framebuffer_t* in_frame_buffer,
  const struct framebuffer_render_buffer_t* in_render_buffer);

// Creates a color buffer's texture as an RGBA texture that doesn't wrap
//
//  in:     in_width_pixels, in_height_pixels
//  in/out: inout_color_buffer
static void color_buffer_gl_init(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels);

// Generates a render buffer object, binds it, and allocates its storage as
//  GL_DEPTH24_STENCIL8
//
//  in:     in_width_pixels, in_height_pixels
//  out:    out_render_buffer
static void render_buffer_gl_init(
  struct framebuffer_render_buffer_t* out_render_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels);

// Binds a color buffer's texture to texture unit 0
//
//  in:     in_color_buffer
static void color_buffer_gl_bind(
  const struct framebuffer_color_buffer_t* in_color_buffer);

// Binds a render buffer to GL_RENDERBUFFER
//
//  in:     in_render_buffer
static void render_buffer_gl_bind(
  const struct framebuffer_render_buffer_t* in_render_buffer);

// Binds a color buffer's texture to texture unit 0 and resizes it there
//
//  in:     in_new_width_pixels, in_new_height_pixels
//  in/out: inout_color_buffer
static void color_buffer_gl_resize(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels);

// Binds a render buffer and reallocates its storage at a new size
//
//  in:     in_render_buffer, in_new_width_pixels, in_new_height_pixels
static void render_buffer_gl_resize(
  const struct framebuffer_render_buffer_t* in_render_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels);

/*******************************************************************************
 *
 *   Function: framebuffer_internal_platform_init()
 *
 ******************************************************************************/
void framebuffer_internal_platform_init(
  struct framebuffer_t* out_frame_buffer)
{
  framebuffer_gl_init(out_frame_buffer);
}

/*******************************************************************************
 *
 *   Function: framebuffer_internal_platform_cleanup()
 *
 ******************************************************************************/
void framebuffer_internal_platform_cleanup(
  const struct framebuffer_t* in_frame_buffer)
{
  framebuffer_gl_cleanup(in_frame_buffer);
}

/*******************************************************************************
 *
 *   Function: framebuffer_internal_platform_bind()
 *
 ******************************************************************************/
void framebuffer_internal_platform_bind(
  const struct framebuffer_t* in_frame_buffer)
{
  framebuffer_gl_bind(in_frame_buffer);
}

/*******************************************************************************
 *
 *   Function: framebuffer_internal_platform_unbind()
 *
 ******************************************************************************/
void framebuffer_internal_platform_unbind()
{
  framebuffer_gl_unbind();
}

/*******************************************************************************
 *
 *   Function: framebuffer_internal_platform_attach_color_buffer()
 *
 ******************************************************************************/
void framebuffer_internal_platform_attach_color_buffer(
  const struct framebuffer_t* in_frame_buffer,
  const struct framebuffer_color_buffer_t* in_color_buffer)
{
  framebuffer_gl_attach_color_buffer(in_frame_buffer, in_color_buffer);
}

/*******************************************************************************
 *
 *   Function: framebuffer_internal_platform_attach_render_buffer()
 *
 ******************************************************************************/
void framebuffer_internal_platform_attach_render_buffer(
  const struct framebuffer_t* in_frame_buffer,
  const struct framebuffer_render_buffer_t* in_render_buffer)
{
  framebuffer_gl_attach_render_buffer(in_frame_buffer, in_render_buffer);
}

/*******************************************************************************
 *
 *   Function: color_buffer_internal_platform_init()
 *
 ******************************************************************************/
void color_buffer_internal_platform_init(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels)
{
  color_buffer_gl_init(inout_color_buffer, in_width_pixels, in_height_pixels);
}

/*******************************************************************************
 *
 *   Function: render_buffer_internal_platform_init()
 *
 ******************************************************************************/
void render_buffer_internal_platform_init(
  struct framebuffer_render_buffer_t* out_render_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels)
{
  render_buffer_gl_init(out_render_buffer, in_width_pixels, in_height_pixels);
}

/*******************************************************************************
 *
 *   Function: color_buffer_internal_platform_bind()
 *
 ******************************************************************************/
void color_buffer_internal_platform_bind(
  const struct framebuffer_color_buffer_t* in_color_buffer)
{
  color_buffer_gl_bind(in_color_buffer);
}

/*******************************************************************************
 *
 *   Function: render_buffer_internal_platform_bind()
 *
 ******************************************************************************/
void render_buffer_internal_platform_bind(
  const struct framebuffer_render_buffer_t* in_render_buffer)
{
  render_buffer_gl_bind(in_render_buffer);
}

/*******************************************************************************
 *
 *   Function: color_buffer_internal_platform_resize()
 *
 ******************************************************************************/
void color_buffer_internal_platform_resize(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels)
{
  color_buffer_gl_resize(inout_color_buffer, in_new_width_pixels, in_new_height_pixels);
}

/*******************************************************************************
 *
 *   Function: render_buffer_internal_platform_resize()
 *
 ******************************************************************************/
void render_buffer_internal_platform_resize(
  const struct framebuffer_render_buffer_t* in_render_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels)
{
  render_buffer_gl_resize(in_render_buffer, in_new_width_pixels, in_new_height_pixels);
}

/*******************************************************************************
 *
 *   Function: framebuffer_gl_check_error()
 *
 ******************************************************************************/
static void framebuffer_gl_check_error(
  const char* in_success_string,
  const char* in_error_string,
  ...)
{
  GLenum _gl_error = GL_NO_ERROR;
  _gl_error = glGetError();

#ifdef DEBUG
  va_list args;
  va_start(args, in_error_string);

  if (_gl_error == GL_NO_ERROR)
  {
    log_write_va(
      LOG_LEVEL_DEBUG,
      in_success_string,
      args);
  }
  else
  {
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
 *   Function: framebuffer_gl_init()
 *
 ******************************************************************************/
static void framebuffer_gl_init(
  struct framebuffer_t* out_frame_buffer)
{
  glGenFramebuffers(1, &(out_frame_buffer->frame_buffer));

  // framebuffer_gl_check_error(
  //   "Frame buffer initialized with ID %d\n",
  //   "Error occurred during frame buffer generation\n",
  //   out_frame_buffer->frame_buffer);
}

/*******************************************************************************
 *
 *   Function: framebuffer_gl_cleanup()
 *
 ******************************************************************************/
static void framebuffer_gl_cleanup(
  const struct framebuffer_t* in_frame_buffer)
{
  glDeleteRenderbuffers(1, &(in_frame_buffer->depth_stencil_buffer.rbo));
  glDeleteFramebuffers(1, &(in_frame_buffer->frame_buffer));
}

/*******************************************************************************
 *
 *   Function: framebuffer_gl_bind()
 *
 ******************************************************************************/
static void framebuffer_gl_bind(
  const struct framebuffer_t* in_frame_buffer)
{
  // framebuffer_gl_check_error(
  //   "No error before frame buffer bind\n",
  //   "Error occurred prior to frame buffer bind\n");

  glBindFramebuffer(GL_FRAMEBUFFER, in_frame_buffer->frame_buffer);

  // framebuffer_gl_check_error(
  //   "Successfully bound frame buffer %d\n",
  //   "Error occurred while binding frame buffer %d\n",
  //   in_frame_buffer->frame_buffer);
}

/*******************************************************************************
 *
 *   Function: framebuffer_gl_unbind()
 *
 ******************************************************************************/
static void framebuffer_gl_unbind()
{
  // Binds back to what should be the default framebuffer (the framebuffer of
  //  the first window spawned)
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*******************************************************************************
 *
 *   Function: framebuffer_gl_attach_color_buffer()
 *
 ******************************************************************************/
static void framebuffer_gl_attach_color_buffer(
  const struct framebuffer_t* in_frame_buffer,
  const struct framebuffer_color_buffer_t* in_color_buffer)
{
  glFramebufferTexture2D(
    GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D,
    in_color_buffer->texture.texture_id,
    0);

  // framebuffer_gl_check_error(
  //   "Attached color buffer ID %d to frame buffer ID %d\n",
  //   "Error occurred during attachment of color buffer ID %d to frame buffer ID %d\n",
  //   in_color_buffer->texture.texture_id,
  //   in_frame_buffer->frame_buffer);
}

/*******************************************************************************
 *
 *   Function: framebuffer_gl_attach_render_buffer()
 *
 ******************************************************************************/
static void framebuffer_gl_attach_render_buffer(
  const struct framebuffer_t* in_frame_buffer,
  const struct framebuffer_render_buffer_t* in_render_buffer)
{
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER,
    GL_DEPTH_STENCIL_ATTACHMENT,
    GL_RENDERBUFFER,
    in_render_buffer->rbo);

  // framebuffer_gl_check_error(
  //   "Attached render buffer ID %d to frame buffer ID %d\n",
  //   "Error occurred during attachment of render buffer ID %d to frame buffer ID %d\n",
  //   in_render_buffer->rbo,
  //   in_frame_buffer->frame_buffer);
}

/*******************************************************************************
 *
 *   Function: color_buffer_gl_init()
 *
 ******************************************************************************/
static void color_buffer_gl_init(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels)
{
  texture_2d_init(
    &(inout_color_buffer->texture),
    in_width_pixels,
    in_height_pixels,
    TEXTURE_2D_INTERNAL_FORMAT_RGBA,
    TEXTURE_2D_DATA_FORMAT_RGBA,
    false);
}

/*******************************************************************************
 *
 *   Function: render_buffer_gl_init()
 *
 ******************************************************************************/
static void render_buffer_gl_init(
  struct framebuffer_render_buffer_t* out_render_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels)
{
  glGenRenderbuffers(1, &(out_render_buffer->rbo));

  render_buffer_gl_bind(out_render_buffer);

  // Use a single render buffer object for both a depth AND stencil buffer.
  glRenderbufferStorage(
    GL_RENDERBUFFER,
    GL_DEPTH24_STENCIL8,
    in_width_pixels,
    in_height_pixels);
}

/*******************************************************************************
 *
 *   Function: color_buffer_gl_bind()
 *
 ******************************************************************************/
static void color_buffer_gl_bind(
  const struct framebuffer_color_buffer_t* in_color_buffer)
{
  texture_2d_bind(
    &(in_color_buffer->texture),
    0);
}

/*******************************************************************************
 *
 *   Function: render_buffer_gl_bind()
 *
 ******************************************************************************/
static void render_buffer_gl_bind(
  const struct framebuffer_render_buffer_t* in_render_buffer)
{
  glBindRenderbuffer(GL_RENDERBUFFER, in_render_buffer->rbo);
}

/*******************************************************************************
 *
 *   Function: color_buffer_gl_resize()
 *
 ******************************************************************************/
static void color_buffer_gl_resize(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels)
{
  color_buffer_gl_bind(inout_color_buffer);

  texture_2d_resize(
    &(inout_color_buffer->texture),
    in_new_width_pixels,
    in_new_height_pixels);
}

/*******************************************************************************
 *
 *   Function: render_buffer_gl_resize()
 *
 ******************************************************************************/
static void render_buffer_gl_resize(
  const struct framebuffer_render_buffer_t* in_render_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels)
{
  render_buffer_gl_bind(in_render_buffer);

  glRenderbufferStorage(
    GL_RENDERBUFFER,
    GL_DEPTH24_STENCIL8,
    in_new_width_pixels,
    in_new_height_pixels);
}
