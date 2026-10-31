#pragma once

// Jotunn's heap allocations, and those of the libraries it uses, go through
//  this allocator. Each allocation carries a tag naming what it's for. DEBUG
//  builds count each allocation against its tag, so the engine's memory use
//  can be broken down while it runs.
//
// Where a tag's memory comes from is its policy: the C library's heap, which
//  every tag uses until told otherwise, a pool carved from memory the
//  application provides, or nowhere at all. Memory must be released by the
//  allocator that provided it, so never mix these functions with malloc() and
//  free(). The allocator assumes allocations happen on one thread

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

// Where a tag's memory comes from
enum allocator_policy_t
{
  // The C library's heap; every tag's policy until it's set otherwise
  ALLOCATOR_POLICY_HEAP = 0x00,
  // A pool set up with allocator_pool_init()
  ALLOCATOR_POLICY_POOL,
  // Nowhere: allocating fails, returning NULL
  ALLOCATOR_POLICY_FORBIDDEN,
  ALLOCATOR_POLICY_COUNT
};

struct allocator_pool_block_t;

// A region of memory that tags can allocate from instead of the heap. Blocks
//  are allocated first fit, and merged with their free neighbours when
//  freed. Set up with allocator_pool_init(); its fields are the allocator's
//  to change
struct allocator_pool_t
{
  // The part of the provided memory that blocks are carved from, aligned
  //  for any type
  unsigned char* memory;
  size_t size_bytes;
  // The free blocks, in address order
  struct allocator_pool_block_t* first_free_block;
  // Bytes in blocks currently allocated, their headers included, and the
  //  most there have been at once
  size_t used_bytes;
  size_t peak_used_bytes;
};

/*******************************************************************************
 *
 *   ALLOCATION
 *
 ******************************************************************************/

// Allocates uninitialized memory, from wherever in_tag's policy says
//
//  in:     in_size_bytes, in_tag
//  ret:    The memory, aligned for any type; NULL when it can't be allocated,
//            including when in_tag's policy is ALLOCATOR_POLICY_FORBIDDEN
void* allocator_allocate(
  size_t in_size_bytes,
  enum allocator_tag_t in_tag);

// Allocates zeroed memory for an array
//
//  in:     in_count, in_element_size_bytes, in_tag
//  ret:    The memory, aligned for any type; NULL when it can't be allocated,
//            including when the array's size overflows
void* allocator_allocate_zeroed(
  size_t in_count,
  size_t in_element_size_bytes,
  enum allocator_tag_t in_tag);

// Resizes memory from this allocator, keeping its contents up to the smaller
//  of the two sizes. A NULL inout_memory is allocated afresh, and a size of
//  zero frees inout_memory. The resized memory is counted against in_tag,
//  whatever it was counted against before, but stays where it came from, so
//  in_tag's policy must still name that place
//
//  in:     in_size_bytes, in_tag
//  in/out: inout_memory
//  ret:    The resized memory, which may have moved; NULL when it can't be
//            resized, including when in_tag's policy no longer names where
//            inout_memory came from, leaving inout_memory as it was, or when
//            in_size_bytes is zero
void* allocator_reallocate(
  void* inout_memory,
  size_t in_size_bytes,
  enum allocator_tag_t in_tag);

// Releases memory from this allocator, back to wherever it came from,
//  whatever its tag's policy is now; NULL is ignored
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
  enum allocator_tag_t in_tag);

/*******************************************************************************
 *
 *   POOLS AND POLICIES
 *
 ******************************************************************************/

// Sets up a pool in inout_memory, which must outlive every tag allocating
//  from it, and every block allocated from it. Some of in_size_bytes is lost
//  to aligning the pool, and each block costs a header of a few bytes
//
//  in:     in_size_bytes
//  in/out: inout_memory
//  out:    out_pool
//  ret:    true on success; false when inout_memory is NULL or too small to
//            hold a single block, with out_pool left empty
bool allocator_pool_init(
  struct allocator_pool_t* out_pool,
  void* inout_memory,
  size_t in_size_bytes);

// Sets where a tag's memory comes from. Set it before anything is allocated
//  with the tag: memory already allocated is still released where it came
//  from, but can't be resized under the new policy. A pool can serve any
//  number of tags, and while it serves any, its memory must not be released
//
//  in:     in_tag, in_policy
//  in/out: inout_pool
//  ret:    true on success; false when in_tag or in_policy isn't valid, or
//            in_policy is ALLOCATOR_POLICY_POOL and inout_pool is NULL, with
//            the tag's policy left as it was
bool allocator_set_tag_policy(
  enum allocator_tag_t in_tag,
  enum allocator_policy_t in_policy,
  struct allocator_pool_t* inout_pool);

// Reads where a tag's memory comes from
//
//  in:     in_tag
//  ret:    The tag's policy; ALLOCATOR_POLICY_FORBIDDEN when in_tag isn't
//            valid
enum allocator_policy_t allocator_get_tag_policy(
  enum allocator_tag_t in_tag);

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
  enum allocator_tag_t in_tag,
  struct allocator_tag_stats_t* out_stats);

// Names a tag for display
//
//  in:     in_tag
//  ret:    The tag's name, or "unknown" when in_tag isn't valid
const char* allocator_get_tag_name(
  enum allocator_tag_t in_tag);
