#include "font.h"

#include <memory.h>

#ifdef DEBUG
   #include <stdio.h>
#endif

static FT_Library font_library;
static unsigned int freetype_initialized = 0;

// static const char _ttf_file_path[64] = "/usr/share/fonts/TTF/Hack-Regular.ttf";

void font_init()
{
   int error;

   if (!freetype_initialized)
   {
      error = FT_Init_FreeType(&font_library);
      
      #ifdef DEBUG
         if (error)
         {
            
         }
      #endif
       
      if (!error)
      {
         #ifdef DEBUG
            fprintf(stdout, "Loaded FreeType Library\n");
         #endif

         freetype_initialized = 1;
      }
   }
}

void font_cleanup()
{
   int error;

   if (freetype_initialized)
   {
      error = FT_Done_FreeType(font_library);
      
      #ifdef DEBUG
         if (error)
         {
            
         }
      #endif
       
      if (!error)
      {
         #ifdef DEBUG
            fprintf(stdout, "Cleaning up FreeType Library\n");
         #endif

         freetype_initialized = 0;
      }
   }
}

//
static unsigned char* typeface_load_glyph_data_for_char(const struct typeface_t* typeface, const char charcode, int* ret_bitmap_width, int* ret_bitmap_height)
{
   int error = 0;

   FT_UInt glyph_index = 0;

   glyph_index = FT_Get_Char_Index(typeface->typeface, charcode);

   error |= FT_Load_Glyph(typeface->typeface, glyph_index, FT_LOAD_DEFAULT);

   #ifdef DEBUG
      if (error)
      {
      fprintf(stdout, "Error loading glyph '%c' - glyph_index = %u\n", charcode, glyph_index);
      }
      else
      {
      fprintf(stdout, "Loaded glyph for char '%c' - glyph_index = %u - glyph\n", charcode, glyph_index);
      }
   #endif

   error |= FT_Render_Glyph(typeface->typeface->glyph, FT_RENDER_MODE_NORMAL);

   #ifdef DEBUG
      if (error)
      {
      fprintf(stdout, "Error rendering glyph for char '%c' - glyph_index = %u\n", charcode, glyph_index);
      }
      else
      {
      fprintf(stdout, "Rendered glyph for char '%c' - glyph_index = %u\n", charcode, glyph_index);
      }
   #endif

   unsigned char* bitmap_vals;

   unsigned int bitmap_width, bitmap_height;

   if (!error)
   {
      unsigned int bitmap_left, bitmap_top;

      bitmap_width  = typeface->typeface->glyph->bitmap.width;
      bitmap_height = typeface->typeface->glyph->bitmap.rows;

      bitmap_left = typeface->typeface->glyph->bitmap_left;
      bitmap_top  = typeface->typeface->glyph->bitmap_top;

      // #ifdef DEBUG
      //    fprintf(stdout, "Bitmap width = %u, Bitmap height = %u\n", bitmap_width, bitmap_height);
      // #endif

      bitmap_vals = (unsigned char*) calloc(bitmap_height*bitmap_width, sizeof(unsigned char));

      FT_Bitmap* bitmap = &(typeface->typeface->glyph->bitmap);

      FT_Int i, j, out_j;
      for ( i = 0; i < bitmap_width; i++)
      {
         for ( j = (bitmap_height - 1), out_j = 0; j >= 0; j--, out_j++)
         {
            unsigned char bitmap_buffer_val = bitmap->buffer[bitmap_width*j + i];

            // #ifdef DEBUG
            //    fprintf(stdout, "%3u ", bitmap_buffer_val);
            // #endif

            bitmap_vals[bitmap_width*out_j + i] = bitmap_buffer_val;
         }
         // #ifdef DEBUG
         //    fprintf(stdout, "\n");
         // #endif
      }
   }

   *ret_bitmap_width  = bitmap_width;
   *ret_bitmap_height = bitmap_height;

   return bitmap_vals;
}

//
static unsigned char* grayscale_bitmap_data_to_rgba_texture_data(unsigned char* char_bitmap_data, const int bitmap_width, const int bitmap_height)
{
   unsigned char* char_rgba_data = (unsigned char*) calloc(bitmap_height*bitmap_width*4, sizeof(unsigned char));

   unsigned int i, j, bitmap_i, bitmap_j;

   for (i = 0, bitmap_i = 0; bitmap_i < bitmap_width; i += 4, bitmap_i++)
   {
      for (j = 0, bitmap_j = 0; bitmap_j < bitmap_height; j += 4, bitmap_j++)
      {
         const char char_bitmap_val = char_bitmap_data[bitmap_width*bitmap_j + bitmap_i];

         char_rgba_data[bitmap_width*j + i + 0] = char_bitmap_val;
         char_rgba_data[bitmap_width*j + i + 1] = char_bitmap_val;
         char_rgba_data[bitmap_width*j + i + 2] = char_bitmap_val;
         char_rgba_data[bitmap_width*j + i + 3] = char_bitmap_val;
      }
   }

   return char_rgba_data;
}

//
static struct texture_2d_t typeface_get_texture_2d_from_char_index(struct typeface_t* typeface, const char charcode)
{
   struct texture_2d_t char_texture;
   int error = 0;

   int bitmap_width, bitmap_height;

   unsigned char* char_bitmap_data = typeface_load_glyph_data_for_char(typeface, charcode, &bitmap_width, &bitmap_height);

   #ifdef DEBUG
      fprintf(stdout, "Loaded Bitmap for Char '%c'- Width = %d, Height = %d\n", charcode, bitmap_width, bitmap_height);
   #endif

