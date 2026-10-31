#include "framebuffer.h"

#include "../../../core/graphics/datatypes/framebuffer_internal.h"

/*******************************************************************************
 *
 *   Function: framebuffer_internal_platform_init()
 *
 ******************************************************************************/
void framebuffer_internal_platform_init(
  struct framebuffer_t* out_frame_buffer)
{
  return;
}

/*******************************************************************************
 *
 *   Function: color_buffer_internal_platform_init()
 *
 ******************************************************************************/
void color_buffer_internal_platform_init(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  unsigned int in_width_pixels,
  unsigned int in_height_pixels)
{
  return;
}

/*******************************************************************************
 *
 *   Function: render_buffer_internal_platform_init()
 *
 ******************************************************************************/
void render_buffer_internal_platform_init(
  struct framebuffer_render_buffer_t* out_render_buffer,
  unsigned int in_width_pixels,
  unsigned int in_height_pixels)
{
  return;
}

/*******************************************************************************
 *
 *   Function: color_buffer_internal_platform_resize()
 *
 ******************************************************************************/
void color_buffer_internal_platform_resize(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  unsigned int in_new_width_pixels,
  unsigned int in_new_height_pixels)
{
  return;
}

/*******************************************************************************
 *
 *   Function: render_buffer_internal_platform_resize()
 *
 ******************************************************************************/
void render_buffer_internal_platform_resize(
  const struct framebuffer_render_buffer_t* in_render_buffer,
  unsigned int in_new_width_pixels,
  unsigned int in_new_height_pixels)
{
  return;
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
  return;
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
  return;
}

/*******************************************************************************
 *
 *   Function: framebuffer_internal_platform_bind()
 *
 ******************************************************************************/
void framebuffer_internal_platform_bind(
  const struct framebuffer_t* in_frame_buffer)
{
  return;
}

/*******************************************************************************
 *
 *   Function: framebuffer_internal_platform_unbind()
 *
 ******************************************************************************/
void framebuffer_internal_platform_unbind()
{
  return;
}

/*******************************************************************************
 *
 *   Function: framebuffer_internal_platform_cleanup()
 *
 ******************************************************************************/
void framebuffer_internal_platform_cleanup(
  const struct framebuffer_t* in_frame_buffer)
{
  return;
}
