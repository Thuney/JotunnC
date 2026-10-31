#pragma once

// Fonts, loaded through FreeType. A typeface renders the printable ASCII
//  characters at one size into a glyph atlas texture, from which strings are
//  drawn a character at a time (see renderer_2d_draw_string())

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string.h>

#include "texture.h"

/*******************************************************************************
 *
 *   LOADED CHARACTERS
 *
 ******************************************************************************/

// The characters each typeface loads, in order; every other character is
//  outside the loaded range
static const char _FONT_LOADED_GLYPHS_STRING[] =
  " !\"#$%&'()*+,-./0123456789:;<=>?@"
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
  "abcdefghijklmnopqrstuvwxyz{|}~";
#define _FONT_LOADED_GLYPHS_STRING_LEN (sizeof(_FONT_LOADED_GLYPHS_STRING) - 1)
#define _FONT_LOADED_GLYPHS_FIRST_CHARACTER (_FONT_LOADED_GLYPHS_STRING[0])
#define _FONT_LOADED_GLYPHS_LAST_CHARACTER \
  (_FONT_LOADED_GLYPHS_STRING[_FONT_LOADED_GLYPHS_STRING_LEN - 1])

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// One character's place in its typeface's glyph atlas, and its metrics in
//  pixels
struct font_glyph_t
{
  char character_representation;
  struct texture_2d_subtexture_t glyph_texture;
  unsigned int width_pixels, height_pixels;
  int offset_x_pixels, offset_y_pixels;
  int advance_x_pixels;
};

// A font face loaded at one size, with its glyphs rendered into an atlas
struct font_typeface_t
{
  FT_Face typeface;
  unsigned int size_pixels;
  unsigned int is_loaded;
  //
  struct texture_2d_t glyph_atlas;
  struct font_glyph_t glyphs[_FONT_LOADED_GLYPHS_STRING_LEN];
};

/*******************************************************************************
 *
 *   LIBRARY
 *
 ******************************************************************************/

// Starts FreeType, allocating through Jotunn's allocator. Does nothing when
//  it's started already. Typefaces can only be loaded once this has succeeded
void font_init(void);

// Shuts FreeType down. Does nothing when it isn't started. Typefaces must be
//  cleaned up first
void font_cleanup(void);

/*******************************************************************************
 *
 *   TYPEFACES
 *
 ******************************************************************************/

// Loads a typeface from a font file and renders its glyph atlas at a pixel
//  height of in_size_pixels. Does nothing unless font_init() has succeeded.
//  is_loaded is set only on success, so out_typeface should start zeroed;
//  on failure the error is logged in DEBUG builds
//
//  in:     in_ttf_file_path, in_size_pixels
//  out:    out_typeface
void font_typeface_init(
  struct font_typeface_t* out_typeface,
  const char* in_ttf_file_path,
  unsigned int in_size_pixels);

// Releases a loaded typeface's glyph atlas and face. Does nothing unless the
//  typeface is loaded and FreeType is started
//
//  in/out: inout_typeface
void font_typeface_cleanup(
  struct font_typeface_t* inout_typeface);

// Renders each loaded character of the typeface's face, at its current size,
//  into a new glyph atlas texture and fills in its glyphs. Called by
//  font_typeface_init(); calling it again leaks the previous atlas
//
//  in/out: inout_typeface
//  ret:    0 on success; otherwise the bitwise OR of the FreeType errors met
//            loading and rendering glyphs, with the atlas made regardless
int font_typeface_load_glyph_atlas(
  struct font_typeface_t* inout_typeface);

// Finds a character's glyph. Logs a warning in DEBUG builds for a character
//  outside the loaded range
//
//  in:     in_typeface, in_charcode
//  ret:    The glyph; NULL when in_typeface isn't loaded or in_charcode is
//            outside the loaded range
const struct font_glyph_t* typeface_get_glyph_from_char(
  const struct font_typeface_t* in_typeface,
  char in_charcode);

// Sets the pixel height the typeface's face renders glyphs at. Glyphs already
//  in the atlas are unchanged
//
//  in:     in_typeface, in_char_height_pixels
void font_typeface_set_char_size(
  const struct font_typeface_t* in_typeface,
  unsigned int in_char_height_pixels);

// Measures a string as drawn in the typeface. Characters without a glyph,
//  outside the loaded range or in a typeface not loaded, add nothing
//
//  in:     in_typeface, in_string, in_string_length
//  ret:    The sum of the characters' advances as x and the tallest glyph's
//            height as y, in pixels
union fvector2_t font_typeface_calculate_string_dimensions(
  const struct font_typeface_t* in_typeface,
  const char* in_string,
  int in_string_length);
