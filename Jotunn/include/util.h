#pragma once

/** System Includes **/
#include <stdlib.h>

/** Macros **/
#define STRINGIFY(x) #x
#define ROOT_PATHIFY(x) STRINGIFY(x)
#define PATH_FROM_JOTUNN_ROOT(x) ROOT_PATHIFY(JOTUNN_ROOT_PATH) x

/** Functions **/
// Reads a whole file into memory
//
//  in:     file_path
//  ret:    The file's contents followed by a terminator, to be released with
//            allocator_free(); NULL when the file can't be read
char* read_raw_file_contents(
  const char* file_path);
