#include "texture.h"

#include <GL/glew.h>

static GLenum gl_get_texture_data_format(enum texture_2d_data_format_t data_format)
{
   GLenum gl_data_format;

   switch(data_format)
   {
      case TEXTURE_2D_FORMAT_RED:
         gl_data_format = GL_RED;
      break;
      case TEXTURE_2D_FORMAT_RG:
         gl_data_format = GL_RG;
      break;
      case TEXTURE_2D_FORMAT_RGB:
         gl_data_format = GL_RGB;
      break;
      case TEXTURE_2D_FORMAT_BGR:
         gl_data_format = GL_BGR;
      break;
      case TEXTURE_2D_FORMAT_RGBA:
         gl_data_format = GL_RGBA;
      break;
      case TEXTURE_2D_FORMAT_BGRA:
         gl_data_format = GL_BGRA;
      break;
      case TEXTURE_2D_FORMAT_DEPTH_COMPONENT:
         gl_data_format = GL_DEPTH_COMPONENT;
      break;
      case TEXTURE_2D_FORMAT_STENCIL_INDEX:
         gl_data_format = GL_STENCIL_INDEX;
      break;
      default:
         gl_data_format = GL_INVALID_ENUM;
      break;
   }
   
   return gl_data_format;
}

static GLenum gl_get_texture_internal_format(enum texture_2d_internal_format_t internal_format)
{
   GLenum gl_internal_format;

   switch(internal_format)
   {
      case TEXTURE_2D_INTERNAL_FORMAT_R8:
         gl_internal_format = GL_R8;
      break;
      case TEXTURE_2D_INTERNAL_FORMAT_R16:
         gl_internal_format = GL_R16;
      break;
      case TEXTURE_2D_INTERNAL_FORMAT_RG8:
         gl_internal_format = GL_RG8;
      break;
      case TEXTURE_2D_INTERNAL_FORMAT_RG16:
         gl_internal_format = GL_RG16;
      break;
      case TEXTURE_2D_INTERNAL_FORMAT_RGB8:
         gl_internal_format = GL_RGB8;
      break;
      case TEXTURE_2D_INTERNAL_FORMAT_RGB10:
         gl_internal_format = GL_RGB10;
      break;
      case TEXTURE_2D_INTERNAL_FORMAT_RGB12:
         gl_internal_format = GL_RGB12;
      break;
      case TEXTURE_2D_INTERNAL_FORMAT_RGBA8:
         gl_internal_format = GL_RGBA8;
      break;
      case TEXTURE_2D_INTERNAL_FORMAT_RGBA12:
         gl_internal_format = GL_RGBA12;
      break;
      default:
         gl_internal_format = GL_INVALID_ENUM;
      break;
   }
   
   return gl_internal_format;
}

static void gl_texture_2d_init(struct texture_2d_t* texture, const enum texture_2d_internal_format_t internal_format)
{
   glCreateTextures(GL_TEXTURE_2D, 1, &texture->texture_id);
   glTextureStorage2D(texture->texture_id, 1, gl_get_texture_internal_format(internal_format), texture->width, texture->height);

   /*
	 * Set the modes for texture filtering
    * 'Minimizing' filter for down-scaling textures
    * 'Magnifying' filter for up-scaling textures
	 */
   glTextureParameteri(texture->texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTextureParameteri(texture->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   /*
	 * Specify how the texture will wrap when texture coordinates are out of the normal range
	 * In this case, the texture will 'repeat' for S and T texture coordinates
	 */
   glTextureParameteri(texture->texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTextureParameteri(texture->texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

static void gl_texture_2d_cleanup(struct texture_2d_t* texture)
{
   glDeleteTextures(1, &texture->texture_id);
}

static void gl_texture_2d_bind(const struct texture_2d_t* texture, const unsigned int texture_slot)
{
   glBindTextureUnit(texture_slot, texture->texture_id);
}

static void gl_texture_2d_set_data(const struct texture_2d_t* texture, void* data, const unsigned int data_size_bytes, const enum texture_2d_data_format_t data_format)
{
   glTextureSubImage2D(texture->texture_id, 0, 0, 0, texture->width, texture->height, gl_get_texture_data_format(data_format), GL_UNSIGNED_BYTE, data);
}

//
//
//

void platform_texture_2d_init(struct texture_2d_t* texture, const enum texture_2d_internal_format_t internal_format)
{
   gl_texture_2d_init(texture, internal_format);
}

void platform_texture_2d_cleanup(struct texture_2d_t* texture)
{
   gl_texture_2d_cleanup(texture);
}

void platform_texture_2d_set_data(const struct texture_2d_t* texture, void* data, const unsigned int data_size_bytes, const enum texture_2d_data_format_t data_format)
{
   gl_texture_2d_set_data(texture, data, data_size_bytes, data_format);
}

void platform_texture_2d_bind(const struct texture_2d_t* texture, const unsigned int texture_slot)
{
   gl_texture_2d_bind(texture, texture_slot);
}