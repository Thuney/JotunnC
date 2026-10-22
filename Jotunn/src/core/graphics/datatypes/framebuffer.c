#include "framebuffer.h"

#ifdef DEBUG
    #include <stdio.h>
#endif

#include <stdlib.h>

extern void platform_framebuffer_init(
  struct framebuffer_t* frame_buffer);
extern void platform_framebuffer_cleanup(
  struct framebuffer_t* frame_buffer);

extern void platform_framebuffer_bind(
  struct framebuffer_t* frame_buffer);
extern void platform_framebuffer_unbind();

extern void platform_framebuffer_attach_color_buffer(
  struct framebuffer_t* frame_buffer,
  struct color_buffer_t* color_buffer);
extern void platform_framebuffer_attach_render_buffer(
  struct framebuffer_t* frame_buffer,
  struct render_buffer_t* render_buffer);

extern void platform_color_buffer_init(
  struct color_buffer_t* color_buffer,
  unsigned int width,
  unsigned int height);
extern void platform_render_buffer_init(
  struct render_buffer_t* render_buffer,
  unsigned int width,
  unsigned int height);

extern void platform_color_buffer_cleanup(
  struct color_buffer_t* color_buffer);
extern void platform_render_buffer_cleanup(
  struct render_buffer_t* render_buffer);

extern void platform_color_buffer_bind(
  struct color_buffer_t* color_buffer);
extern void platform_render_buffer_bind(
  struct render_buffer_t* render_buffer);

extern void platform_color_buffer_resize(
  struct color_buffer_t* color_buffer,
  unsigned int new_width,
  unsigned int new_height);
extern void platform_render_buffer_resize(
  struct render_buffer_t* render_buffer,
  unsigned int new_width,
  unsigned int new_height);

void framebuffer_init(
  struct framebuffer_t* frame_buffer,
  unsigned int width,
  unsigned int height)
{
  platform_framebuffer_init(
    frame_buffer);

  framebuffer_bind(
    frame_buffer);

  color_buffer_init(
    &(frame_buffer->color_buffer),
    width,
    height);
  color_buffer_bind(
    &(frame_buffer->color_buffer));
  framebuffer_attach_color_buffer(
    frame_buffer,
    &(frame_buffer->color_buffer));

  render_buffer_init(
    &(frame_buffer->depth_stencil_buffer),
    width,
    height);
  render_buffer_bind(
    &(frame_buffer->depth_stencil_buffer));
  framebuffer_attach_render_buffer(
    frame_buffer,
    &(frame_buffer->depth_stencil_buffer));

  framebuffer_unbind();
}

void framebuffer_resize(
  struct framebuffer_t* frame_buffer,
  unsigned int new_width,
  unsigned int new_height)
{
  color_buffer_resize(
    &(frame_buffer->color_buffer),
    new_width,
    new_height);
  render_buffer_resize(
    &(frame_buffer->depth_stencil_buffer),
    new_width,
    new_height);
}

void framebuffer_attach_color_buffer(
  struct framebuffer_t* frame_buffer,
  struct color_buffer_t* color_buffer)
{
  platform_framebuffer_attach_color_buffer(
    frame_buffer,
    color_buffer);
}

void framebuffer_attach_render_buffer(
  struct framebuffer_t* frame_buffer,
  struct render_buffer_t* render_buffer)
{
  platform_framebuffer_attach_render_buffer(
    frame_buffer,
    render_buffer);
}

void framebuffer_bind(
  struct framebuffer_t* frame_buffer)
{
  platform_framebuffer_bind(frame_buffer);
}

void framebuffer_unbind()
{
  platform_framebuffer_unbind();
}

void framebuffer_cleanup(
  struct framebuffer_t* frame_buffer)
{
  platform_framebuffer_cleanup(frame_buffer);

  color_buffer_cleanup(&(frame_buffer->color_buffer));
  render_buffer_cleanup(&(frame_buffer->depth_stencil_buffer));
}

//

void color_buffer_init(
  struct color_buffer_t* color_buffer,
  unsigned int width,
  unsigned int height)
{
  platform_color_buffer_init(color_buffer, width, height);
}

void render_buffer_init(
  struct render_buffer_t* render_buffer,
  unsigned int width,
  unsigned int height)
{
  platform_render_buffer_init(render_buffer, width, height);
}

void color_buffer_bind(
  struct color_buffer_t* color_buffer)
{
  platform_color_buffer_bind(color_buffer);
}

void render_buffer_bind(
  struct render_buffer_t* render_buffer)
{
  platform_render_buffer_bind(render_buffer);
}

void color_buffer_resize(
  struct color_buffer_t* color_buffer,
  unsigned int new_width,
  unsigned int new_height)
{
  platform_color_buffer_resize(color_buffer, new_width, new_height);
}

void render_buffer_resize(
  struct render_buffer_t* render_buffer,
  unsigned int new_width,
  unsigned int new_height)
{
  platform_render_buffer_resize(render_buffer, new_width, new_height);
}

void color_buffer_cleanup(
  struct color_buffer_t* color_buffer)
{
#ifdef DEBUG
  fprintf(stdout, "Cleaning up color buffer\n");
#endif
}

void render_buffer_cleanup(
  struct render_buffer_t* render_buffer)
{
#ifdef DEBUG
  fprintf(stdout, "Cleaning up render buffer\n");
#endif
}
