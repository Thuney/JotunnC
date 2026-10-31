#include "texture.h"
#include "log.h"

#include "../../../core/graphics/datatypes/texture_internal.h"

#include <GL/glew.h>

#ifdef DEBUG
  #include <stdio.h>
  #include <stdarg.h>
#endif

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Takes OpenGL's pending error, clearing it. In DEBUG builds, logs
//  in_success_string at debug level when there was none, and otherwise
//  in_error_string and the error code at error level, formatting either with
//  the remaining arguments. Its calls are all commented out at present
//
//  in:     in_success_string, in_error_string
static void texture_gl_check_error(
  const char* in_success_string,
  const char* in_error_string,
  ...);

// Translates a texture data format to OpenGL's. The integer formats map to
//  the plain ones, as every upload is of unsigned bytes
//
//  in:     in_data_format
//  ret:    The OpenGL format; GL_INVALID_ENUM for an unknown one
static GLenum texture_gl_get_data_format(
  const enum texture_2d_data_format_t in_data_format);

// Translates a texture internal format to OpenGL's
//
//  in:     in_internal_format
//  ret:    The OpenGL format; GL_INVALID_ENUM for an unknown one
static GLenum texture_gl_get_internal_format(
  const enum texture_2d_internal_format_t in_internal_format);

// Generates a texture, binds it to the active unit and to unit 0, sets linear
//  filtering, and allocates its storage at the size and formats set in it
//
//  in:     in_should_wrap
//  in/out: inout_texture
static void texture_2d_gl_init(
  struct texture_2d_t* inout_texture,
  const bool in_should_wrap);

// Deletes a texture
//
//  in:     in_texture
static void texture_2d_gl_cleanup(
  const struct texture_2d_t* in_texture);

// Binds a texture to a texture unit
//
//  in:     in_texture, in_texture_slot
static void texture_2d_gl_bind(
  const struct texture_2d_t* in_texture,
  const unsigned int in_texture_slot);

// Uploads the whole of a texture's pixels; in_data_size_bytes isn't used
//
//  in:     in_texture, in_data, in_data_size_bytes
static void texture_2d_gl_set_data(
  const struct texture_2d_t* in_texture,
  const void* in_data,
  const unsigned int in_data_size_bytes);

// Reallocates whichever texture is bound to the active unit at the size and
//  formats set in in_texture; in_new_width_pixels and in_new_height_pixels
//  aren't used
//
//  in:     in_texture, in_new_width_pixels, in_new_height_pixels
static void texture_2d_gl_resize(
  const struct texture_2d_t* in_texture,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels);

/*******************************************************************************
 *
 *   Function: texture_2d_internal_platform_init()
 *
 ******************************************************************************/
void texture_2d_internal_platform_init(
  struct texture_2d_t* inout_texture,
  const bool in_should_wrap)
{
  texture_2d_gl_init(
    inout_texture,
    in_should_wrap);
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
  texture_2d_gl_resize(
    in_texture,
    in_new_width_pixels,
    in_new_height_pixels);
}

/*******************************************************************************
 *
 *   Function: texture_2d_internal_platform_cleanup()
 *
 ******************************************************************************/
void texture_2d_internal_platform_cleanup(
  const struct texture_2d_t* in_texture)
{
  texture_2d_gl_cleanup(in_texture);
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
  texture_2d_gl_set_data(
    in_texture,
    in_data,
    in_data_size_bytes);
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
  glPixelStorei(
    GL_UNPACK_ROW_LENGTH,
    (GLint)in_row_length_pixels);

  glTextureSubImage2D(
    in_texture->texture_id,
    0,
    (GLint)in_x_pixels,
    (GLint)in_y_pixels,
    (GLsizei)in_width_pixels,
    (GLsizei)in_height_pixels,
    texture_gl_get_data_format(in_texture->data_format),
    GL_UNSIGNED_BYTE,
    in_data);

  // Zero means rows are tightly packed, which every other upload assumes
  glPixelStorei(
    GL_UNPACK_ROW_LENGTH,
    0);
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
  texture_2d_gl_bind(
    in_texture,
    in_texture_slot);
}

/*******************************************************************************
 *
 *   Function: texture_gl_check_error()
 *
 ******************************************************************************/
