#pragma once

#include "fvector.h"

#include <stdbool.h>

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

struct texture_2d_t
{
  unsigned char* data;
  unsigned int data_size_bytes;
  //
  unsigned int texture_id;
  unsigned int width, height;
  int channels;

  enum texture_2d_internal_format_t internal_format;
  enum texture_2d_data_format_t data_format;
  //
  unsigned int is_loaded;
};

struct subtexture_2d_t
{
  struct texture_2d_t* parent_texture;
  fvector2 subtexture_coordinates[2];
};

void texture_2d_init(
  struct texture_2d_t* texture,
  unsigned int width,
  unsigned int height,
  enum texture_2d_internal_format_t internal_format,
  enum texture_2d_data_format_t data_format,
  bool wrap);
void texture_2d_cleanup(
  struct texture_2d_t* texture);

void texture_2d_resize(
  struct texture_2d_t* texture,
  unsigned int new_width,
  unsigned int new_height);

void texture_2d_set_data(
  struct texture_2d_t* texture,
  unsigned char* data,
  const unsigned int data_size_bytes);
void texture_2d_create_from_file_path(
  struct texture_2d_t* texture,
  const char* filepath,
  const unsigned int flip_vertically);

void texture_2d_bind(
  const struct texture_2d_t* texture,
  const unsigned int texture_slot);
