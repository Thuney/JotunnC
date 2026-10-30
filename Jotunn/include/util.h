#pragma once

// Odds and ends shared across Jotunn: paths into its source tree, and
//  reading files

#include <stdlib.h>

/*******************************************************************************
 *
 *   PATHS
 *
 ******************************************************************************/

// PATH_FROM_JOTUNN_ROOT() turns a string literal path relative to Jotunn's
//  root directory into an absolute one, also a string literal. The build sets
//  JOTUNN_ROOT_PATH
#define STRINGIFY(x) #x
#define ROOT_PATHIFY(x) STRINGIFY(x)
#define PATH_FROM_JOTUNN_ROOT(x) ROOT_PATHIFY(JOTUNN_ROOT_PATH) x

/*******************************************************************************
 *
 *   FILES
 *
 ******************************************************************************/

// Reads a whole file into memory
//
//  in:     in_file_path
//  ret:    The file's contents followed by a terminator, to be released with
//            allocator_free(); NULL when the file can't be read
char* util_read_raw_file_contents(
  const char* in_file_path);
