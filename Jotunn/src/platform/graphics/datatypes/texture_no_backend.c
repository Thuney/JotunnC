#include "texture.h"

#include "../../../core/graphics/datatypes/texture_internal.h"

/*******************************************************************************
 *
 *   Function: texture_2d_internal_platform_init()
 *
 ******************************************************************************/
void texture_2d_internal_platform_init(
  struct texture_2d_t* inout_texture,
  const bool in_should_wrap)
{
}

/*******************************************************************************
 *
 *   Function: texture_2d_internal_platform_resize()
 *
 ******************************************************************************/
void texture_2d_internal_platform_resize(
  const struct texture_2d_t* in_texture,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels)
{
}

/*******************************************************************************
 *
 *   Function: texture_2d_internal_platform_set_data()
 *
 ******************************************************************************/
void texture_2d_internal_platform_set_data(
  const struct texture_2d_t* in_texture,
  const void* in_data,
  const unsigned int in_data_size_bytes)
{
}

/*******************************************************************************
 *
 *   Function: texture_2d_internal_platform_bind()
 *
 ******************************************************************************/
void texture_2d_internal_platform_bind(
  const struct texture_2d_t* in_texture,
  const unsigned int in_texture_slot)
{
}

/*******************************************************************************
 *
 *   Function: texture_2d_internal_platform_set_sub_data()
 *
 ******************************************************************************/
void texture_2d_internal_platform_set_sub_data(
  const struct texture_2d_t* in_texture,
  const uint32_t in_x_pixels,
  const uint32_t in_y_pixels,
  const uint32_t in_width_pixels,
  const uint32_t in_height_pixels,
  const uint32_t in_row_length_pixels,
  const void* in_data)
{
}