static void texture_gl_check_error(
  const char* in_success_string,
  const char* in_error_string,
  ...)
{
  GLenum _gl_error = GL_NO_ERROR;
  _gl_error = glGetError();

#ifdef DEBUG
  va_list args;
  va_start(
    args,
    in_error_string);

  if (_gl_error == GL_NO_ERROR) {
    log_write_va(
      LOG_LEVEL_DEBUG,
      in_success_string,
      args);
  }
  else {
    log_write_va(
      LOG_LEVEL_ERROR,
      in_error_string,
      args);
    log_write(
      LOG_LEVEL_ERROR,
      "--> Error Code: %d",
      _gl_error);
  }

  va_end(args);
#endif
}

/*******************************************************************************
 *
 *   Function: texture_gl_get_data_format()
 *
 ******************************************************************************/
static GLenum texture_gl_get_data_format(
  const enum texture_2d_data_format_t in_data_format)
{
  GLenum gl_data_format;

  switch (in_data_format)
  {
    case TEXTURE_2D_DATA_FORMAT_RED:
      gl_data_format = GL_RED;
    break;
    case TEXTURE_2D_DATA_FORMAT_RG:
      gl_data_format = GL_RG;
    break;
    case TEXTURE_2D_DATA_FORMAT_RGB:
      gl_data_format = GL_RGB;
    break;
    case TEXTURE_2D_DATA_FORMAT_BGR:
      gl_data_format = GL_BGR;
    break;
    case TEXTURE_2D_DATA_FORMAT_RGBA:
      gl_data_format = GL_RGBA;
    break;
    case TEXTURE_2D_DATA_FORMAT_BGRA:
      gl_data_format = GL_BGRA;
    break;
    case TEXTURE_2D_DATA_FORMAT_RED_INTEGER:
      gl_data_format = GL_RED;
    break;
    case TEXTURE_2D_DATA_FORMAT_RG_INTEGER:
      gl_data_format = GL_RG;
    break;
    case TEXTURE_2D_DATA_FORMAT_RGB_INTEGER:
      gl_data_format = GL_RGB;
    break;
    case TEXTURE_2D_DATA_FORMAT_BGR_INTEGER:
      gl_data_format = GL_BGR;
    break;
    case TEXTURE_2D_DATA_FORMAT_RGBA_INTEGER:
      gl_data_format = GL_RGBA;
    break;
    case TEXTURE_2D_DATA_FORMAT_BGRA_INTEGER:
      gl_data_format = GL_BGRA;
    break;
    case TEXTURE_2D_DATA_FORMAT_STENCIL_INDEX:
      gl_data_format = GL_STENCIL_INDEX;
    break;
    case TEXTURE_2D_DATA_FORMAT_DEPTH_COMPONENT:
      gl_data_format = GL_DEPTH_COMPONENT;
    break;
    case TEXTURE_2D_DATA_FORMAT_DEPTH_STENCIL:
      gl_data_format = GL_DEPTH_STENCIL;
    break;
    default:
      gl_data_format = GL_INVALID_ENUM;
    break;
  }

  return gl_data_format;
}

/*******************************************************************************
 *
 *   Function: texture_gl_get_internal_format()
 *
 ******************************************************************************/
static GLenum texture_gl_get_internal_format(
  const enum texture_2d_internal_format_t in_internal_format)
{
  GLenum gl_internal_format;

  switch (in_internal_format)
  {
    case TEXTURE_2D_INTERNAL_FORMAT_RED:
      gl_internal_format = GL_RED;
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_RG:
      gl_internal_format = GL_RG;
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_RGB:
      gl_internal_format = GL_RGB;
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_RGBA:
      gl_internal_format = GL_RGBA;
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_DEPTH_COMPONENT:
      gl_internal_format = GL_DEPTH_COMPONENT;
    break;
    case TEXTURE_2D_INTERNAL_FORMAT_DEPTH_STENCIL:
      gl_internal_format = GL_DEPTH_STENCIL;
    break;
    default:
      gl_internal_format = GL_INVALID_ENUM;
    break;
  }

  return gl_internal_format;
}

/*******************************************************************************
 *
 *   Function: texture_2d_gl_init()
 *
 ******************************************************************************/
