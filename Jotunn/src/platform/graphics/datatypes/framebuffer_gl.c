#include "framebuffer.h"

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

// Framebuffer
static void gl_framebuffer_init(
  struct framebuffer_t* frame_buffer);
static void gl_framebuffer_cleanup(
  struct framebuffer_t* frame_buffer);

static void gl_framebuffer_bind(
  struct framebuffer_t* frame_buffer);
static void gl_framebuffer_unbind();

static void gl_framebuffer_attach_color_buffer(
  struct framebuffer_t* frame_buffer,
  struct color_buffer_t* color_buffer);
static void gl_framebuffer_attach_render_buffer(
  struct framebuffer_t* frame_buffer,
  struct render_buffer_t* render_buffer);

// Component Buffers
static void gl_color_buffer_init(
  struct color_buffer_t* color_buffer,
  unsigned int width,
  unsigned int height);
static void gl_render_buffer_init(
  struct render_buffer_t* render_buffer,
  unsigned int width,
  unsigned int height);

static void gl_color_buffer_bind(
  struct color_buffer_t* color_buffer);
static void gl_render_buffer_bind(
  struct render_buffer_t* render_buffer);

static void gl_color_buffer_resize(
  struct color_buffer_t* color_buffer,
  unsigned int new_width,
  unsigned int new_height);
static void gl_render_buffer_resize(
  struct render_buffer_t* render_buffer,
  unsigned int new_width,
  unsigned int new_height);

// -----------------------------------------------------------------------------

void platform_framebuffer_init(
  struct framebuffer_t* frame_buffer)
{
  gl_framebuffer_init(frame_buffer);
}

void platform_framebuffer_cleanup(
  struct framebuffer_t* frame_buffer)
{
  gl_framebuffer_cleanup(frame_buffer);
}

void platform_framebuffer_bind(
  struct framebuffer_t* frame_buffer)
{
  gl_framebuffer_bind(frame_buffer);
}

void platform_framebuffer_unbind()
{
  gl_framebuffer_unbind();
}

void platform_framebuffer_attach_color_buffer(
  struct framebuffer_t* frame_buffer,
  struct color_buffer_t* color_buffer)
{
  gl_framebuffer_attach_color_buffer(frame_buffer, color_buffer);
}

void platform_framebuffer_attach_render_buffer(
  struct framebuffer_t* frame_buffer,
  struct render_buffer_t* render_buffer)
{
  gl_framebuffer_attach_render_buffer(frame_buffer, render_buffer);
}

void platform_color_buffer_init(
  struct color_buffer_t* color_buffer,
  unsigned int width,
  unsigned int height)
{
  gl_color_buffer_init(color_buffer, width, height);
}

void platform_render_buffer_init(
  struct render_buffer_t* render_buffer,
  unsigned int width,
  unsigned int height)
{
  gl_render_buffer_init(render_buffer, width, height);
}

void platform_color_buffer_bind(
  struct color_buffer_t* color_buffer)
{
  gl_color_buffer_bind(color_buffer);
}

void platform_render_buffer_bind(
  struct render_buffer_t* render_buffer)
{
  gl_render_buffer_bind(render_buffer);
}

void platform_color_buffer_resize(
  struct color_buffer_t* color_buffer,
  unsigned int new_width,
  unsigned int new_height)
{
  gl_color_buffer_resize(color_buffer, new_width, new_height);
}

void platform_render_buffer_resize(
  struct render_buffer_t* render_buffer,
  unsigned int new_width,
  unsigned int new_height)
{
  gl_render_buffer_resize(render_buffer, new_width, new_height);
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

static void gl_framebuffer_init(
  struct framebuffer_t* frame_buffer)
{
  glGenFramebuffers(1, &(frame_buffer->frame_buffer));

  gl_error_check(
    "Frame buffer initialized with ID %d\n",
    "Error occurred during frame buffer generation\n",
    frame_buffer->frame_buffer);
}

static void gl_framebuffer_cleanup(
  struct framebuffer_t* frame_buffer)
{
  glDeleteRenderbuffers(1, &(frame_buffer->depth_stencil_buffer.rbo));
  glDeleteFramebuffers(1, &(frame_buffer->frame_buffer));
}

static void gl_framebuffer_bind(
  struct framebuffer_t* frame_buffer)
{
  gl_error_check(
    "No error before frame buffer bind\n",
    "Error occurred prior to frame buffer bind\n");

  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->frame_buffer);

  gl_error_check(
    "Successfully bound frame buffer %d\n",
    "Error occurred while binding frame buffer %d\n",
    frame_buffer->frame_buffer);
}

static void gl_framebuffer_unbind()
{
  // Binds back to what should be the default framebuffer (the framebuffer of the first window spawned)
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void gl_framebuffer_attach_color_buffer(
  struct framebuffer_t* frame_buffer,
  struct color_buffer_t* color_buffer)
{
  glFramebufferTexture2D(
    GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D,
    color_buffer->texture.texture_id,
    0);

  gl_error_check(
    "Attached color buffer ID %d to frame buffer ID %d\n",
    "Error occurred during attachment of color buffer ID %d to frame buffer ID %d\n",
    color_buffer->texture.texture_id,
    frame_buffer->frame_buffer);
}

static void gl_framebuffer_attach_render_buffer(
  struct framebuffer_t* frame_buffer,
  struct render_buffer_t* render_buffer)
{
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER,
    GL_DEPTH_STENCIL_ATTACHMENT,
    GL_RENDERBUFFER,
    render_buffer->rbo);

  gl_error_check(
    "Attached render buffer ID %d to frame buffer ID %d\n",
    "Error occurred during attachment of render buffer ID %d to frame buffer ID %d\n",
    render_buffer->rbo,
    frame_buffer->frame_buffer);
}

// Component Buffers
static void gl_color_buffer_init(
  struct color_buffer_t* color_buffer,
  unsigned int width,
  unsigned int height)
{
  texture_2d_init(
    &(color_buffer->texture),
    width,
    height,
    TEXTURE_2D_INTERNAL_FORMAT_RGBA,
    TEXTURE_2D_DATA_FORMAT_RGBA,
    false);
}

static void gl_render_buffer_init(
  struct render_buffer_t* render_buffer,
  unsigned int width,
  unsigned int height)
{
  glGenRenderbuffers(1, &(render_buffer->rbo));

  gl_render_buffer_bind(render_buffer);

  // Use a single render buffer object for both a depth AND stencil buffer.
  glRenderbufferStorage(
    GL_RENDERBUFFER,
    GL_DEPTH24_STENCIL8,
    width,
    height);
}

static void gl_color_buffer_bind(
  struct color_buffer_t* color_buffer)
{
  texture_2d_bind(
    &(color_buffer->texture),
    0);
}

static void gl_render_buffer_bind(
  struct render_buffer_t* render_buffer)
{
  glBindRenderbuffer(GL_RENDERBUFFER, render_buffer->rbo);
}

static void gl_color_buffer_resize(
  struct color_buffer_t* color_buffer,
  unsigned int new_width,
  unsigned int new_height)
{
  gl_color_buffer_bind(color_buffer);

  texture_2d_resize(
    &(color_buffer->texture),
    new_width,
    new_height);
}

static void gl_render_buffer_resize(
  struct render_buffer_t* render_buffer,
  unsigned int new_width,
  unsigned int new_height)
{
  gl_render_buffer_bind(render_buffer);

  glRenderbufferStorage(
    GL_RENDERBUFFER,
    GL_DEPTH24_STENCIL8,
    new_width,
    new_height);
}
