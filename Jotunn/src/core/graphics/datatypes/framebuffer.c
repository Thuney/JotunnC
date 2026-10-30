#include "framebuffer.h"
#include "framebuffer_internal.h"

#include "../resource_internal.h"

#ifdef DEBUG
    #include <stdio.h>
#endif

#include <stdlib.h>

// Render buffers hold 24 bits of depth and 8 of stencil per pixel
#define _FRAMEBUFFER_RENDER_BUFFER_BYTES_PER_PIXEL 4

/*******************************************************************************
 *
 *   Function: framebuffer_init()
 *
 ******************************************************************************/
void framebuffer_init(
  struct framebuffer_t* inout_frame_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels)
{
  framebuffer_internal_platform_init(
    inout_frame_buffer);

  framebuffer_bind(
    inout_frame_buffer);

  framebuffer_color_buffer_init(
    &(inout_frame_buffer->color_buffer),
    in_width_pixels,
    in_height_pixels);
  framebuffer_color_buffer_bind(
    &(inout_frame_buffer->color_buffer));
  framebuffer_attach_color_buffer(
    inout_frame_buffer,
    &(inout_frame_buffer->color_buffer));

  framebuffer_render_buffer_init(
    &(inout_frame_buffer->depth_stencil_buffer),
    in_width_pixels,
    in_height_pixels);
  framebuffer_render_buffer_bind(
    &(inout_frame_buffer->depth_stencil_buffer));
  framebuffer_attach_render_buffer(
    inout_frame_buffer,
    &(inout_frame_buffer->depth_stencil_buffer));

  framebuffer_unbind();

  // The framebuffer object holds no memory of its own; its attachments do
  resource_internal_register(
    &(inout_frame_buffer->resource),
    RESOURCE_KIND_FRAMEBUFFER,
    0);

  resource_set_name(
    &(inout_frame_buffer->color_buffer.texture.resource),
    "framebuffer color");
  resource_set_name(
    &(inout_frame_buffer->depth_stencil_buffer.resource),
    "framebuffer depth-stencil");
}

/*******************************************************************************
 *
 *   Function: framebuffer_resize()
 *
 ******************************************************************************/
void framebuffer_resize(
  struct framebuffer_t* inout_frame_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels)
{
  framebuffer_color_buffer_resize(
    &(inout_frame_buffer->color_buffer),
    in_new_width_pixels,
    in_new_height_pixels);
  framebuffer_render_buffer_resize(
    &(inout_frame_buffer->depth_stencil_buffer),
    in_new_width_pixels,
    in_new_height_pixels);
}

/*******************************************************************************
 *
 *   Function: framebuffer_attach_color_buffer()
 *
 ******************************************************************************/
void framebuffer_attach_color_buffer(
  const struct framebuffer_t* in_frame_buffer,
  const struct framebuffer_color_buffer_t* in_color_buffer)
{
  framebuffer_internal_platform_attach_color_buffer(
    in_frame_buffer,
    in_color_buffer);
}

/*******************************************************************************
 *
 *   Function: framebuffer_attach_render_buffer()
 *
 ******************************************************************************/
void framebuffer_attach_render_buffer(
  const struct framebuffer_t* in_frame_buffer,
  const struct framebuffer_render_buffer_t* in_render_buffer)
{
  framebuffer_internal_platform_attach_render_buffer(
    in_frame_buffer,
    in_render_buffer);
}

/*******************************************************************************
 *
 *   Function: framebuffer_bind()
 *
 ******************************************************************************/
void framebuffer_bind(
  const struct framebuffer_t* in_frame_buffer)
{
  framebuffer_internal_platform_bind(in_frame_buffer);
}

/*******************************************************************************
 *
 *   Function: framebuffer_unbind()
 *
 ******************************************************************************/
void framebuffer_unbind()
{
  framebuffer_internal_platform_unbind();
}

/*******************************************************************************
 *
 *   Function: framebuffer_cleanup()
 *
 ******************************************************************************/
void framebuffer_cleanup(
  struct framebuffer_t* inout_frame_buffer)
{
  framebuffer_internal_platform_cleanup(inout_frame_buffer);

  framebuffer_color_buffer_cleanup(&(inout_frame_buffer->color_buffer));
  framebuffer_render_buffer_cleanup(&(inout_frame_buffer->depth_stencil_buffer));

  resource_internal_unregister(&(inout_frame_buffer->resource));
}

/*******************************************************************************
 *
 *   Function: framebuffer_color_buffer_init()
 *
 ******************************************************************************/
void framebuffer_color_buffer_init(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels)
{
  color_buffer_internal_platform_init(inout_color_buffer, in_width_pixels, in_height_pixels);
}

/*******************************************************************************
 *
 *   Function: framebuffer_render_buffer_init()
 *
 ******************************************************************************/
void framebuffer_render_buffer_init(
  struct framebuffer_render_buffer_t* inout_render_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels)
{
  render_buffer_internal_platform_init(inout_render_buffer, in_width_pixels, in_height_pixels);

  resource_internal_register(
    &(inout_render_buffer->resource),
    RESOURCE_KIND_RENDER_BUFFER,
    ((size_t)in_width_pixels * in_height_pixels * _FRAMEBUFFER_RENDER_BUFFER_BYTES_PER_PIXEL));
}

/*******************************************************************************
 *
 *   Function: framebuffer_color_buffer_bind()
 *
 ******************************************************************************/
void framebuffer_color_buffer_bind(
  const struct framebuffer_color_buffer_t* in_color_buffer)
{
  color_buffer_internal_platform_bind(in_color_buffer);
}

/*******************************************************************************
 *
 *   Function: framebuffer_render_buffer_bind()
 *
 ******************************************************************************/
void framebuffer_render_buffer_bind(
  const struct framebuffer_render_buffer_t* in_render_buffer)
{
  render_buffer_internal_platform_bind(in_render_buffer);
}

/*******************************************************************************
 *
 *   Function: framebuffer_color_buffer_resize()
 *
 ******************************************************************************/
void framebuffer_color_buffer_resize(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels)
{
  color_buffer_internal_platform_resize(inout_color_buffer, in_new_width_pixels, in_new_height_pixels);
}

/*******************************************************************************
 *
 *   Function: framebuffer_render_buffer_resize()
 *
 ******************************************************************************/
void framebuffer_render_buffer_resize(
  struct framebuffer_render_buffer_t* inout_render_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels)
{
  render_buffer_internal_platform_resize(inout_render_buffer, in_new_width_pixels, in_new_height_pixels);

  resource_internal_set_size(
    &(inout_render_buffer->resource),
    ((size_t)in_new_width_pixels * in_new_height_pixels * _FRAMEBUFFER_RENDER_BUFFER_BYTES_PER_PIXEL));
}

/*******************************************************************************
 *
 *   Function: framebuffer_color_buffer_cleanup()
 *
 ******************************************************************************/
void framebuffer_color_buffer_cleanup(
  struct framebuffer_color_buffer_t* inout_color_buffer)
{
#ifdef DEBUG
  fprintf(stdout, "Cleaning up color buffer\n");
#endif

  texture_2d_cleanup(&(inout_color_buffer->texture));
}

/*******************************************************************************
 *
 *   Function: framebuffer_render_buffer_cleanup()
 *
 ******************************************************************************/
void framebuffer_render_buffer_cleanup(
  struct framebuffer_render_buffer_t* inout_render_buffer)
{
#ifdef DEBUG
  fprintf(stdout, "Cleaning up render buffer\n");
#endif

  // The render buffer itself is deleted along with its framebuffer
  resource_internal_unregister(&(inout_render_buffer->resource));
}
