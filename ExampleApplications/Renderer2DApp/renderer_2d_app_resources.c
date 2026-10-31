#include "renderer_2d_app_resources.h"

#include <stdbool.h>
#include <stdio.h>

#define RENDERER_2D_APP_RESOURCES_IMAGE_PATH \
  EXAMPLE_RES_PATH "/textures/AaronShakespeare.png"
#define RENDERER_2D_APP_RESOURCES_TYPEFACE_SIZE_PIXELS 20

// The texture made in code: a checkerboard of 8 by 8 cells, each 8 pixels
//  square, with 4 bytes (RGBA) to a pixel
#define RENDERER_2D_APP_RESOURCES_CHECKER_SIZE_PIXELS 64
#define RENDERER_2D_APP_RESOURCES_CHECKER_CELL_PIXELS 8
#define RENDERER_2D_APP_RESOURCES_CHECKER_CHANNELS 4
#define RENDERER_2D_APP_RESOURCES_CHECKER_SIZE_BYTES \
  (RENDERER_2D_APP_RESOURCES_CHECKER_SIZE_PIXELS *   \
   RENDERER_2D_APP_RESOURCES_CHECKER_SIZE_PIXELS *   \
   RENDERER_2D_APP_RESOURCES_CHECKER_CHANNELS)

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Loads an image file into a texture, flipped so that the image's first row
//  is at the bottom, as OpenGL expects. A file that can't be loaded is
//  reported on stderr, and Jotunn's placeholder texture made instead
//
//  in:     in_file_path
//  out:    out_texture
static void renderer_2d_app_resources_load_image(
  struct texture_2d_t* out_texture,
  const char* in_file_path);

// Makes a texture from pixels generated in code: fills a checkerboard's
//  pixels, then uploads them to a new texture
//
//  out:    out_texture
static void renderer_2d_app_resources_create_checker(
  struct texture_2d_t* out_texture);

// Fills a checkerboard's RGBA pixels, row by row from the bottom
//
//  out:    out_pixels
static inline void renderer_2d_app_resources_fill_checker_pixels(
  unsigned char out_pixels[RENDERER_2D_APP_RESOURCES_CHECKER_SIZE_BYTES]);

/*******************************************************************************
 *
 *   Function: renderer_2d_app_resources_load()
 *
 ******************************************************************************/
void renderer_2d_app_resources_load(
  struct renderer_2d_app_resources_t* out_resources)
{
  renderer_2d_app_resources_load_image(
    &(out_resources->image_texture),
    RENDERER_2D_APP_RESOURCES_IMAGE_PATH);

  renderer_2d_app_resources_create_checker(&(out_resources->checker_texture));

  // Renders the printable ASCII characters of the typeface Jotunn ships with
  //  into a glyph atlas, itself a texture. The font library was started by
  //  application_init()
  font_typeface_init(
    &(out_resources->typeface),
    FONT_DEFAULT_TYPEFACE_PATH,
    RENDERER_2D_APP_RESOURCES_TYPEFACE_SIZE_PIXELS);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_resources_release()
 *
 ******************************************************************************/
void renderer_2d_app_resources_release(
  struct renderer_2d_app_resources_t* inout_resources)
{
  // The typeface is harmless to release when it wasn't loaded
  font_typeface_cleanup(&(inout_resources->typeface));
  texture_2d_cleanup(&(inout_resources->checker_texture));
  texture_2d_cleanup(&(inout_resources->image_texture));
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_resources_load_image()
 *
 ******************************************************************************/
static void renderer_2d_app_resources_load_image(
  struct texture_2d_t* out_texture,
  const char* in_file_path)
{
  const unsigned int should_flip_vertically = 1;

  texture_2d_create_from_file_path(
    out_texture,
    in_file_path,
    should_flip_vertically);

  // Something to draw all the same, which makes the failure plain to see
  if (!out_texture->is_loaded) {
    fprintf(
      stderr,
      "Couldn't load '%s'; drawing a placeholder instead\n",
      in_file_path);

    texture_2d_create_placeholder(out_texture);
  }
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_resources_create_checker()
 *
 ******************************************************************************/
static void renderer_2d_app_resources_create_checker(
  struct texture_2d_t* out_texture)
{
  const bool should_wrap = true;

  static unsigned char pixels[RENDERER_2D_APP_RESOURCES_CHECKER_SIZE_BYTES];

  renderer_2d_app_resources_fill_checker_pixels(pixels);

  texture_2d_init(
    out_texture,
    RENDERER_2D_APP_RESOURCES_CHECKER_SIZE_PIXELS,
    RENDERER_2D_APP_RESOURCES_CHECKER_SIZE_PIXELS,
    TEXTURE_2D_INTERNAL_FORMAT_RGBA,
    TEXTURE_2D_DATA_FORMAT_RGBA,
    should_wrap);

  // The texture keeps no copy, so the pixels needn't outlive the upload
  texture_2d_set_data(
    out_texture,
    pixels,
    sizeof(pixels));
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_resources_fill_checker_pixels()
 *
 ******************************************************************************/
static inline void renderer_2d_app_resources_fill_checker_pixels(
  unsigned char out_pixels[RENDERER_2D_APP_RESOURCES_CHECKER_SIZE_BYTES])
{
  const unsigned int size_pixels = RENDERER_2D_APP_RESOURCES_CHECKER_SIZE_PIXELS;
  const unsigned int cell_pixels = RENDERER_2D_APP_RESOURCES_CHECKER_CELL_PIXELS;
  const unsigned int channels = RENDERER_2D_APP_RESOURCES_CHECKER_CHANNELS;

  const unsigned char light_color[RENDERER_2D_APP_RESOURCES_CHECKER_CHANNELS] =
    {0xE8, 0xE8, 0xE8, 0xFF};
  const unsigned char dark_color[RENDERER_2D_APP_RESOURCES_CHECKER_CHANNELS] =
    {0x30, 0x60, 0xA0, 0xFF};

  for (unsigned int y = 0; y < size_pixels; y++) {
    for (unsigned int x = 0; x < size_pixels; x++) {
      const bool is_light = ((((x / cell_pixels) + (y / cell_pixels)) % 2) == 0);
      const unsigned char* color = is_light ? light_color : dark_color;

      unsigned char* pixel = &(out_pixels[((y * size_pixels) + x) * channels]);

      for (unsigned int channel = 0; channel < channels; channel++) {
        pixel[channel] = color[channel];
      }
    }
  }
}
