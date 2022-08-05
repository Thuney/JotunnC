#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

struct typeface_t
{
   FT_Face typeface;
};

void font_init();
void font_cleanup();

void typeface_init(struct typeface_t* typeface, const char* ttf_file_path);
void typeface_cleanup(struct typeface_t* typeface);

void typeface_set_char_size(struct typeface_t* typeface);