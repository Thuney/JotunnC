#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string.h>

#include "texture.h"

static const char _FONT_LOADED_GLYPHS_STRING[] = "!\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^`abcdefghijklmnopqrstuvwxyz{|}~"; 
#define _FONT_LOADED_GLYPHS_STRING_LEN (sizeof(_FONT_LOADED_GLYPHS_STRING)-1)
#define _FONT_LOADED_GLYPHS_FIRST_CHARACTER (_FONT_LOADED_GLYPHS_STRING[0])
#define _FONT_LOADED_GLYPHS_LAST_CHARACTER (_FONT_LOADED_GLYPHS_STRING[_FONT_LOADED_GLYPHS_STRING_LEN-1])

struct typeface_t
{
   FT_Face typeface;
   unsigned int size_points;
   unsigned int is_loaded;
   struct texture_2d_t glyph_textures[_FONT_LOADED_GLYPHS_STRING_LEN];
};

void font_init();
void font_cleanup();

void typeface_init(struct typeface_t* typeface, const char* ttf_file_path, const unsigned int size_points);
void typeface_cleanup(struct typeface_t* typeface);

const struct texture_2d_t* typeface_get_texture_from_char(const struct typeface_t* typeface, const char charcode);
void typeface_set_char_size(const struct typeface_t* typeface, const unsigned int char_height_points);