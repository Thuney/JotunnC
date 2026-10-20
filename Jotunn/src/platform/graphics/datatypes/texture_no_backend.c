#include "texture.h"

void platform_texture_2d_init(
  struct texture_2d_t* texture,
  bool wrap)
{
}

void platform_texture_2d_resize(
  struct texture_2d_t* texture,
  const int new_width,
  const int new_height)
{
}

void platform_texture_2d_set_data(
  const struct texture_2d_t* texture,
  void* data,
  const unsigned int data_size_bytes)
{
}

void platform_texture_2d_bind(
  const struct texture_2d_t* texture,
  const unsigned int texture_slot)
{
}

void platform_texture_2d_set_sub_data(
  const struct texture_2d_t* in_texture,
  const uint32_t in_x_pixels,
  const uint32_t in_y_pixels,
  const uint32_t in_width_pixels,
  const uint32_t in_height_pixels,
  const uint32_t in_row_length_pixels,
  const void* in_data)
{
}
