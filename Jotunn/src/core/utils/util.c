#include "util.h"

#include "allocator.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

char* read_raw_file_contents(
  const char* file_path)
{
  FILE* file = fopen(file_path, "r");
  if (file == NULL)
  {
  #ifdef DEBUG
    fprintf(
      stdout,
      "Failed to open %s\n",
      file_path);
  #endif

    return NULL;
  }

  // Find end of file
  fseek(file, 0L, SEEK_END);
  const long file_size_bytes = ftell(file);
  rewind(file);

  char* buffer = NULL;
  if (file_size_bytes >= 0)
  {
    // Zeroed, so the contents are followed by a terminator
    buffer =
      (char*)allocator_allocate_zeroed(
        1,
        ((size_t)file_size_bytes + 1),
        ALLOCATOR_TAG_FILE);
  }

  if (buffer == NULL)
  {
  #ifdef DEBUG
    fprintf(stdout, "Memory alloc fails\n");
  #endif
  }
  else
  {
    const size_t read_size_bytes =
      fread(
        buffer,
        1,
        (size_t)file_size_bytes,
        file);

    if (read_size_bytes != (size_t)file_size_bytes)
    {
    #ifdef DEBUG
      fprintf(stdout, "Entire read fails\n");
    #endif

      allocator_free(buffer);
      buffer = NULL;
    }
  }

  fclose(file);

  return buffer;
}
