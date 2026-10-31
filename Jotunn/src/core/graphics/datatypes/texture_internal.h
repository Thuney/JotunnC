#pragma once

// Private to Jotunn: the hooks through which texture.c reaches the graphics
//  backend. Each platform's texture source file defines them; the core
//  functions in texture.c keep the texture's fields and its entry in the
//  resource registry up to date around them

#include "texture.h"

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 *
 *   TEXTURES
 *
 ******************************************************************************/

// Creates the backend's texture at the size and formats already set in the
//  texture, its pixels undefined, with linear filtering. in_should_wrap sets
//  repeat wrapping explicitly. Leaves the texture bound to texture unit 0 and
//  to the active unit
//
//  in:     in_should_wrap
//  in/out: inout_texture
void texture_2d_internal_platform_init(
  struct texture_2d_t* inout_texture,
  bool in_should_wrap);

// Deletes the backend's texture
//
//  in:     in_texture
void texture_2d_internal_platform_cleanup(
  const struct texture_2d_t* in_texture);

// Reallocates whichever texture is bound to the active unit at the size and
//  formats set in in_texture, leaving its pixels undefined. The GL backend
//  reads the new size from in_texture, not from in_new_width_pixels and
//  in_new_height_pixels, so the caller sets it there first
//
//  in:     in_texture, in_new_width_pixels, in_new_height_pixels
void texture_2d_internal_platform_resize(
  const struct texture_2d_t* in_texture,
  unsigned int in_new_width_pixels,
  unsigned int in_new_height_pixels);

/*******************************************************************************
 *
 *   PIXEL DATA
 *
 ******************************************************************************/

// Uploads the whole of a texture's pixels, as unsigned bytes in its data
//  format. The caller has already checked in_data_size_bytes against the
//  texture's size
//
//  in:     in_texture, in_data, in_data_size_bytes
void texture_2d_internal_platform_set_data(
  const struct texture_2d_t* in_texture,
  const void* in_data,
  unsigned int in_data_size_bytes);

// Uploads a rectangle of pixels into a texture, leaving the rest of it
//  untouched. in_data points at the rectangle's first pixel, in the texture's
//  data format, and successive rows of the rectangle start
//  in_row_length_pixels apart
//
//  in:     in_texture, in_x_pixels, in_y_pixels, in_width_pixels,
//            in_height_pixels, in_row_length_pixels, in_data
void texture_2d_internal_platform_set_sub_data(
  const struct texture_2d_t* in_texture,
  uint32_t in_x_pixels,
  uint32_t in_y_pixels,
  uint32_t in_width_pixels,
  uint32_t in_height_pixels,
  uint32_t in_row_length_pixels,
  const void* in_data);

/*******************************************************************************
 *
 *   BINDING
 *
 ******************************************************************************/

// Binds a texture to a texture unit
//
//  in:     in_texture, in_texture_slot
void texture_2d_internal_platform_bind(
  const struct texture_2d_t* in_texture,
  unsigned int in_texture_slot);
