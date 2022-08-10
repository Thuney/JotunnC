#include "font.h"

#include <memory.h>
#include <math.h>

#ifdef DEBUG
   #include <stdio.h>
#endif

static FT_Library font_library;
static unsigned int freetype_initialized = 0;

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
static unsigned char* grayscale_bitmap_data_to_rgba_texture_data(unsigned char* char_bitmap_data, const int bitmap_width, const int bitmap_height)
{
   unsigned char* char_rgba_data = (unsigned char*) calloc(bitmap_height*bitmap_width*4, sizeof(unsigned char));

   unsigned int i, j, bitmap_i, bitmap_j;

   for (j = 0, bitmap_j = 0; bitmap_j < bitmap_height; j += 4, bitmap_j++)
   {
      for (i = 0, bitmap_i = 0; bitmap_i < bitmap_width; i += 4, bitmap_i++)
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

static int typeface_render_char(struct typeface_t* typeface, const char charcode)
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
   #endif

   error |= FT_Render_Glyph(typeface->typeface->glyph, FT_RENDER_MODE_NORMAL);

   #ifdef DEBUG
      if (error)
      {
         fprintf(stdout, "Error rendering glyph for char '%c' - glyph_index = %u\n", charcode, glyph_index);
      }
   #endif

   return error;
}

static int typeface_load_glyph_atlas(struct typeface_t* typeface)
{
   int error = 0;

   // Guess at the dimension of the texture atlas
   const int glyph_height_estimate = ((typeface->typeface->size->metrics.height >> 6) + 1);
   const int max_dimension = (glyph_height_estimate) * ceilf(sqrtf(_FONT_LOADED_GLYPHS_STRING_LEN));

   int texture_atlas_width = 1;
   while (texture_atlas_width < max_dimension) texture_atlas_width <<= 1; // Starting at 1, multiply width by 2 until we exceed our guess
   int texture_atlas_height = texture_atlas_width;

   struct texture_2d_t* texture_atlas_texture = &(typeface->glyph_atlas);
   struct glyph_t*      texture_atlas_glyphs  = &(typeface->glyphs[0]);

   unsigned char* pixel_data_bitmap = (unsigned char*)calloc(texture_atlas_width*texture_atlas_height, 1);
   int pen_x = 0, pen_y = 0;

   char cur_char;
   FT_Bitmap* char_bitmap;

   unsigned int i;
   for (i = 0; i < _FONT_LOADED_GLYPHS_STRING_LEN; i++)
   {
      cur_char = _FONT_LOADED_GLYPHS_STRING[i];
      error   |= typeface_render_char(typeface, cur_char); // Load glyph into glyph slot

      char_bitmap = &(typeface->typeface->glyph->bitmap);

      if (pen_x + char_bitmap->width >= texture_atlas_width)
      {
         pen_x = 0;
         pen_y += glyph_height_estimate;
      }

      int row, col;
      for (row = 0; row < char_bitmap->rows; ++row)
      {
         for (col = 0; col < char_bitmap->width; ++col)
         {
            int x = pen_x + col;
            int y = pen_y + row;

            pixel_data_bitmap[(y*texture_atlas_width) + x] = char_bitmap->buffer[(row*char_bitmap->pitch) + col];
         }
      }

      texture_atlas_glyphs[i].character_representation = cur_char;
      texture_atlas_glyphs[i].width                    = char_bitmap->width;
      texture_atlas_glyphs[i].height                   = char_bitmap->rows;
      texture_atlas_glyphs[i].offset_x                 = typeface->typeface->glyph->bitmap_left;
      texture_atlas_glyphs[i].offset_y                 = (char_bitmap->rows - typeface->typeface->glyph->bitmap_top);
      texture_atlas_glyphs[i].advance_x                = (typeface->typeface->glyph->advance.x >> 6);

      float x0, y0, x1, y1;

      x0 = ((float)pen_x / (float)texture_atlas_width);
      y0 = ((float)pen_y / (float)texture_atlas_height);

      x1 = ((float)(pen_x + char_bitmap->width) / (float)texture_atlas_width);
      y1 = ((float)(pen_y + char_bitmap->rows) / (float)texture_atlas_height);

      texture_atlas_glyphs[i].glyph_texture = (struct subtexture_2d_t) 
         {
            .parent_texture = texture_atlas_texture,
            .subtexture_coordinates = 
               { 
                  { x0, y0 }, 
                  { x1, y1 } 
               }
         };

      pen_x += (char_bitmap->width + 1);
   }

   unsigned char* pixel_data_rgba = grayscale_bitmap_data_to_rgba_texture_data(pixel_data_bitmap, texture_atlas_width, texture_atlas_height);

   texture_2d_init(texture_atlas_texture, texture_atlas_width, texture_atlas_height, TEXTURE_2D_INTERNAL_FORMAT_RGBA8);
   texture_2d_set_data(texture_atlas_texture, pixel_data_rgba, (texture_atlas_width*texture_atlas_height*4), TEXTURE_2D_FORMAT_RGBA);

   free(pixel_data_bitmap);

   return error;
}

static int typeface_cleanup_glyph_atlas(struct typeface_t* typeface)
{
   int error = 0;

   struct glyph_t* glyph;

   unsigned int i;
   for (i = 0; i < _FONT_LOADED_GLYPHS_STRING_LEN; i++)
   {
      glyph = &typeface->glyphs[i];

      glyph->glyph_texture.parent_texture = 0;
   }

   texture_2d_cleanup(&typeface->glyph_atlas);

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
         // #ifdef DEBUG
         //    fprintf(stdout, "Loaded TypeFace from %s\n", ttf_file_path);
         //    fprintf(stdout, "Num Glyphs = %u\n", typeface->typeface->num_glyphs);
         //    fprintf(stdout, "Face Flags = %u\n", typeface->typeface->face_flags);
         //    fprintf(stdout, "Num Fixed Sizes = %u\n", typeface->typeface->num_fixed_sizes);
         // #endif

         typeface_set_char_size(typeface, size_points);

         error = typeface_load_glyph_atlas(typeface);
         if (!error) typeface->is_loaded = 1;
      }
   }
}

void typeface_cleanup(struct typeface_t* typeface)
{
   int error = 0;

   if (freetype_initialized && typeface->is_loaded)
   {
      error |= typeface_cleanup_glyph_atlas(typeface);
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

const struct glyph_t* typeface_get_glyph_from_char(const struct typeface_t* typeface, const char charcode)
{
   const struct glyph_t* char_glyph;

   if (typeface->is_loaded)
   {
      const unsigned int is_char_code_in_loaded_range = (charcode >= _FONT_LOADED_GLYPHS_FIRST_CHARACTER && charcode <= _FONT_LOADED_GLYPHS_LAST_CHARACTER);
      if (is_char_code_in_loaded_range)
      {
         const unsigned int index = ((const unsigned int)charcode - (const unsigned int)_FONT_LOADED_GLYPHS_FIRST_CHARACTER);

         char_glyph = &(typeface->glyphs[index]);
      }
      #ifdef DEBUG
         else
         {
            fprintf(stdout, "Attempted to load char outside of char range loaded by typeface\n");
         }
      #endif
   }

   return char_glyph;
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