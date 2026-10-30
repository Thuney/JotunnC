#include "font.h"
#include "log.h"

#include "allocator.h"

#include <ft2build.h>
#include FT_MODULE_H

#include <math.h>
#include <memory.h>

#ifdef DEBUG
   #include <stdio.h>
#endif

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Allocates for FreeType
//
//  in:     in_memory, in_size_bytes
//  ret:    The memory; NULL when it can't be allocated
static void* _font_freetype_allocate(
  FT_Memory in_memory,
  long in_size_bytes);

// Releases memory FreeType allocated
//
//  in:     in_memory
//  in/out: inout_block
static void _font_freetype_free(
  FT_Memory in_memory,
  void* inout_block);

// Resizes memory FreeType allocated
//
//  in:     in_memory, in_current_size_bytes, in_new_size_bytes
//  in/out: inout_block
//  ret:    The resized memory, which may have moved; NULL when it can't be
//            resized, leaving inout_block as it was
static void* _font_freetype_reallocate(
  FT_Memory in_memory,
  long in_current_size_bytes,
  long in_new_size_bytes,
  void* inout_block);

// Expands a one-byte-per-pixel grayscale bitmap to RGBA, copying each gray
//  value into all four channels
//
//  in:     in_bitmap_data, in_bitmap_width, in_bitmap_height
//  ret:    The RGBA pixels, which the caller releases with allocator_free()
static unsigned char* font_convert_grayscale_to_rgba(
  const unsigned char* in_bitmap_data,
  const int in_bitmap_width,
  const int in_bitmap_height);

// Loads a character's glyph into the typeface's glyph slot and renders it to
//  a grayscale bitmap there. Errors are logged in DEBUG builds
//
//  in:     in_typeface, in_charcode
//  ret:    0 on success; otherwise the bitwise OR of the FreeType errors from
//            loading and rendering the glyph
static int font_typeface_render_char(
  const struct font_typeface_t* in_typeface,
  const char in_charcode);

// Copies the glyph just rendered into the atlas bitmap at the pen, fills in
//  its glyph from the typeface's glyph slot, and advances the pen past it
//
//  in:     in_typeface, in_atlas_texture, in_atlas_width, in_atlas_height,
//            in_pen_y, in_char_code, in_bitmap
//  in/out: inout_pen_x
//  out:    out_glyph, out_atlas_bitmap
//  ret:    0, always
static int font_typeface_add_bitmap_to_glyph_atlas(
  const struct font_typeface_t* in_typeface,
  struct texture_2d_t* in_atlas_texture,
  struct font_glyph_t* out_glyph,
  unsigned char* out_atlas_bitmap,
  const int in_atlas_width,
  const int in_atlas_height,
  int* inout_pen_x,
  const int* in_pen_y,
  const char in_char_code,
  const FT_Bitmap* in_bitmap);

// Detaches the glyphs from the glyph atlas and releases the atlas texture
//
//  in/out: inout_typeface
//  ret:    0, always
static int font_typeface_cleanup_glyph_atlas(
  struct font_typeface_t* inout_typeface);

static FT_Library font_library;
static unsigned int is_freetype_initialized = 0;

// FreeType allocates through Jotunn, so its memory is counted too. It keeps
//  this for the library's lifetime, so it can't live on the stack
static struct FT_MemoryRec_ font_freetype_memory =
{
  .user    = NULL,
  .alloc   = &(_font_freetype_allocate),
  .free    = &(_font_freetype_free),
  .realloc = &(_font_freetype_reallocate)
};

/*******************************************************************************
 *
 *   Function: font_init()
 *
 ******************************************************************************/
void font_init(void)
{
  int error;

  if (!is_freetype_initialized)
  {
    // What FT_Init_FreeType() does, but with Jotunn's allocator
    error =
      FT_New_Library(
        &(font_freetype_memory),
        &(font_library));

    if (!error)
    {
      FT_Add_Default_Modules(font_library);
      FT_Set_Default_Properties(font_library);
    }

      #ifdef DEBUG
    if (error)
    {
    }
      #endif

    if (!error)
    {
         #ifdef DEBUG
      log_write(
        LOG_LEVEL_INFO,
        "Loaded FreeType Library");
         #endif

      is_freetype_initialized = 1;
    }
  }
}

/*******************************************************************************
 *
 *   Function: font_cleanup()
 *
 ******************************************************************************/
void font_cleanup(void)
{
  int error;

  if (is_freetype_initialized)
  {
    error = FT_Done_Library(font_library);

  #ifdef DEBUG
    if (error)
    {
    }
  #endif

    if (!error)
    {
    #ifdef DEBUG
      log_write(
        LOG_LEVEL_INFO,
        "Cleaning up FreeType Library");
    #endif

      is_freetype_initialized = 0;
    }
  }
}

