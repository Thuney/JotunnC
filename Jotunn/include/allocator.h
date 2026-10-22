#pragma once

// Jotunn's heap allocations, and those of the libraries it uses, go through
//  this allocator. DEBUG builds count each allocation against a tag naming
//  what it's for, so the engine's memory use can be broken down while it
//  runs; other builds pass straight through to the C library. Memory must be
//  released by the allocator that provided it, so never mix these functions
//  with malloc() and free(). Counting assumes allocations happen on one thread

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 *
 *   TAGS
 *
 ******************************************************************************/

// What an allocation is for
enum allocator_tag_t
{
  ALLOCATOR_TAG_GENERAL = 0x00,
  ALLOCATOR_TAG_APPLICATION,
  ALLOCATOR_TAG_WINDOW,
  ALLOCATOR_TAG_RENDERER,
  ALLOCATOR_TAG_UI,
  ALLOCATOR_TAG_FONT,
  ALLOCATOR_TAG_FILE,
  ALLOCATOR_TAG_DIAGNOSTICS,
  // Made by the libraries Jotunn uses
  ALLOCATOR_TAG_IMGUI,
  ALLOCATOR_TAG_FREETYPE,
  ALLOCATOR_TAG_STB_IMAGE,
  ALLOCATOR_TAG_COUNT
};

// The memory counted against one tag
struct allocator_tag_stats_t
{
  size_t live_bytes;
  size_t peak_live_bytes;
  uint64_t live_count;
  // Every allocation and reallocation since startup; the difference between
  //  two frames is the number made in between
  uint64_t total_count;
};

/*******************************************************************************
 *
 *   ALLOCATION
 *
 ******************************************************************************/

// Allocates uninitialized memory
//
//  in:     in_size_bytes, in_tag
//  ret:    The memory, aligned for any type; NULL when it can't be allocated
void* allocator_allocate(
  const size_t in_size_bytes,
  const enum allocator_tag_t in_tag);

// Allocates zeroed memory for an array
//
//  in:     in_count, in_element_size_bytes, in_tag
//  ret:    The memory, aligned for any type; NULL when it can't be allocated,
//            including when the array's size overflows
void* allocator_allocate_zeroed(
  const size_t in_count,
  const size_t in_element_size_bytes,
  const enum allocator_tag_t in_tag);

// Resizes memory from this allocator, keeping its contents up to the smaller
//  of the two sizes. A NULL inout_memory is allocated afresh, and a size of
//  zero frees inout_memory. The resized memory is counted against in_tag,
//  whatever it was counted against before
//
//  in:     in_size_bytes, in_tag
//  in/out: inout_memory
//  ret:    The resized memory, which may have moved; NULL when it can't be
//            resized, leaving inout_memory as it was, or when in_size_bytes
//            is zero
void* allocator_reallocate(
  void* inout_memory,
  const size_t in_size_bytes,
  const enum allocator_tag_t in_tag);

// Releases memory from this allocator; NULL is ignored
//
//  in/out: inout_memory
void allocator_free(
  void* inout_memory);

// Copies a NUL-terminated string
//
//  in:     in_string, in_tag
//  ret:    The copy; NULL when it can't be allocated
char* allocator_duplicate_string(
  const char* in_string,
  const enum allocator_tag_t in_tag);

/*******************************************************************************
 *
 *   STATISTICS
 *
 ******************************************************************************/

// Reads the memory counted against a tag
//
//  in:     in_tag
//  out:    out_stats
//  ret:    true when allocations are counted in this build and in_tag is
//            valid; otherwise false, with out_stats zeroed
bool allocator_get_tag_stats(
  const enum allocator_tag_t in_tag,
  struct allocator_tag_stats_t* out_stats);

// Names a tag for display
//
//  in:     in_tag
//  ret:    The tag's name, or "unknown" when in_tag isn't valid
const char* allocator_get_tag_name(
  const enum allocator_tag_t in_tag);
