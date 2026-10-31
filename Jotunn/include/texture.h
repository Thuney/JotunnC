#pragma once

// 2D textures in graphics memory, each with an entry in the resource registry.
//  Textures never keep a copy of their pixels, and every upload is of unsigned
//  bytes, one per channel

#include "fvector.h"
#include "resource.h"

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

enum texture_2d_internal_format_t
{
  TEXTURE_2D_INTERNAL_FORMAT_RED = 0x00,
  TEXTURE_2D_INTERNAL_FORMAT_RG,
  TEXTURE_2D_INTERNAL_FORMAT_RGB,
  TEXTURE_2D_INTERNAL_FORMAT_RGBA,
  TEXTURE_2D_INTERNAL_FORMAT_DEPTH_COMPONENT,
  TEXTURE_2D_INTERNAL_FORMAT_DEPTH_STENCIL,
};

enum texture_2d_data_format_t
{
  TEXTURE_2D_DATA_FORMAT_RED = 0x00,
  TEXTURE_2D_DATA_FORMAT_RG,
  TEXTURE_2D_DATA_FORMAT_RGB,
  TEXTURE_2D_DATA_FORMAT_BGR,
  TEXTURE_2D_DATA_FORMAT_RGBA,
  TEXTURE_2D_DATA_FORMAT_BGRA,
  TEXTURE_2D_DATA_FORMAT_RED_INTEGER,
  TEXTURE_2D_DATA_FORMAT_RG_INTEGER,
  TEXTURE_2D_DATA_FORMAT_RGB_INTEGER,
  TEXTURE_2D_DATA_FORMAT_BGR_INTEGER,
  TEXTURE_2D_DATA_FORMAT_RGBA_INTEGER,
  TEXTURE_2D_DATA_FORMAT_BGRA_INTEGER,
  TEXTURE_2D_DATA_FORMAT_STENCIL_INDEX,
  TEXTURE_2D_DATA_FORMAT_DEPTH_COMPONENT,
  TEXTURE_2D_DATA_FORMAT_DEPTH_STENCIL,
};

// A texture never owns pixel data; whatever is uploaded to it stays with the
//  caller
struct texture_2d_t
{
  unsigned int texture_id;
  unsigned int width, height;
  int channels;

  enum texture_2d_internal_format_t internal_format;
  enum texture_2d_data_format_t data_format;
  // Set while the texture holds graphics memory, from texture_2d_init() until
  //  texture_2d_cleanup()
  unsigned int is_loaded;
  // Its entry in the resource registry
  struct resource_t resource;
};

struct texture_2d_subtexture_t
{
  struct texture_2d_t* parent_texture;
  union fvector2_t subtexture_coordinates[2];
};

/*******************************************************************************
 *
 *   TEXTURES
 *
 ******************************************************************************/

// Creates a texture of the given size and formats, its pixels undefined, and
//  registers it with the resource registry. Filtering is linear, and
//  in_should_wrap sets repeat wrapping explicitly, though OpenGL repeats by
//  default anyway. The channel count follows from in_internal_format, or is -1
//  for an unknown one. Leaves the texture bound to texture unit 0 and to the
//  active unit
//
//  in:     in_width_pixels, in_height_pixels, in_internal_format,
//            in_data_format, in_should_wrap
//  in/out: inout_texture
void texture_2d_init(
  struct texture_2d_t* inout_texture,
  unsigned int in_width_pixels,
  unsigned int in_height_pixels,
  enum texture_2d_internal_format_t in_internal_format,
  enum texture_2d_data_format_t in_data_format,
  bool in_should_wrap);

// Deletes a texture's graphics memory, if it holds any, and removes it from
//  the resource registry, leaving it unloaded with invalid fields. Cleaning up
//  a texture twice is harmless
//
//  in/out: inout_texture
void texture_2d_cleanup(
  struct texture_2d_t* inout_texture);

// Reallocates a texture at a new size, leaving its pixels undefined, and
//  updates its registry entry. The GL backend reallocates whichever texture is
//  bound to the active unit, so the texture must be bound there first
//
//  in:     in_new_width_pixels, in_new_height_pixels
//  in/out: inout_texture
void texture_2d_resize(
  struct texture_2d_t* inout_texture,
  unsigned int in_new_width_pixels,
  unsigned int in_new_height_pixels);

/*******************************************************************************
 *
 *   PIXEL DATA
 *
 ******************************************************************************/

// Uploads the whole of an initialized texture's pixels. The texture doesn't
//  keep data, so the caller may release it as soon as this returns. Nothing is
//  uploaded unless in_data_size_bytes is the texture's width times its height
//  times its channels (DEBUG builds log the mismatch)
//
//  in:     in_texture, in_data, in_data_size_bytes
void texture_2d_set_data(
  const struct texture_2d_t* in_texture,
  const unsigned char* in_data,
  unsigned int in_data_size_bytes);

// Uploads a rectangle of pixels into an already initialized texture, leaving
//  the rest of it untouched. in_data points at the rectangle's first pixel,
//  in the texture's data format, and successive rows of the rectangle start
//  in_row_length_pixels apart
//
//  in:     in_texture, in_x_pixels, in_y_pixels, in_width_pixels,
//            in_height_pixels, in_row_length_pixels, in_data
void texture_2d_set_sub_data(
  const struct texture_2d_t* in_texture,
  uint32_t in_x_pixels,
  uint32_t in_y_pixels,
  uint32_t in_width_pixels,
  uint32_t in_height_pixels,
  uint32_t in_row_length_pixels,
  const void* in_data);

// Loads an image file into a new texture that wraps, its formats chosen by
//  the image's channel count. in_should_flip_vertically stays set for any later
//  loads through stb_image. When the file can't be loaded no texture is
//  created, though its width, height and channels may have been written. An
//  image of other than 1, 3 or 4 channels keeps whatever formats the texture
//  held before
//
//  in:     in_file_path, in_should_flip_vertically
//  in/out: inout_texture
void texture_2d_create_from_file_path(
  struct texture_2d_t* inout_texture,
  const char* in_file_path,
  unsigned int in_should_flip_vertically);

/*******************************************************************************
 *
 *   BINDING
 *
 ******************************************************************************/

// Binds a texture to a texture unit
//
//  in:     in_texture, in_texture_slot
void texture_2d_bind(
  const struct texture_2d_t* in_texture,
  unsigned int in_texture_slot);