/*******************************************************************************
 *
 *   Function: font_convert_grayscale_to_rgba()
 *
 ******************************************************************************/
static unsigned char* font_convert_grayscale_to_rgba(
  const unsigned char* in_bitmap_data,
  const int in_bitmap_width,
  const int in_bitmap_height)
{
  unsigned char* char_rgba_data =
    (unsigned char*)allocator_allocate_zeroed(
      (in_bitmap_height * in_bitmap_width * 4),
      sizeof(unsigned char),
      ALLOCATOR_TAG_FONT);

  unsigned int i, j, bitmap_i, bitmap_j;

  for (j = 0, bitmap_j = 0; bitmap_j < in_bitmap_height; j += 4, bitmap_j++)
  {
    for (i = 0, bitmap_i = 0; bitmap_i < in_bitmap_width; i += 4, bitmap_i++)
    {
      const char char_bitmap_val =
        in_bitmap_data[in_bitmap_width * bitmap_j + bitmap_i];

      char_rgba_data[in_bitmap_width * j + i + 0] = char_bitmap_val;
      char_rgba_data[in_bitmap_width * j + i + 1] = char_bitmap_val;
      char_rgba_data[in_bitmap_width * j + i + 2] = char_bitmap_val;
      char_rgba_data[in_bitmap_width * j + i + 3] = char_bitmap_val;
    }
  }

  return char_rgba_data;
}

/*******************************************************************************
 *
 *   Function: font_typeface_render_char()
 *
 ******************************************************************************/
static int font_typeface_render_char(
  const struct font_typeface_t* in_typeface,
  const char in_charcode)
{
  int error = 0;

  FT_UInt glyph_index = 0;

  glyph_index = FT_Get_Char_Index(in_typeface->typeface, in_charcode);

  error |= FT_Load_Glyph(in_typeface->typeface, glyph_index, FT_LOAD_DEFAULT);

#ifdef DEBUG
  if (error)
  {
    log_write(
      LOG_LEVEL_ERROR,
      "Error loading glyph '%c' - glyph_index = %u",
      in_charcode,
      glyph_index);
  }
#endif

  error |= FT_Render_Glyph(in_typeface->typeface->glyph, FT_RENDER_MODE_NORMAL);

#ifdef DEBUG
  if (error)
  {
    log_write(
      LOG_LEVEL_ERROR,
      "Error rendering glyph for char '%c' - glyph_index = %u",
      in_charcode,
      glyph_index);
  }
#endif

  return error;
}

/*******************************************************************************
 *
 *   Function: font_typeface_add_bitmap_to_glyph_atlas()
 *
 ******************************************************************************/
static int font_typeface_add_bitmap_to_glyph_atlas(
  const struct font_typeface_t* in_typeface,
  struct texture_2d_t* in_atlas_texture,
  struct font_glyph_t* out_glyph,
  unsigned char* out_atlas_bitmap,
  const int in_atlas_width,
  const int in_atlas_height,
  int* inout_pen_x,
  const int* in_pen_y,
  const char in_char_code,
  const FT_Bitmap* in_bitmap)
{
  int error = 0;

  int row, col;
  for (row = 0; row < in_bitmap->rows; ++row)
  {
    for (col = 0; col < in_bitmap->width; ++col)
    {
      const int x = (*inout_pen_x) + col;
      const int y = (*in_pen_y) + row;

      out_atlas_bitmap[(y * in_atlas_width) + x] =
        in_bitmap->buffer[(row * in_bitmap->pitch) + col];
    }
  }

  out_glyph->character_representation = in_char_code;
  out_glyph->width_pixels = in_bitmap->width;
  out_glyph->height_pixels = in_bitmap->rows;
  out_glyph->offset_x_pixels = in_typeface->typeface->glyph->bitmap_left;
  out_glyph->offset_y_pixels =
    (in_bitmap->rows - in_typeface->typeface->glyph->bitmap_top);
  out_glyph->advance_x_pixels = (in_typeface->typeface->glyph->advance.x >> 6);

  // Compute texture coordinates for glyph on atlas (in range [0.0f, 1.0f])
  {
    float x0, y0, x1, y1;

    x0 = ((float)(*inout_pen_x) / (float)in_atlas_width);
    y0 = ((float)(*in_pen_y) / (float)in_atlas_height);

    x1 = ((float)((*inout_pen_x) + in_bitmap->width) / (float)in_atlas_width);
    y1 = ((float)((*in_pen_y) + in_bitmap->rows) / (float)in_atlas_height);

    out_glyph->glyph_texture =
      (struct texture_2d_subtexture_t){.parent_texture = in_atlas_texture,
                               .subtexture_coordinates = {{x0, y0}, {x1, y1}}};
  }

  (*inout_pen_x) += (in_bitmap->width + 1);

  return error;
}

