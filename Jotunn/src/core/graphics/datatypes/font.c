#include "font.h"

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

void typeface_init(struct typeface_t* typeface, const char* ttf_file_path)
{
   int error;

   if (freetype_initialized)
   {
      error = FT_New_Face(font_library, ttf_file_path, 0, &typeface->typeface);

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
      }
   }

}

void typeface_cleanup(struct typeface_t* typeface)
{
   int error;

   if (freetype_initialized)
   {
      error = FT_Done_Face(typeface->typeface);

      if (!error)
      {
         #ifdef DEBUG
            fprintf(stdout, "Cleaning up TypeFace\n");
         #endif
      }
   }
}