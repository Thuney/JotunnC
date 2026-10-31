#include "texture.h"
#include "texture_internal.h"
#include "log.h"

#include "allocator.h"
#include "../resource_internal.h"

#include <memory.h>

// stb_image allocates through Jotunn, so its memory is counted too
#define STBI_MALLOC(size_bytes) \
  allocator_allocate((size_bytes), ALLOCATOR_TAG_STB_IMAGE)
#define STBI_REALLOC(memory, size_bytes) \
  allocator_reallocate((memory), (size_bytes), ALLOCATOR_TAG_STB_IMAGE)
#define STBI_FREE(memory) \
  allocator_free(memory)

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#ifdef DEBUG
  #include <stdio.h>
#endif

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Estimates the graphics memory behind a texture from its size and format
//
//  in:     in_texture
//  ret:    The estimate in bytes; 0 for an unknown format
static size_t _texture_2d_get_size_bytes(
  const struct texture_2d_t* in_texture);

/*******************************************************************************
 *
 *   Function: texture_2d_init()
 *
 ******************************************************************************/
void texture_2d_init(
  struct texture_2d_t* inout_texture,
  const unsigned int in_width_pixels,
  const unsigned int in_height_pixels,
  const enum texture_2d_internal_format_t in_internal_format,
  const enum texture_2d_data_format_t in_data_format,
  const bool in_should_wrap)
{
  inout_texture->width       = in_width_pixels;
  inout_texture->height      = in_height_pixels;

  inout_texture->internal_format = in_internal_format;
  inout_texture->data_format     = in_data_format;

  switch (inout_texture->internal_format)
  {
    case TEXTURE_2D_INTERNAL_FORMAT_RED:
    case TEXTURE_2D_INTERNAL_FORMAT_DEPTH_COMPONENT:
    {
      inout_texture->channels = 1;
    }
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_RG:
    case TEXTURE_2D_INTERNAL_FORMAT_DEPTH_STENCIL:
    {
      inout_texture->channels = 2;
    }
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_RGB:
    {
      inout_texture->channels = 3;
    }
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_RGBA:
    {
      inout_texture->channels = 4;
    }
    break;
    default:
    {
      inout_texture->channels = -1;
    }
    break;
  }

  texture_2d_internal_platform_init(
    inout_texture,
    in_should_wrap);

  inout_texture->is_loaded = 1;

  resource_internal_register(
    &(inout_texture->resource),
    RESOURCE_KIND_TEXTURE,
    _texture_2d_get_size_bytes(inout_texture));
}

/*******************************************************************************
 *
 *   Function: texture_2d_cleanup()
 *
 ******************************************************************************/
void texture_2d_cleanup(
  struct texture_2d_t* inout_texture)
{
  if (inout_texture->is_loaded) {
    texture_2d_internal_platform_cleanup(inout_texture);
  }

  resource_internal_unregister(&(inout_texture->resource));

  inout_texture->texture_id = -1;
  inout_texture->width = -1;
  inout_texture->height = -1;
  inout_texture->data_format = -1;
  inout_texture->internal_format = -1;
  inout_texture->is_loaded = 0;
}

/*******************************************************************************
 *
 *   Function: texture_2d_resize()
 *
 ******************************************************************************/
void texture_2d_resize(
  struct texture_2d_t* inout_texture,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels)
{
  inout_texture->width  = in_new_width_pixels;
  inout_texture->height = in_new_height_pixels;
  texture_2d_internal_platform_resize(
    inout_texture,
    in_new_width_pixels,
    in_new_height_pixels);

  resource_internal_set_size(
    &(inout_texture->resource),
    _texture_2d_get_size_bytes(inout_texture));
}

/*******************************************************************************
 *
 *   Function: texture_2d_set_data()
 *
 ******************************************************************************/
void texture_2d_set_data(
  const struct texture_2d_t* in_texture,
  const unsigned char* in_data,
  const unsigned int in_data_size_bytes)
{
  if (in_data_size_bytes != (in_texture->width * in_texture->height * in_texture->channels)) {
  #ifdef DEBUG
    log_write(
      LOG_LEVEL_ERROR,
      "Provided data_size_bytes does not equal expected (computed) "
      "data_size_bytes");
  #endif

    return;
  }

  texture_2d_internal_platform_set_data(
    in_texture,
    in_data,
    in_data_size_bytes);
}

