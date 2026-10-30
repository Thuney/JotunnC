#pragma once

// Private to Jotunn: the hooks through which framebuffer.c reaches the
//  graphics backend. Each platform's framebuffer source file defines them; the
//  core functions in framebuffer.c handle the resource registry around them

#include "framebuffer.h"

/*******************************************************************************
 *
 *   FRAMEBUFFERS
 *
 ******************************************************************************/

// Creates the backend's framebuffer object, without any attachments
//
//  out:    out_frame_buffer
void framebuffer_internal_platform_init(
  struct framebuffer_t* out_frame_buffer);

// Deletes the backend's framebuffer object and its depth-stencil render
//  buffer. The color texture is left to the caller
//
//  in:     in_frame_buffer
void framebuffer_internal_platform_cleanup(
  const struct framebuffer_t* in_frame_buffer);

// Makes a framebuffer the target of drawing and reading
//
//  in:     in_frame_buffer
void framebuffer_internal_platform_bind(
  const struct framebuffer_t* in_frame_buffer);

// Binds the default framebuffer, that of the first window created, again
void framebuffer_internal_platform_unbind(void);

// Attaches a color buffer as color attachment 0 of whichever framebuffer is
//  bound; in_frame_buffer isn't used by the GL backend
//
//  in:     in_frame_buffer, in_color_buffer
void framebuffer_internal_platform_attach_color_buffer(
  const struct framebuffer_t* in_frame_buffer,
  const struct framebuffer_color_buffer_t* in_color_buffer);

// Attaches a render buffer as the depth-stencil attachment of whichever
//  framebuffer is bound; in_frame_buffer isn't used by the GL backend
//
//  in:     in_frame_buffer, in_render_buffer
void framebuffer_internal_platform_attach_render_buffer(
  const struct framebuffer_t* in_frame_buffer,
  const struct framebuffer_render_buffer_t* in_render_buffer);

/*******************************************************************************
 *
 *   ATTACHMENTS
 *
 ******************************************************************************/

// Creates a color buffer's RGBA texture, which doesn't wrap. The texture
//  registers itself with the resource registry and is left bound to texture
//  unit 0
//
//  in:     in_width_pixels, in_height_pixels
//  in/out: inout_color_buffer
void color_buffer_internal_platform_init(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels);

// Creates the backend's render buffer object, holding 24 bits of depth and 8
//  of stencil per pixel, and leaves it bound
//
//  in:     in_width_pixels, in_height_pixels
//  out:    out_render_buffer
void render_buffer_internal_platform_init(
  struct framebuffer_render_buffer_t* out_render_buffer,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels);

// Binds a color buffer's texture to texture unit 0
//
//  in:     in_color_buffer
void color_buffer_internal_platform_bind(
  const struct framebuffer_color_buffer_t* in_color_buffer);

// Binds a render buffer as the current render buffer
//
//  in:     in_render_buffer
void render_buffer_internal_platform_bind(
  const struct framebuffer_render_buffer_t* in_render_buffer);

// Resizes a color buffer's texture, leaving its contents undefined. The
//  texture's registry entry is updated, and it's left bound to texture unit 0,
//  which must be the active unit
//
//  in:     in_new_width_pixels, in_new_height_pixels
//  in/out: inout_color_buffer
void color_buffer_internal_platform_resize(
  struct framebuffer_color_buffer_t* inout_color_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels);

// Reallocates a render buffer's storage at a new size, leaving its contents
//  undefined and it bound
//
//  in:     in_render_buffer, in_new_width_pixels, in_new_height_pixels
void render_buffer_internal_platform_resize(
  const struct framebuffer_render_buffer_t* in_render_buffer,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels);
