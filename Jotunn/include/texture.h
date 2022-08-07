#pragma once

enum texture_2d_internal_format_t
{
   TEXTURE_2D_INTERNAL_FORMAT_R8 = 0x00,
   TEXTURE_2D_INTERNAL_FORMAT_R16,
   TEXTURE_2D_INTERNAL_FORMAT_RG8,
   TEXTURE_2D_INTERNAL_FORMAT_RG16,
   TEXTURE_2D_INTERNAL_FORMAT_RGB8,
   TEXTURE_2D_INTERNAL_FORMAT_RGB10,
   TEXTURE_2D_INTERNAL_FORMAT_RGB12,
   TEXTURE_2D_INTERNAL_FORMAT_RGBA8,
   TEXTURE_2D_INTERNAL_FORMAT_RGBA12,
};

enum texture_2d_data_format_t
{
   TEXTURE_2D_FORMAT_RED = 0x00,
   TEXTURE_2D_FORMAT_RG,
   TEXTURE_2D_FORMAT_RGB,
   TEXTURE_2D_FORMAT_BGR,
   TEXTURE_2D_FORMAT_RGBA,
   TEXTURE_2D_FORMAT_BGRA,
   TEXTURE_2D_FORMAT_DEPTH_COMPONENT,
   TEXTURE_2D_FORMAT_STENCIL_INDEX,
};

struct texture_2d_t
{
   unsigned char* data;
   unsigned int data_size_bytes;
   //
   unsigned int texture_id;
   int width, height, channels;
   enum texture_2d_internal_format_t internal_format;
   enum texture_2d_data_format_t data_format;
   //
   unsigned int is_loaded;
};

void texture_2d_init(struct texture_2d_t* texture, const int width, const int height, const enum texture_2d_internal_format_t internal_format);
void texture_2d_cleanup(struct texture_2d_t* texture);

void texture_2d_set_data(struct texture_2d_t* texture, unsigned char* data, const unsigned int data_size_bytes, const enum texture_2d_data_format_t data_format);
void texture_2d_create_from_file_path(struct texture_2d_t* texture, const char* filepath, const unsigned int flip_vertically);

void texture_2d_bind(const struct texture_2d_t* texture, const unsigned int texture_slot);