#pragma once

// Odds and ends shared across Jotunn: paths into its source tree, getting
//  from an embedded struct to the struct embedding it, and reading files

#include <stddef.h>
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
 *   EMBEDDED STRUCTS
 *
 ******************************************************************************/

// container_of() turns ptr, a pointer to the member of a struct of type type,
//  into a pointer to that struct, wherever in it the member sits. ptr is
//  checked against the member's type. __extension__ keeps -pedantic quiet
//  about the GNU statement expression
#define container_of(ptr, type, member) __extension__ ({  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);  \
    (type *)( (char *)__mptr - offsetof(type,member) );})

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