static void texture_2d_gl_init(
  struct texture_2d_t* inout_texture,
  const bool in_should_wrap)
{
  // glCreateTextures(GL_TEXTURE_2D, 1, &inout_texture->texture_id);

  // texture_gl_check_error(
  //   "No error before texture init\n",
  //   "Error occurred prior to texture init\n");

  glGenTextures(
    1,
    &(inout_texture->texture_id));

  // texture_gl_check_error(
  //   "Texture initialized with ID %d\n",
  //   "Error occurred during texture generation: [ID %d]\n",
  //   inout_texture->texture_id);

  glBindTexture(
    GL_TEXTURE_2D,
    inout_texture->texture_id);
  texture_2d_gl_bind(
    inout_texture,
    0);

  /*
   * Set the modes for texture filtering
   * 'Minimizing' filter for down-scaling textures
   * 'Magnifying' filter for up-scaling textures
   */
  glTextureParameteri(
    inout_texture->texture_id,
    GL_TEXTURE_MIN_FILTER,
    GL_LINEAR);
  glTextureParameteri(
    inout_texture->texture_id,
    GL_TEXTURE_MAG_FILTER,
    GL_LINEAR);

  if (in_should_wrap) {
    /*
     * Specify how the texture will wrap when texture coordinates are out of the normal range
     * In this case, the texture will 'repeat' for S and T texture coordinates
     */
    glTextureParameteri(
      inout_texture->texture_id,
      GL_TEXTURE_WRAP_S,
      GL_REPEAT);
    glTextureParameteri(
      inout_texture->texture_id,
      GL_TEXTURE_WRAP_T,
      GL_REPEAT);
  }

  const GLenum gl_internal_format = texture_gl_get_internal_format(inout_texture->internal_format);

  const GLenum gl_data_format = texture_gl_get_data_format(inout_texture->data_format);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    gl_internal_format,
    inout_texture->width,
    inout_texture->height,
    0,
    gl_data_format,
    GL_UNSIGNED_BYTE,
    NULL);

  // texture_gl_check_error(
  //   "Successfully specified image data for Texture ID %d\n",
  //   "Error occurred during image data specification for Texture ID %d\n",
  //   inout_texture->texture_id);
}

/*******************************************************************************
 *
 *   Function: texture_2d_gl_cleanup()
 *
 ******************************************************************************/
static void texture_2d_gl_cleanup(
  const struct texture_2d_t* in_texture)
{
  glDeleteTextures(
    1,
    &(in_texture->texture_id));
}

/*******************************************************************************
 *
 *   Function: texture_2d_gl_bind()
 *
 ******************************************************************************/
static void texture_2d_gl_bind(
  const struct texture_2d_t* in_texture,
  const unsigned int in_texture_slot)
{
  glBindTextureUnit(
    in_texture_slot,
    in_texture->texture_id);

  // texture_gl_check_error(
  //   "Texture ID %d bound to Texture Slot %d\n",
  //   "Error occurred while binding Texture ID %d to Texture Slot %d\n",
  //   in_texture->texture_id,
  //   in_texture_slot);
}

/*******************************************************************************
 *
 *   Function: texture_2d_gl_set_data()
 *
 ******************************************************************************/
static void texture_2d_gl_set_data(
  const struct texture_2d_t* in_texture,
  const void* in_data,
  const unsigned int in_data_size_bytes)
{
  glTextureSubImage2D(
    in_texture->texture_id,
    0,
    0,
    0,
    in_texture->width,
    in_texture->height,
    texture_gl_get_data_format(in_texture->data_format),
    GL_UNSIGNED_BYTE,
    in_data);
  // glGenerateMipmap(GL_TEXTURE_2D);
}

/*******************************************************************************
 *
 *   Function: texture_2d_gl_resize()
 *
 ******************************************************************************/
static void texture_2d_gl_resize(
  const struct texture_2d_t* in_texture,
  const unsigned int in_new_width_pixels,
  const unsigned int in_new_height_pixels)
{
  const GLenum gl_internal_format = texture_gl_get_internal_format(in_texture->internal_format);

  const GLenum gl_data_format = texture_gl_get_data_format(in_texture->data_format);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    gl_internal_format,
    in_texture->width,
    in_texture->height,
    0,
    gl_data_format,
    GL_UNSIGNED_BYTE,
    NULL);
}