/*******************************************************************************
 *
 *   Function: font_typeface_cleanup_glyph_atlas()
 *
 ******************************************************************************/
static int font_typeface_cleanup_glyph_atlas(
  struct font_typeface_t* inout_typeface)
{
  int error = 0;

  struct font_glyph_t* glyph;

  unsigned int i;
  for (i = 0; i < _FONT_LOADED_GLYPHS_STRING_LEN; i++)
  {
    glyph = &inout_typeface->glyphs[i];

    glyph->glyph_texture.parent_texture = 0;
  }

  if (inout_typeface->glyph_atlas.is_loaded)
  {
    texture_2d_cleanup(&(inout_typeface->glyph_atlas));
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: font_typeface_init()
 *
 ******************************************************************************/
void font_typeface_init(
  struct font_typeface_t* out_typeface,
  const char* in_ttf_file_path,
  const unsigned int in_size_pixels)
{
  int error = 0;

  if (is_freetype_initialized)
  {
    error |= FT_New_Face(font_library, in_ttf_file_path, 0, &out_typeface->typeface);

  #ifdef DEBUG
    if (error == FT_Err_Unknown_File_Format)
    {
      log_write(
        LOG_LEVEL_ERROR,
        "Error loading TypeFace - Unknown File Format");
    }
    else if (error)
    {
      log_write(
        LOG_LEVEL_ERROR,
        "Error loading TypeFace - Generic Error - Code = %u",
        error);
    }
  #endif

    if (!error)
    {
      // #ifdef DEBUG
      //    fprintf(stdout, "Loaded TypeFace from %s\n", ttf_file_path);
      //    fprintf(stdout, "Num Glyphs = %u\n", typeface->typeface->num_glyphs);
      //    fprintf(stdout, "Face Flags = %u\n", typeface->typeface->face_flags);
      //    fprintf(stdout, "Num Fixed Sizes = %u\n", typeface->typeface->num_fixed_sizes);
      // #endif

      font_typeface_set_char_size(out_typeface, in_size_pixels);
      error |= font_typeface_load_glyph_atlas(out_typeface);

      if (!error)
        out_typeface->is_loaded = 1;
    }
  }
}

/*******************************************************************************
 *
 *   Function: font_typeface_cleanup()
 *
 ******************************************************************************/
void font_typeface_cleanup(
  struct font_typeface_t* inout_typeface)
{
  int error = 0;

  if (is_freetype_initialized && inout_typeface->is_loaded)
  {
    error |= font_typeface_cleanup_glyph_atlas(inout_typeface);
    error |= FT_Done_Face(inout_typeface->typeface);

    if (!error)
    {
    #ifdef DEBUG
      log_write(
        LOG_LEVEL_INFO,
        "Cleaning up TypeFace");
    #endif

      inout_typeface->is_loaded = 0;
    }
  }
}

/*******************************************************************************
 *
 *   Function: font_typeface_load_glyph_atlas()
 *
 ******************************************************************************/
int font_typeface_load_glyph_atlas(
  struct font_typeface_t* inout_typeface)
{
  int error = 0;

  // Guess at the dimension of the texture atlas
  const int glyph_height_estimate =
    ((inout_typeface->typeface->size->metrics.height >> 6) + 1);
  const int max_dimension =
    (glyph_height_estimate)*ceilf(sqrtf(_FONT_LOADED_GLYPHS_STRING_LEN));

  int texture_atlas_width = 1;
  while (texture_atlas_width < max_dimension)
    texture_atlas_width <<=
      1; // Starting at 1, multiply width by 2 until we exceed our guess
  int texture_atlas_height = texture_atlas_width;

  struct texture_2d_t* texture_atlas_texture = &(inout_typeface->glyph_atlas);
  struct font_glyph_t* texture_atlas_glyphs = &(inout_typeface->glyphs[0]);

  unsigned char* pixel_data_bitmap =
    (unsigned char*)allocator_allocate_zeroed(
      (texture_atlas_width * texture_atlas_height),
      1,
      ALLOCATOR_TAG_FONT);
  int pen_x = 0, pen_y = 0;

  char cur_char;
  FT_Bitmap* char_bitmap;

  unsigned int i;
  for (i = 0; i < _FONT_LOADED_GLYPHS_STRING_LEN; i++)
  {
    cur_char = _FONT_LOADED_GLYPHS_STRING[i];
    error |=
      font_typeface_render_char(inout_typeface, cur_char); // Load glyph into glyph slot

    char_bitmap = &(inout_typeface->typeface->glyph->bitmap);

    if (pen_x + char_bitmap->width >= texture_atlas_width)
    {
      pen_x = 0;
      pen_y += glyph_height_estimate;
    }

    font_typeface_add_bitmap_to_glyph_atlas(
      inout_typeface,
      texture_atlas_texture,
      &(texture_atlas_glyphs[i]),
      pixel_data_bitmap,
      texture_atlas_width,
      texture_atlas_height,
      &pen_x,
      &pen_y,
      cur_char,
      char_bitmap);
  }

  unsigned char* pixel_data_rgba = font_convert_grayscale_to_rgba(
    pixel_data_bitmap, texture_atlas_width, texture_atlas_height);

  texture_2d_init(
    texture_atlas_texture,
    texture_atlas_width,
    texture_atlas_height,
    TEXTURE_2D_INTERNAL_FORMAT_RGBA,
    TEXTURE_2D_DATA_FORMAT_RGBA,
    false);
  texture_2d_set_data(
    texture_atlas_texture,
    pixel_data_rgba,
    texture_atlas_width * texture_atlas_height * 4);
  resource_set_name(
    &(texture_atlas_texture->resource),
    "glyph atlas");

  allocator_free(pixel_data_rgba);
  allocator_free(pixel_data_bitmap);

  return error;
}

/*******************************************************************************
 *
 *   Function: typeface_get_glyph_from_char()
 *
 ******************************************************************************/
const struct font_glyph_t* typeface_get_glyph_from_char(
  const struct font_typeface_t* in_typeface,
  const char in_charcode)
{
  const struct font_glyph_t* char_glyph;

  if (in_typeface->is_loaded)
  {
    const unsigned int is_char_code_in_loaded_range =
      (in_charcode >= _FONT_LOADED_GLYPHS_FIRST_CHARACTER &&
       in_charcode <= _FONT_LOADED_GLYPHS_LAST_CHARACTER);
    if (is_char_code_in_loaded_range)
    {
      const unsigned int index =
        ((const unsigned int)in_charcode -
         (const unsigned int)_FONT_LOADED_GLYPHS_FIRST_CHARACTER);

      char_glyph = &(in_typeface->glyphs[index]);
    }
  #ifdef DEBUG
    else
    {
      log_write(
        LOG_LEVEL_WARNING,
        "Attempted to load char outside of char range loaded by typeface");
    }
  #endif
  }

  return char_glyph;
}

/*******************************************************************************
 *
 *   Function: font_typeface_set_char_size()
 *
 ******************************************************************************/
void font_typeface_set_char_size(
  const struct font_typeface_t* in_typeface,
  const unsigned int in_char_height_pixels)
{
  int error = FT_Set_Pixel_Sizes(in_typeface->typeface, 0, in_char_height_pixels);
}

/*******************************************************************************
 *
 *   Function: font_typeface_calculate_string_dimensions()
 *
 ******************************************************************************/
union fvector2_t font_typeface_calculate_string_dimensions(
  const struct font_typeface_t* in_typeface,
  const char* in_string,
  const int in_string_length)
{
  static union fvector2_t string_dimensions;
  string_dimensions = (union fvector2_t){0.0f, 0.0f};

  for (int i = 0; i < in_string_length; i++)
  {
    char c = in_string[i];

    const struct font_glyph_t* glyph = typeface_get_glyph_from_char(in_typeface, c);

    string_dimensions.comp.x += (glyph->advance_x_pixels);

    if (glyph->height_pixels > string_dimensions.comp.y)
    {
      string_dimensions.comp.y = glyph->height_pixels;
    }
  }

  return string_dimensions;
}

/*******************************************************************************
 *
 *   Function: _font_freetype_allocate()
 *
 ******************************************************************************/
static void* _font_freetype_allocate(
  FT_Memory in_memory,
  long in_size_bytes)
{
  (void)in_memory;

  if (in_size_bytes < 0)
  {
    return NULL;
  }

  return allocator_allocate(
    (size_t)in_size_bytes,
    ALLOCATOR_TAG_FREETYPE);
}

/*******************************************************************************
 *
 *   Function: _font_freetype_free()
 *
 ******************************************************************************/
static void _font_freetype_free(
  FT_Memory in_memory,
  void* inout_block)
{
  (void)in_memory;

  allocator_free(inout_block);
}

/*******************************************************************************
 *
 *   Function: _font_freetype_reallocate()
 *
 ******************************************************************************/
static void* _font_freetype_reallocate(
  FT_Memory in_memory,
  long in_current_size_bytes,
  long in_new_size_bytes,
  void* inout_block)
{
  (void)in_memory;
  // The allocator knows each block's size already
  (void)in_current_size_bytes;

  if (in_new_size_bytes < 0)
  {
    return NULL;
  }

  return allocator_reallocate(
    inout_block,
    (size_t)in_new_size_bytes,
    ALLOCATOR_TAG_FREETYPE);
}
