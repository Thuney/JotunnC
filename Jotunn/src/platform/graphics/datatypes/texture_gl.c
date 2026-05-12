#include "texture.h"

#include <GL/glew.h>

#ifdef DEBUG
  #include <stdio.h>
  #include <stdarg.h>

  static void gl_error_check(
    const char* success_string,
    const char* error_string,
    ...);
#endif

static GLenum gl_get_texture_data_format(
  enum texture_2d_data_format_t data_format);

static GLenum gl_get_texture_internal_format(
  enum texture_2d_internal_format_t internal_format);

static void gl_texture_2d_init(
  struct texture_2d_t* texture,
  bool wrap);

static void gl_texture_2d_cleanup(
  struct texture_2d_t* texture);

static void gl_texture_2d_bind(
  const struct texture_2d_t* texture,
  const unsigned int texture_slot);

static void gl_texture_2d_set_data(
  const struct texture_2d_t* texture,
  void* data,
  const unsigned int data_size_bytes);

static void gl_texture_2d_resize(
  struct texture_2d_t* texture,
  const int new_width,
  const int new_height);

// -----------------------------------------------------------------------------

void platform_texture_2d_init(
  struct texture_2d_t* texture,
  bool wrap)
{
  gl_texture_2d_init(texture, wrap);
}

void platform_texture_2d_resize(
  struct texture_2d_t* texture,
  const int new_width,
  const int new_height)
{
  gl_texture_2d_resize(texture, new_width, new_height);
}

void platform_texture_2d_cleanup(
  struct texture_2d_t* texture)
{
  gl_texture_2d_cleanup(texture);
}

void platform_texture_2d_set_data(
  const struct texture_2d_t* texture,
  void* data,
  const unsigned int data_size_bytes)
{
  gl_texture_2d_set_data(texture, data, data_size_bytes);
}

void platform_texture_2d_bind(
  const struct texture_2d_t* texture,
  const unsigned int texture_slot)
{
  gl_texture_2d_bind(texture, texture_slot);
}

// -----------------------------------------------------------------------------

static void gl_error_check(
  const char* success_string,
  const char* error_string,
  ...)
{
  GLenum _gl_error = GL_NO_ERROR;
  _gl_error = glGetError();

#ifdef DEBUG
  va_list args;
  va_start(args, error_string);

  if (_gl_error == GL_NO_ERROR)
  {
    vprintf(success_string, args);
  }
  else
  {
    vprintf(error_string, args);
    printf("--> Error Code: %d\n", _gl_error);
  }

  va_end(args);
#endif
}

static GLenum gl_get_texture_data_format(
  enum texture_2d_data_format_t data_format)
{
  GLenum gl_data_format;

  switch (data_format)
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

static GLenum gl_get_texture_internal_format(
  enum texture_2d_internal_format_t internal_format)
{
  GLenum gl_internal_format;

  switch (internal_format)
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

static void gl_texture_2d_init(
  struct texture_2d_t* texture,
  bool wrap)
{
  // glCreateTextures(GL_TEXTURE_2D, 1, &texture->texture_id);

  // gl_error_check(
  //   "No error before texture init\n",
  //   "Error occurred prior to texture init\n");

  glGenTextures(1, &(texture->texture_id));

  // gl_error_check(
  //   "Texture initialized with ID %d\n",
  //   "Error occurred during texture generation: [ID %d]\n",
  //   texture->texture_id);

  glBindTexture(GL_TEXTURE_2D, texture->texture_id);
  gl_texture_2d_bind(texture, 0);

  /*
   * Set the modes for texture filtering
   * 'Minimizing' filter for down-scaling textures
   * 'Magnifying' filter for up-scaling textures
   */
  glTextureParameteri(texture->texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(texture->texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (wrap)
  {
    /*
     * Specify how the texture will wrap when texture coordinates are out of the normal range
     * In this case, the texture will 'repeat' for S and T texture coordinates
     */
    glTextureParameteri(texture->texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture->texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  const GLenum gl_internal_format =
    gl_get_texture_internal_format(texture->internal_format);

  const GLenum gl_data_format =
    gl_get_texture_data_format(texture->data_format);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    gl_internal_format,
    texture->width,
    texture->height,
    0,
    gl_data_format,
    GL_UNSIGNED_BYTE,
    NULL);

  // gl_error_check(
  //   "Successfully specified image data for Texture ID %d\n",
  //   "Error occurred during image data specification for Texture ID %d\n",
  //   texture->texture_id);
}

static void gl_texture_2d_cleanup(
  struct texture_2d_t* texture)
{
  glDeleteTextures(1, &texture->texture_id);
}

static void gl_texture_2d_bind(
  const struct texture_2d_t* texture,
  const unsigned int texture_slot)
{
  glBindTextureUnit(texture_slot, texture->texture_id);

  // gl_error_check(
  //   "Texture ID %d bound to Texture Slot %d\n",
  //   "Error occurred while binding Texture ID %d to Texture Slot %d\n",
  //   texture->texture_id,
  //   texture_slot);
}

static void gl_texture_2d_set_data(
  const struct texture_2d_t* texture,
  void* data,
  const unsigned int data_size_bytes)
{
  glTextureSubImage2D(
    texture->texture_id,
    0,
    0,
    0,
    texture->width,
    texture->height,
    gl_get_texture_data_format(texture->data_format),
    GL_UNSIGNED_BYTE,
    data);
  // glGenerateMipmap(GL_TEXTURE_2D);
}

static void gl_texture_2d_resize(
  struct texture_2d_t* texture,
  const int new_width,
  const int new_height)
{
  const GLenum gl_internal_format =
    gl_get_texture_internal_format(texture->internal_format);

  const GLenum gl_data_format =
    gl_get_texture_data_format(texture->data_format);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    gl_internal_format,
    texture->width,
    texture->height,
    0,
    gl_data_format,
    GL_UNSIGNED_BYTE,
    NULL);
}
