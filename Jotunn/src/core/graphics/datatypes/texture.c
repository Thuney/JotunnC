#include "texture.h"

#include <memory.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#ifdef DEBUG
   #include <stdio.h>
#endif

extern void platform_texture_2d_init(struct texture_2d_t* texture, const enum texture_2d_internal_format_t internal_format, bool wrap);
extern void platform_texture_2d_resize(struct texture_2d_t* texture, const int new_width, const int new_height);
extern void platform_texture_2d_cleanup(struct texture_2d_t* texture);
extern void platform_texture_2d_set_data(const struct texture_2d_t* texture, void* data, const unsigned int data_size_bytes, const enum texture_2d_data_format_t data_format);

extern void platform_texture_2d_bind(const struct texture_2d_t* texture, const unsigned int texture_slot);

void texture_2d_init(struct texture_2d_t* texture, const int width, const int height, const enum texture_2d_internal_format_t internal_format, bool wrap)
{
   texture->width    = width;
   texture->height   = height;
   
   switch (internal_format)
   {
      case TEXTURE_2D_INTERNAL_FORMAT_R8:
      case TEXTURE_2D_INTERNAL_FORMAT_R16:
      {
         
         texture->channels = 1;
      }
      break;
      case TEXTURE_2D_INTERNAL_FORMAT_RG8:
      case TEXTURE_2D_INTERNAL_FORMAT_RG16:
      {
         texture->channels = 2;
      }
      break;
      case TEXTURE_2D_INTERNAL_FORMAT_RGB8:
      case TEXTURE_2D_INTERNAL_FORMAT_RGB10:
      case TEXTURE_2D_INTERNAL_FORMAT_RGB12:
      {
         texture->channels = 3;
      }
      break;
      case TEXTURE_2D_INTERNAL_FORMAT_RGBA8:
      case TEXTURE_2D_INTERNAL_FORMAT_RGBA12:
      {
         texture->channels = 4;
      }
      break;
      default:
      {
         
      }
      break;
   }

   platform_texture_2d_init(texture, internal_format, wrap);
}

void texture_2d_cleanup(struct texture_2d_t* texture)
{
   if (texture->is_loaded) platform_texture_2d_cleanup(texture);

   // If you opt to be the owner of this texture's data, unbind the data pointer before calling texture cleanup
   if (texture->data) stbi_image_free(texture->data);
   
   texture->data_size_bytes = -1;
   texture->texture_id      = -1;
   texture->width           = -1;
   texture->height          = -1;
   texture->data            = 0;
   texture->data_format     = -1;
   texture->internal_format = -1;
}

void texture_2d_resize(struct texture_2d_t* texture, const int new_width, const int new_height)
{
   texture->width  = new_width;
   texture->height = new_height;
   platform_texture_2d_resize(texture, new_width, new_height);
}

void texture_2d_set_data(struct texture_2d_t* texture, unsigned char* data, const unsigned int data_size_bytes, const enum texture_2d_data_format_t data_format)
{
   if (data_size_bytes != (texture->width*texture->height*texture->channels))
   {
      #ifdef DEBUG
         fprintf(stdout, "Provided data_size_bytes does not equal expected (computed) data_size_bytes\n");
      #endif

      return;
   }

   texture->data            = data;
   texture->data_size_bytes = data_size_bytes;
   texture->data_format     = data_format;

   platform_texture_2d_set_data(texture, data, data_size_bytes, data_format);

   texture->is_loaded = 1;
}

void texture_2d_create_from_file_path(struct texture_2d_t* texture, const char* filepath, const unsigned int flip_vertically)
{
   stbi_set_flip_vertically_on_load(flip_vertically);
   texture->data = stbi_load(filepath, &texture->width, &texture->height, &texture->channels, 0);
   texture->data_size_bytes = (texture->width*texture->height*texture->channels);

   switch (texture->channels)
   {
      case 1:
      {
         texture->data_format     = TEXTURE_2D_FORMAT_RED;
         texture->internal_format = TEXTURE_2D_INTERNAL_FORMAT_R8;
      }
      break;
      case 3:
      {
         texture->data_format     = TEXTURE_2D_FORMAT_RGB;
         texture->internal_format = TEXTURE_2D_INTERNAL_FORMAT_RGB8;
      }
      break;
      case 4:
      {
         texture->data_format     = TEXTURE_2D_FORMAT_RGBA;
         texture->internal_format = TEXTURE_2D_INTERNAL_FORMAT_RGBA8;
      }
      break;
      default:
      {
         
      }
      break;

      #ifdef DEBUG
         fprintf(stdout, "Texture Data Size Bytes = %u, Texture Channels = %u, Texture Data Format = %u, Texture Internal Format = %u\n", texture->data_size_bytes, texture->channels, texture->data_format, texture->internal_format);
      #endif
   }

   if (texture->data)
   {
      texture->is_loaded = 1; // Indicate that we are already loaded so we don't needlessly re-set fields

      texture_2d_init(texture, texture->width, texture->height, texture->internal_format, true);
      texture_2d_set_data(texture, texture->data, texture->data_size_bytes, texture->data_format);
   } 
}

void texture_2d_bind(const struct texture_2d_t* texture, const unsigned int texture_slot)
{
   platform_texture_2d_bind(texture, texture_slot);
}