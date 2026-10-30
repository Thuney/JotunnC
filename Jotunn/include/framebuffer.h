#pragma once

// Offscreen render targets. A framebuffer has a color attachment, an RGBA
//  texture that can be sampled once drawn to, and a combined depth-stencil
//  render buffer. The framebuffer and both attachments each have an entry in
//  the resource registry

#include "resource.h"
#include "texture.h"

#include <stdint.h>

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// Components of a framebuffer

struct framebuffer_color_buffer_t
{
  struct texture_2d_t texture;
  // unsigned int texture_id;
};

struct framebuffer_render_buffer_t
{
  unsigned int rbo;
  // Its entry in the resource registry
  struct resource_t resource;
};

struct framebuffer_t
{
  unsigned int frame_buffer;
  struct framebuffer_color_buffer_t color_buffer;
  struct framebuffer_render_buffer_t depth_stencil_buffer;
  // Its entry in the resource registry; the attachments have their own
  struct resource_t resource;
};

/*******************************************************************************
 *
 *   FRAMEBUFFERS
 *
 ******************************************************************************/

// Creates a framebuffer with a color attachment and a depth-stencil
//  attachment of the given size, registering all three with the resource
//  registry. Leaves the default framebuffer bound, and the color texture bound
//  to texture unit 0
//
//  in:     in_width_pixels, in_height_pixels
//  in/out: inout_frame_buffer
void framebuffer_init(
  struct framebuffer_t* inout_frame_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels);

// Resizes both of a framebuffer's attachments, leaving their contents
//  undefined. The color texture is respecified through texture unit 0, so
//  that must be the active unit; it's left bound there. Not proven to work: it
//  was long marked as probably broken
//
//  in:     in_new_width_pixels, in_new_height_pixels
//  in/out: inout_frame_buffer
void framebuffer_resize(
  struct framebuffer_t* inout_frame_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels);

// Attaches a color buffer as color attachment 0 of whichever framebuffer is
//  bound. Bind in_frame_buffer first; the GL backend doesn't use it
//
//  in:     in_frame_buffer, in_color_buffer
void framebuffer_attach_color_buffer(
  const struct framebuffer_t* in_frame_buffer,
  const struct framebuffer_color_buffer_t* in_color_buffer);

// Attaches a render buffer as the depth-stencil attachment of whichever
//  framebuffer is bound. Bind in_frame_buffer first; the GL backend doesn't
//  use it
//
//  in:     in_frame_buffer, in_render_buffer
void framebuffer_attach_render_buffer(
  const struct framebuffer_t* in_frame_buffer,
  const struct framebuffer_render_buffer_t* in_render_buffer);

// Makes a framebuffer the target of drawing and reading
//
//  in:     in_frame_buffer
void framebuffer_bind(
  const struct framebuffer_t* in_frame_buffer);

// Binds the default framebuffer, that of the first window created, again
void framebuffer_unbind(void);

// Deletes a framebuffer along with both of its attachments, removing all three
//  from the resource registry
//
//  in/out: inout_frame_buffer
void framebuffer_cleanup(
  struct framebuffer_t* inout_frame_buffer);

/*******************************************************************************
 *
 *   ATTACHMENTS
 *
 ******************************************************************************/

// Creates a color buffer's RGBA texture, which doesn't wrap, and registers it
//  with the resource registry. Leaves the texture bound to texture unit 0
//
//  in:     in_width_pixels, in_height_pixels
//  in/out: inout_color_buffer
void framebuffer_color_buffer_init(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels);

// Creates a render buffer holding 24 bits of depth and 8 of stencil per pixel,
//  and registers it with the resource registry. Leaves it bound
//
//  in:     in_width_pixels, in_height_pixels
//  in/out: inout_render_buffer
void framebuffer_render_buffer_init(
  struct framebuffer_render_buffer_t* inout_render_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels);

// Binds a color buffer's texture to texture unit 0
//
//  in:     in_color_buffer
void framebuffer_color_buffer_bind(
  const struct framebuffer_color_buffer_t* in_color_buffer);

// Binds a render buffer as the current render buffer
//
//  in:     in_render_buffer
void framebuffer_render_buffer_bind(
  const struct framebuffer_render_buffer_t* in_render_buffer);

// Resizes a color buffer's texture, leaving its contents undefined, and
//  updates its size in the resource registry. The texture is respecified
//  through texture unit 0, so that must be the active unit; it's left bound
//  there
//
//  in:     in_new_width_pixels, in_new_height_pixels
//  in/out: inout_color_buffer
void framebuffer_color_buffer_resize(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels);

// Resizes a render buffer, leaving its contents undefined and it bound, and
//  updates its size in the resource registry
//
//  in:     in_new_width_pixels, in_new_height_pixels
//  in/out: inout_render_buffer
void framebuffer_render_buffer_resize(
  struct framebuffer_render_buffer_t* inout_render_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels);

// Deletes a color buffer's texture and removes it from the resource registry
//
//  in/out: inout_color_buffer
void framebuffer_color_buffer_cleanup(
  struct framebuffer_color_buffer_t* inout_color_buffer);

// Removes a render buffer from the resource registry. The render buffer itself
//  is deleted along with its framebuffer by framebuffer_cleanup()
//
//  in/out: inout_render_buffer
void framebuffer_render_buffer_cleanup(
  struct framebuffer_render_buffer_t* inout_render_buffer);
