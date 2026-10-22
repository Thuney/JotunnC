#include "texture.h"

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

extern void platform_texture_2d_init(
  struct texture_2d_t* texture,
  bool wrap);
extern void platform_texture_2d_cleanup(
  struct texture_2d_t* texture);

extern void platform_texture_2d_bind(
  const struct texture_2d_t* texture,
  unsigned int texture_slot);

extern void platform_texture_2d_set_data(
  const struct texture_2d_t* texture,
  void* data,
  unsigned int data_size_bytes);

extern void platform_texture_2d_set_sub_data(
  const struct texture_2d_t* in_texture,
  const uint32_t in_x_pixels,
  const uint32_t in_y_pixels,
  const uint32_t in_width_pixels,
  const uint32_t in_height_pixels,
  const uint32_t in_row_length_pixels,
  const void* in_data);

extern void platform_texture_2d_resize(
  struct texture_2d_t* texture,
  unsigned int new_width,
  unsigned int new_height);

// Estimates the graphics memory behind a texture from its size and format
//
//  in:     in_texture
//  ret:    The estimate in bytes; 0 for an unknown format
static size_t _texture_2d_get_size_bytes(
  const struct texture_2d_t* in_texture);

void texture_2d_init(
  struct texture_2d_t* texture,
  unsigned int width,
  unsigned int height,
  enum texture_2d_internal_format_t internal_format,
  enum texture_2d_data_format_t data_format,
  bool wrap)
{
  texture->width       = width;
  texture->height      = height;

  texture->internal_format = internal_format;
  texture->data_format     = data_format;

  switch (texture->internal_format)
  {
    case TEXTURE_2D_INTERNAL_FORMAT_RED:
    case TEXTURE_2D_INTERNAL_FORMAT_DEPTH_COMPONENT:
    {
      texture->channels = 1;
    }
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_RG:
    case TEXTURE_2D_INTERNAL_FORMAT_DEPTH_STENCIL:
    {
      texture->channels = 2;
    }
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_RGB:
    {
      texture->channels = 3;
    }
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_RGBA:
    {
      texture->channels = 4;
    }
    break;
    default:
    {
      texture->channels = -1;
    }
    break;
  }

  platform_texture_2d_init(texture, wrap);

  texture->is_loaded = 1;

  resource_internal_register(
    &(texture->resource),
    RESOURCE_KIND_TEXTURE,
    _texture_2d_get_size_bytes(texture));
}

void texture_2d_cleanup(
  struct texture_2d_t* texture)
{
  if (texture->is_loaded)
  {
    platform_texture_2d_cleanup(texture);
  }

  resource_internal_unregister(&(texture->resource));

  texture->texture_id = -1;
  texture->width = -1;
  texture->height = -1;
  texture->data_format = -1;
  texture->internal_format = -1;
  texture->is_loaded = 0;
}

void texture_2d_resize(
  struct texture_2d_t* texture,
  unsigned int new_width,
  unsigned int new_height)
{
  texture->width  = new_width;
  texture->height = new_height;
  platform_texture_2d_resize(texture, new_width, new_height);

  resource_internal_set_size(
    &(texture->resource),
    _texture_2d_get_size_bytes(texture));
}

void texture_2d_set_data(
  struct texture_2d_t* texture,
  unsigned char* data,
  const unsigned int data_size_bytes)
{
  if (data_size_bytes != (texture->width * texture->height * texture->channels))
  {
  #ifdef DEBUG
    fprintf(
      stdout,
      "Provided data_size_bytes does not equal expected (computed) "
      "data_size_bytes\n");
  #endif

    return;
  }

  platform_texture_2d_set_data(texture, data, data_size_bytes);
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
  platform_texture_2d_set_sub_data(
    in_texture,
    in_x_pixels,
    in_y_pixels,
    in_width_pixels,
    in_height_pixels,
    in_row_length_pixels,
    in_data);
}

void texture_2d_create_from_file_path(
  struct texture_2d_t* texture,
  const char* filepath,
  const unsigned int flip_vertically)
{
  stbi_set_flip_vertically_on_load(flip_vertically);

  unsigned char* data =
    stbi_load(
      filepath,
      (int*)&(texture->width),
      (int*)&(texture->height),
      &(texture->channels),
      0);

  const unsigned int data_size_bytes =
    (texture->width * texture->height * texture->channels);

  switch (texture->channels)
  {
    case 1:
    {
      texture->data_format     = TEXTURE_2D_DATA_FORMAT_RED;
      texture->internal_format = TEXTURE_2D_INTERNAL_FORMAT_RED;
    }
    break;
    case 3:
    {
      texture->data_format     = TEXTURE_2D_DATA_FORMAT_RGB;
      texture->internal_format = TEXTURE_2D_INTERNAL_FORMAT_RGB;
    }
    break;
    case 4:
    {
      texture->data_format     = TEXTURE_2D_DATA_FORMAT_RGBA;
      texture->internal_format = TEXTURE_2D_INTERNAL_FORMAT_RGBA;
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
      texture->channels,
      texture->data_format,
      texture->internal_format);
  #endif

  if (data)
  {
    texture_2d_init(
      texture,
      texture->width,
      texture->height,
      texture->internal_format,
      texture->data_format,
      true);

    texture_2d_set_data(
      texture,
      data,
      data_size_bytes);

    // The pixels live on in the texture's graphics memory
    stbi_image_free(data);
  }
}

void texture_2d_bind(
  const struct texture_2d_t* texture,
  const unsigned int texture_slot)
{
  platform_texture_2d_bind(texture, texture_slot);
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
