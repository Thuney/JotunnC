#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string.h>

#include "texture.h"

static const char _FONT_LOADED_GLYPHS_STRING[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"; 
#define _FONT_LOADED_GLYPHS_STRING_LEN (sizeof(_FONT_LOADED_GLYPHS_STRING)-1)
#define _FONT_LOADED_GLYPHS_FIRST_CHARACTER (_FONT_LOADED_GLYPHS_STRING[0])
#define _FONT_LOADED_GLYPHS_LAST_CHARACTER (_FONT_LOADED_GLYPHS_STRING[_FONT_LOADED_GLYPHS_STRING_LEN-1])

struct glyph_t
{
   char character_representation;
   struct subtexture_2d_t glyph_texture;
   unsigned int width, height;
   int offset_x, offset_y;
   int advance_x;
};

struct typeface_t
{
   FT_Face typeface;
   unsigned int size_points;
   unsigned int is_loaded;
   //
   struct texture_2d_t glyph_atlas;
   struct glyph_t glyphs[_FONT_LOADED_GLYPHS_STRING_LEN];
};

void font_init();
void font_cleanup();

void typeface_init(struct typeface_t* typeface, const char* ttf_file_path, const unsigned int size_points);
void typeface_cleanup(struct typeface_t* typeface);

int typeface_load_glyph_atlas(struct typeface_t* typeface);
const struct glyph_t* typeface_get_glyph_from_char(const struct typeface_t* typeface, const char charcode);
void typeface_set_char_size(const struct typeface_t* typeface, const unsigned int char_height_points);

fvector2 typeface_calculate_string_dimensions(struct typeface_t* typeface, const char* string, int string_length);