   // unsigned int i, j;
   
   // for (i = 0; i < bitmap_width; i++)
   // {
   //    for (j = 0; j < bitmap_height; j++)
   //    {
   //       unsigned char bitmap_buffer_val = char_bitmap_data[bitmap_width*j + i];
   //       #ifdef DEBUG
   //          fprintf(stdout, "%3u ", bitmap_buffer_val);
   //       #endif
   //    }
   //    #ifdef DEBUG
   //       fprintf(stdout, "\n");
   //    #endif
   // }

   unsigned char* char_rgba_data = grayscale_bitmap_data_to_rgba_texture_data(char_bitmap_data, bitmap_width, bitmap_height);

   // unsigned int i, j;
   // for (i = 0; i < bitmap_width*4; i++)
   // {
   //    for (j = 0; j < bitmap_height; j++)
   //    {
   //       unsigned char rgba_buffer_val = char_rgba_data[bitmap_width*j + i];
   //       #ifdef DEBUG
   //          fprintf(stdout, "%3u ", rgba_buffer_val);
   //       #endif
   //    }
   //    #ifdef DEBUG
   //       fprintf(stdout, "\n");
   //    #endif
   // }

   free(char_bitmap_data);

   texture_2d_init(&char_texture, bitmap_width, bitmap_height, TEXTURE_2D_INTERNAL_FORMAT_RGBA8);
   texture_2d_set_data(&char_texture, char_rgba_data, (bitmap_width*bitmap_height*4), TEXTURE_2D_FORMAT_RGBA);

   return char_texture;
}

//
static int typeface_load_character_textures(struct typeface_t* typeface)
{
   int error = 0;

   unsigned int i;
   for (i = 0; i < _FONT_LOADED_GLYPHS_STRING_LEN; i++)
   {
      typeface->glyph_textures[i] = typeface_get_texture_2d_from_char_index(typeface, _FONT_LOADED_GLYPHS_STRING[i]);
   }

   return error;
}

//
static int typeface_cleanup_character_textures(struct typeface_t* typeface)
{
   int error = 0;

   struct texture_2d_t* texture;

   unsigned int i;
   for (i = 0; i < _FONT_LOADED_GLYPHS_STRING_LEN; i++)
   {
      texture = &typeface->glyph_textures[i];

      if (texture->is_loaded) texture_2d_cleanup(texture);
   }

   return error;
}

//

void typeface_init(struct typeface_t* typeface, const char* ttf_file_path, const unsigned int size_points)
{
   int error = 0;

   if (freetype_initialized)
   {
      error |= FT_New_Face(font_library, ttf_file_path, 0, &typeface->typeface);

      #ifdef DEBUG
         if (error == FT_Err_Unknown_File_Format)
         {
            fprintf(stdout, "Error loading TypeFace - Unknown File Format\n");
         }
         else if (error)
         {
            fprintf(stdout, "Error loading TypeFace - Generic Error - Code = %u\n", error);
         }
      #endif

      if (!error)
      {
         #ifdef DEBUG
            fprintf(stdout, "Loaded TypeFace from %s\n", ttf_file_path);
            fprintf(stdout, "Num Glyphs = %u\n", typeface->typeface->num_glyphs);
            fprintf(stdout, "Face Flags = %u\n", typeface->typeface->face_flags);
            fprintf(stdout, "Num Fixed Sizes = %u\n", typeface->typeface->num_fixed_sizes);
         #endif

         typeface_set_char_size(typeface, size_points);

         error = typeface_load_character_textures(typeface);

         if (!error) typeface->is_loaded = 1;
      }
   }
}

void typeface_cleanup(struct typeface_t* typeface)
{
   int error = 0;

   if (freetype_initialized && typeface->is_loaded)
   {
      error |= typeface_cleanup_character_textures(typeface);
      error |= FT_Done_Face(typeface->typeface);

      if (!error)
      {
         #ifdef DEBUG
            fprintf(stdout, "Cleaning up TypeFace\n");
         #endif

         typeface->is_loaded = 0;
      }
   }
}

const struct texture_2d_t* typeface_get_texture_from_char(const struct typeface_t* typeface, const char charcode)
{
   const struct texture_2d_t* char_texture;

   if (typeface->is_loaded)
   {
      const unsigned int is_char_code_in_loaded_range = (charcode >= _FONT_LOADED_GLYPHS_FIRST_CHARACTER && charcode <= _FONT_LOADED_GLYPHS_LAST_CHARACTER);
      if (is_char_code_in_loaded_range)
      {
         const unsigned int index = ((const unsigned int)charcode - (const unsigned int)_FONT_LOADED_GLYPHS_FIRST_CHARACTER);

         // #ifdef DEBUG
         //     fprintf(stdout, "Requested character '%c' in range - returning loaded texture from index = %u\n", charcode, index);
         // #endif

         char_texture = &(typeface->glyph_textures[index]);
      }
      #ifdef DEBUG
         else
         {
            fprintf(stdout, "Attempted to load char outside of char range loaded by typeface\n");
         }
      #endif
   }

   return char_texture;
}

void typeface_set_char_size(const struct typeface_t* typeface, const unsigned int char_height_points)
{
   const unsigned int width  = 1200;
   const unsigned int height = 1000;

   /* 0 - handle to face object           */
   /* 1 - char_width in 1/64th of points  */
   /* 2 - char_height in 1/64th of points */
   /* 3 - horizontal device resolution    */
   /* 4 - vertical device resolution      */

   int error = FT_Set_Char_Size(typeface->typeface, 0, (char_height_points*64U), width, height);
}