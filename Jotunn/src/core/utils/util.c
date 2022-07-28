#include "util.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

char* read_raw_file_contents(const char* file_path)
{
   FILE* file = fopen(file_path, "r");

   long file_size_bytes;
   char* buffer;

   // Find end of file
   fseek(file, 0L, SEEK_END);
   file_size_bytes = ftell(file);
   rewind(file);

   buffer = (char*)calloc(1, file_size_bytes + 1);
   if (!buffer)
   {
      fclose(file);
      #ifdef DEBUG
         fprintf(stdout, "Memory alloc fails\n");
      #endif
   }

   if(1 != fread(buffer, file_size_bytes, 1, file))
   {
      fclose(file);
      free(buffer);
      #ifdef DEBUG
         fprintf(stdout, "Entire read fails\n");
      #endif
   }
   else
   {
      fclose(file);
   }

   #ifdef DEBUG
      // fprintf(stdout, "Shader File Contents:\n%s", buffer);
   #endif
   return buffer;
}