/*******************************************************************************
 *
 *   Function: texture_2d_set_sub_data()
 *
 ******************************************************************************/
void texture_2d_set_sub_data(
  const struct texture_2d_t* in_texture,
  const uint32_t in_x_pixels,
  const uint32_t in_y_pixels,
  const uint32_t in_width_pixels,
  const uint32_t in_height_pixels,
  const uint32_t in_row_length_pixels,
  const void* in_data)
{
  texture_2d_internal_platform_set_sub_data(
    in_texture,
    in_x_pixels,
    in_y_pixels,
    in_width_pixels,
    in_height_pixels,
    in_row_length_pixels,
    in_data);
}

/*******************************************************************************
 *
 *   Function: texture_2d_create_from_file_path()
 *
 ******************************************************************************/
void texture_2d_create_from_file_path(
  struct texture_2d_t* inout_texture,
  const char* in_file_path,
  const unsigned int in_should_flip_vertically)
{
  stbi_set_flip_vertically_on_load(in_should_flip_vertically);

  unsigned char* data =
    stbi_load(
      in_file_path,
      (int*)&(inout_texture->width),
      (int*)&(inout_texture->height),
      &(inout_texture->channels),
      0);

  const unsigned int data_size_bytes =
    (inout_texture->width * inout_texture->height * inout_texture->channels);

  switch (inout_texture->channels)
  {
    case 1:
    {
      inout_texture->data_format     = TEXTURE_2D_DATA_FORMAT_RED;
      inout_texture->internal_format = TEXTURE_2D_INTERNAL_FORMAT_RED;
    }
    break;
    case 3:
    {
      inout_texture->data_format     = TEXTURE_2D_DATA_FORMAT_RGB;
      inout_texture->internal_format = TEXTURE_2D_INTERNAL_FORMAT_RGB;
    }
    break;
    case 4:
    {
      inout_texture->data_format     = TEXTURE_2D_DATA_FORMAT_RGBA;
      inout_texture->internal_format = TEXTURE_2D_INTERNAL_FORMAT_RGBA;
    }
    break;
    default:
    {
    }
    break;
  }

  #ifdef DEBUG
    fprintf(
      stdout,
      "Texture Data Size Bytes = %u, Texture Channels = %u, Texture Data "
      "Format = %u, Texture Internal Format = %u\n",
      data_size_bytes,
      inout_texture->channels,
      inout_texture->data_format,
      inout_texture->internal_format);
  #endif

  if (data) {
    texture_2d_init(
      inout_texture,
      inout_texture->width,
      inout_texture->height,
      inout_texture->internal_format,
      inout_texture->data_format,
      true);

    texture_2d_set_data(
      inout_texture,
      data,
      data_size_bytes);

    // The pixels live on in the texture's graphics memory
    stbi_image_free(data);
  }
}

/*******************************************************************************
 *
 *   Function: texture_2d_bind()
 *
 ******************************************************************************/
void texture_2d_bind(
  const struct texture_2d_t* in_texture,
  const unsigned int in_texture_slot)
{
  texture_2d_internal_platform_bind(
    in_texture,
    in_texture_slot);
}

/*******************************************************************************
 *
 *   Function: _texture_2d_get_size_bytes()
 *
 ******************************************************************************/
static size_t _texture_2d_get_size_bytes(
  const struct texture_2d_t* in_texture)
{
  size_t bytes_per_pixel;

  switch (in_texture->internal_format)
  {
    case TEXTURE_2D_INTERNAL_FORMAT_RED:
    {
      bytes_per_pixel = 1;
    }
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_RG:
    {
      bytes_per_pixel = 2;
    }
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_RGB:
    {
      bytes_per_pixel = 3;
    }
    break;
    // Depth is stored in at least 24 bits, so is counted as 32 like the rest
    case TEXTURE_2D_INTERNAL_FORMAT_RGBA:
    case TEXTURE_2D_INTERNAL_FORMAT_DEPTH_COMPONENT:
    case TEXTURE_2D_INTERNAL_FORMAT_DEPTH_STENCIL:
    {
      bytes_per_pixel = 4;
    }
    break;
    default:
    {
      bytes_per_pixel = 0;
    }
    break;
  }

  return ((size_t)in_texture->width * in_texture->height * bytes_per_pixel);
}
