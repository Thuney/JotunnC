#include "allocator.h"

#include <assert.h>
#include <stdalign.h>
#include <stdint.h>
#ifndef JOTUNN_NO_HEAP
  #include <stdlib.h>
#endif
#include <string.h>

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

#ifdef DEBUG

// Precedes every allocation while allocations are counted. Aligning it as
//  strictly as malloc() aligns its results keeps the memory after it just as
//  aligned
struct allocator_header_t
{
  alignas(max_align_t) size_t size_bytes;
  enum allocator_tag_t tag;
};

static_assert(
  (sizeof(struct allocator_header_t) % alignof(max_align_t)) == 0,
  "Memory following an allocator header would be misaligned");

#endif

// Precedes every block of a pool, allocated or free. Its alignment keeps the
//  memory after it aligned for any type, as malloc()'s is
struct allocator_pool_block_t
{
  // The whole block's size, this header included; a multiple of the
  //  header's alignment
  alignas(max_align_t) size_t size_bytes;
  // The next free block by address, while this one is free
  struct allocator_pool_block_t* next_free_block;
};

static_assert(
  (sizeof(struct allocator_pool_block_t) % alignof(max_align_t)) == 0,
  "Memory following a pool block header would be misaligned");

// Where a tag's memory comes from
struct allocator_tag_source_t
{
  enum allocator_policy_t policy;
  // Set while policy is ALLOCATOR_POLICY_POOL
  struct allocator_pool_t* pool;
};

/*******************************************************************************
 *
 *   STATE
 *
 ******************************************************************************/

static const char* const allocator_tag_names[] =
{
  [ALLOCATOR_TAG_GENERAL]     = "general",
  [ALLOCATOR_TAG_APPLICATION] = "application",
  [ALLOCATOR_TAG_WINDOW]      = "window",
  [ALLOCATOR_TAG_RENDERER]    = "renderer",
  [ALLOCATOR_TAG_UI]          = "ui",
  [ALLOCATOR_TAG_FONT]        = "font",
  [ALLOCATOR_TAG_FILE]        = "file",
  [ALLOCATOR_TAG_DIAGNOSTICS] = "diagnostics",
  [ALLOCATOR_TAG_IMGUI]       = "imgui",
  [ALLOCATOR_TAG_FREETYPE]    = "freetype",
  [ALLOCATOR_TAG_STB_IMAGE]   = "stb_image",
};

static_assert(
  (sizeof(allocator_tag_names) / sizeof(allocator_tag_names[0])) == ALLOCATOR_TAG_COUNT,
  "Every allocator tag needs a name");

// What every tag's policy is until it's set otherwise
#ifdef JOTUNN_NO_HEAP
  #define ALLOCATOR_INITIAL_POLICY ALLOCATOR_POLICY_FORBIDDEN
#else
  #define ALLOCATOR_INITIAL_POLICY ALLOCATOR_POLICY_HEAP
#endif

static struct allocator_tag_source_t allocator_tag_sources[] =
{
  [ALLOCATOR_TAG_GENERAL]     = { .policy = ALLOCATOR_INITIAL_POLICY },
  [ALLOCATOR_TAG_APPLICATION] = { .policy = ALLOCATOR_INITIAL_POLICY },
  [ALLOCATOR_TAG_WINDOW]      = { .policy = ALLOCATOR_INITIAL_POLICY },
  [ALLOCATOR_TAG_RENDERER]    = { .policy = ALLOCATOR_INITIAL_POLICY },
  [ALLOCATOR_TAG_UI]          = { .policy = ALLOCATOR_INITIAL_POLICY },
  [ALLOCATOR_TAG_FONT]        = { .policy = ALLOCATOR_INITIAL_POLICY },
  [ALLOCATOR_TAG_FILE]        = { .policy = ALLOCATOR_INITIAL_POLICY },
  [ALLOCATOR_TAG_DIAGNOSTICS] = { .policy = ALLOCATOR_INITIAL_POLICY },
  [ALLOCATOR_TAG_IMGUI]       = { .policy = ALLOCATOR_INITIAL_POLICY },
  [ALLOCATOR_TAG_FREETYPE]    = { .policy = ALLOCATOR_INITIAL_POLICY },
  [ALLOCATOR_TAG_STB_IMAGE]   = { .policy = ALLOCATOR_INITIAL_POLICY },
};

static_assert(
  (sizeof(allocator_tag_sources) / sizeof(allocator_tag_sources[0])) == ALLOCATOR_TAG_COUNT,
  "Every allocator tag needs a source");

#ifdef DEBUG
static struct allocator_tag_stats_t allocator_tag_stats[ALLOCATOR_TAG_COUNT];
#endif

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Checks that a tag is one of allocator_tag_t's
//
//  in:     in_tag
//  ret:    true when in_tag is valid; otherwise false
static bool _allocator_is_tag_valid(
  enum allocator_tag_t in_tag);

// Replaces a tag that isn't valid with ALLOCATOR_TAG_GENERAL
//
//  in:     in_tag
//  ret:    in_tag when it's valid; otherwise ALLOCATOR_TAG_GENERAL
static enum allocator_tag_t allocator_get_valid_tag(
  enum allocator_tag_t in_tag);

// Allocates a block from wherever a tag's policy says
//
//  in:     in_size_bytes, in_tag, in_is_zeroed
//  ret:    The block; NULL when it can't be allocated, or the tag's policy
//            is ALLOCATOR_POLICY_FORBIDDEN
static void* allocator_source_allocate(
  size_t in_size_bytes,
  enum allocator_tag_t in_tag,
  bool in_is_zeroed);

// Resizes a block where it came from, which must be where a tag's policy
//  says
//
//  in:     in_size_bytes, in_tag
//  in/out: inout_memory
//  ret:    The resized block, which may have moved; NULL when it can't be
//            resized, or the tag's policy doesn't name where it came from,
//            leaving it as it was
static void* allocator_source_reallocate(
  void* inout_memory,
  size_t in_size_bytes,
  enum allocator_tag_t in_tag);

// Releases a block back to where it came from
//
//  in/out: inout_memory
static void allocator_source_free(
  void* inout_memory);

// Finds the pool, among those serving a tag, that a block came from
//
//  in:     in_memory
//  ret:    The pool; NULL when in_memory came from the heap
static struct allocator_pool_t* allocator_find_pool(
  const void* in_memory);

// Finds the size of the pool block holding an allocation
//
//  in:     in_size_bytes
//  out:    out_block_size_bytes
//  ret:    true on success; false when the block's size overflows
static bool allocator_pool_get_block_size(
  size_t in_size_bytes,
  size_t* out_block_size_bytes);

// Allocates a block from a pool, first fit
//
//  in:     in_size_bytes
//  in/out: inout_pool
//  ret:    The memory after the block's header; NULL when no free block is
//            large enough
static void* allocator_pool_allocate(
  struct allocator_pool_t* inout_pool,
  size_t in_size_bytes);

// Resizes a block from a pool, in place when it can be
//
//  in:     in_size_bytes
//  in/out: inout_pool, inout_memory
//  ret:    The resized memory, which may have moved; NULL when no free block
//            is large enough, leaving inout_memory as it was
static void* allocator_pool_reallocate(
  struct allocator_pool_t* inout_pool,
  void* inout_memory,
  size_t in_size_bytes);

// Releases a block back to a pool
//
//  in/out: inout_pool, inout_memory
static void allocator_pool_free(
  struct allocator_pool_t* inout_pool,
  void* inout_memory);

// Splits the end off a block from a pool, leaving it in_size_bytes, and
//  releases the end; does nothing when the end would be too small to be a
//  block of its own
//
//  in:     in_block_size_bytes
//  in/out: inout_pool, inout_block
static void allocator_pool_trim_block(
  struct allocator_pool_t* inout_pool,
  struct allocator_pool_block_t* inout_block,
  size_t in_block_size_bytes);

// Adds a block to a pool's free blocks, merging it with any free neighbour
//
//  in/out: inout_pool, inout_block
static void allocator_pool_insert_free_block(
  struct allocator_pool_t* inout_pool,
  struct allocator_pool_block_t* inout_block);

#ifdef DEBUG

// Allocates memory behind a header recording its size and tag, and counts it
//  against the tag; an invalid tag is counted as ALLOCATOR_TAG_GENERAL
//
//  in:     in_size_bytes, in_tag, in_is_zeroed
//  ret:    The memory after the header; NULL when it can't be allocated
static void* _allocator_allocate_counted(
  size_t in_size_bytes,
  enum allocator_tag_t in_tag,
  bool in_is_zeroed);

// Adds an allocation to a tag's counts
//
//  in:     in_tag, in_size_bytes
static void _allocator_count_allocation(
  enum allocator_tag_t in_tag,
  size_t in_size_bytes);

// Removes an allocation from a tag's counts
//
//  in:     in_tag, in_size_bytes
static void _allocator_count_release(
  enum allocator_tag_t in_tag,
  size_t in_size_bytes);

#endif

/*******************************************************************************
 *
 *   Function: allocator_allocate()
 *
 ******************************************************************************/
void* allocator_allocate(
  size_t in_size_bytes,
  enum allocator_tag_t in_tag)
{
#ifdef DEBUG
  return _allocator_allocate_counted(
    in_size_bytes,
    in_tag,
    false);
#else
  return allocator_source_allocate(
    in_size_bytes,
    in_tag,
    false);
#endif
}

/*******************************************************************************
 *
 *   Function: allocator_allocate_zeroed()
 *
 ******************************************************************************/
void* allocator_allocate_zeroed(
  size_t in_count,
  size_t in_element_size_bytes,
  enum allocator_tag_t in_tag)
{
  if ((in_element_size_bytes != 0) &&
      (in_count > (SIZE_MAX / in_element_size_bytes)))
  {
    return NULL;
  }

#ifdef DEBUG
  return _allocator_allocate_counted(
    (in_count * in_element_size_bytes),
    in_tag,
    true);
#else
  return allocator_source_allocate(
    (in_count * in_element_size_bytes),
    in_tag,
    true);
#endif
}

/*******************************************************************************
 *
 *   Function: allocator_reallocate()
 *
 ******************************************************************************/
void* allocator_reallocate(
  void* inout_memory,
  size_t in_size_bytes,
  enum allocator_tag_t in_tag)
{
  if (inout_memory == NULL) {
    return allocator_allocate(
      in_size_bytes,
      in_tag);
  }

  if (in_size_bytes == 0) {
    allocator_free(inout_memory);

    return NULL;
  }

#ifdef DEBUG
  if (in_size_bytes > (SIZE_MAX - sizeof(struct allocator_header_t))) {
    return NULL;
  }

  struct allocator_header_t* header = ((struct allocator_header_t*)inout_memory) - 1;

  // The header can't be read once the block has been resized
  const size_t previous_size_bytes = header->size_bytes;
  const enum allocator_tag_t previous_tag = header->tag;

  const enum allocator_tag_t tag = allocator_get_valid_tag(in_tag);

  struct allocator_header_t* resized_header =
    (struct allocator_header_t*)allocator_source_reallocate(
      header,
      (sizeof(struct allocator_header_t) + in_size_bytes),
      tag);

  if (resized_header == NULL) {
    return NULL;
  }

  resized_header->size_bytes = in_size_bytes;
  resized_header->tag = tag;

  _allocator_count_release(
    previous_tag,
    previous_size_bytes);
  _allocator_count_allocation(
    tag,
    in_size_bytes);

  return (resized_header + 1);
#else
  return allocator_source_reallocate(
    inout_memory,
    in_size_bytes,
    in_tag);
#endif
}

/*******************************************************************************
 *
 *   Function: allocator_free()
 *
 ******************************************************************************/
void allocator_free(
  void* inout_memory)
{
  if (inout_memory == NULL) {
    return;
  }

#ifdef DEBUG
  struct allocator_header_t* header = ((struct allocator_header_t*)inout_memory) - 1;

  _allocator_count_release(
    header->tag,
    header->size_bytes);

  allocator_source_free(header);
#else
  allocator_source_free(inout_memory);
#endif
}

/*******************************************************************************
 *
 *   Function: allocator_duplicate_string()
 *
 ******************************************************************************/
char* allocator_duplicate_string(
  const char* in_string,
  enum allocator_tag_t in_tag)
{
  const size_t size_bytes = (strlen(in_string) + 1);

  char* copy =
    (char*)allocator_allocate(
      size_bytes,
      in_tag);

  if (copy != NULL) {
    memcpy(
      copy,
      in_string,
      size_bytes);
  }

  return copy;
}

/*******************************************************************************
 *
 *   Function: allocator_pool_init()
 *
 ******************************************************************************/
bool allocator_pool_init(
  struct allocator_pool_t* out_pool,
  void* inout_memory,
  size_t in_size_bytes)
{
  memset(
    out_pool,
    0,
    sizeof(struct allocator_pool_t));

  if (inout_memory == NULL) {
    return false;
  }

  // Blocks start and end on the header's alignment, so the memory is trimmed
  //  to it at both ends
  const uintptr_t alignment = alignof(struct allocator_pool_block_t);
  const uintptr_t start = (uintptr_t)inout_memory;
  const uintptr_t aligned_start = ((start + (alignment - 1)) & ~(alignment - 1));
  const size_t lost_bytes = (size_t)(aligned_start - start);

  if (in_size_bytes < lost_bytes) {
    return false;
  }

  const size_t size_bytes = ((in_size_bytes - lost_bytes) & ~(size_t)(alignment - 1));

  // Room for a header and the smallest allocation after it
  if (size_bytes < (sizeof(struct allocator_pool_block_t) + alignment)) {
    return false;
  }

  struct allocator_pool_block_t* block = (struct allocator_pool_block_t*)aligned_start;
  block->size_bytes = size_bytes;
  block->next_free_block = NULL;

  out_pool->memory = (unsigned char*)aligned_start;
  out_pool->size_bytes = size_bytes;
  out_pool->first_free_block = block;

  return true;
}

/*******************************************************************************
 *
 *   Function: allocator_set_tag_policy()
 *
 ******************************************************************************/
bool allocator_set_tag_policy(
  enum allocator_tag_t in_tag,
  enum allocator_policy_t in_policy,
  struct allocator_pool_t* inout_pool)
{
  // An enum may be signed, so a negative policy wraps past the end here too
  if (!_allocator_is_tag_valid(in_tag) ||
      ((unsigned int)in_policy >= (unsigned int)ALLOCATOR_POLICY_COUNT))
  {
    return false;
  }

  if ((in_policy == ALLOCATOR_POLICY_POOL) &&
      (inout_pool == NULL))
  {
    return false;
  }

#ifdef JOTUNN_NO_HEAP
  if (in_policy == ALLOCATOR_POLICY_HEAP) {
    return false;
  }
#endif

  struct allocator_tag_source_t* source = &(allocator_tag_sources[in_tag]);

  source->policy = in_policy;
  source->pool =
    (in_policy == ALLOCATOR_POLICY_POOL) ?
      inout_pool :
      NULL;

  return true;
}

/*******************************************************************************
 *
 *   Function: allocator_get_tag_policy()
 *
 ******************************************************************************/
enum allocator_policy_t allocator_get_tag_policy(
  enum allocator_tag_t in_tag)
{
  return _allocator_is_tag_valid(in_tag) ?
    allocator_tag_sources[in_tag].policy :
    ALLOCATOR_POLICY_FORBIDDEN;
}

/*******************************************************************************
 *
 *   Function: allocator_get_tag_stats()
 *
 ******************************************************************************/
bool allocator_get_tag_stats(
  enum allocator_tag_t in_tag,
  struct allocator_tag_stats_t* out_stats)
{
  memset(
    out_stats,
    0,
    sizeof(struct allocator_tag_stats_t));

#ifdef DEBUG
  if (_allocator_is_tag_valid(in_tag)) {
    *out_stats = allocator_tag_stats[in_tag];

    return true;
  }
#else
  (void)in_tag;
#endif

  return false;
}

/*******************************************************************************
 *
 *   Function: allocator_get_tag_name()
 *
 ******************************************************************************/
const char* allocator_get_tag_name(
  enum allocator_tag_t in_tag)
{
  return _allocator_is_tag_valid(in_tag) ?
    allocator_tag_names[in_tag] :
    "unknown";
}

/*******************************************************************************
 *
 *   Function: _allocator_is_tag_valid()
 *
 ******************************************************************************/
static bool _allocator_is_tag_valid(
  enum allocator_tag_t in_tag)
{
  // An enum may be signed, so a negative tag wraps past the end here too
  return ((unsigned int)in_tag < (unsigned int)ALLOCATOR_TAG_COUNT);
}

/*******************************************************************************
 *
 *   Function: allocator_get_valid_tag()
 *
 ******************************************************************************/
static enum allocator_tag_t allocator_get_valid_tag(
  enum allocator_tag_t in_tag)
{
  return _allocator_is_tag_valid(in_tag) ?
    in_tag :
    ALLOCATOR_TAG_GENERAL;
}

/*******************************************************************************
 *
 *   Function: allocator_source_allocate()
 *
 ******************************************************************************/
static void* allocator_source_allocate(
  size_t in_size_bytes,
  enum allocator_tag_t in_tag,
  bool in_is_zeroed)
{
  const struct allocator_tag_source_t* source =
    &(allocator_tag_sources[allocator_get_valid_tag(in_tag)]);

  switch (source->policy)
  {
  #ifndef JOTUNN_NO_HEAP
    case ALLOCATOR_POLICY_HEAP:
    {
      if (in_is_zeroed) {
        return calloc(
          1,
          in_size_bytes);
      }

      return malloc(in_size_bytes);
    }
  #endif

    case ALLOCATOR_POLICY_POOL:
    {
      void* memory =
        allocator_pool_allocate(
          source->pool,
          in_size_bytes);

      if ((memory != NULL) &&
          in_is_zeroed)
      {
        memset(
          memory,
          0,
          in_size_bytes);
      }

      return memory;
    }

    default:
    {
      return NULL;
    }
  }
}

/*******************************************************************************
 *
 *   Function: allocator_source_reallocate()
 *
 ******************************************************************************/
static void* allocator_source_reallocate(
  void* inout_memory,
  size_t in_size_bytes,
  enum allocator_tag_t in_tag)
{
  const struct allocator_tag_source_t* source =
    &(allocator_tag_sources[allocator_get_valid_tag(in_tag)]);
  struct allocator_pool_t* pool = allocator_find_pool(inout_memory);

  // Moving a block between sources would need its size, which a block from
  //  the heap doesn't record outside DEBUG builds
#ifndef JOTUNN_NO_HEAP
  if ((source->policy == ALLOCATOR_POLICY_HEAP) &&
      (pool == NULL))
  {
    return realloc(
      inout_memory,
      in_size_bytes);
  }
#endif

  if ((source->policy == ALLOCATOR_POLICY_POOL) &&
      (pool == source->pool))
  {
    return allocator_pool_reallocate(
      pool,
      inout_memory,
      in_size_bytes);
  }

  return NULL;
}

/*******************************************************************************
 *
 *   Function: allocator_source_free()
 *
 ******************************************************************************/
static void allocator_source_free(
  void* inout_memory)
{
  struct allocator_pool_t* pool = allocator_find_pool(inout_memory);

  if (pool != NULL) {
    allocator_pool_free(
      pool,
      inout_memory);
  }
#ifndef JOTUNN_NO_HEAP
  else {
    free(inout_memory);
  }
#endif
}

/*******************************************************************************
 *
 *   Function: allocator_find_pool()
 *
 ******************************************************************************/
static struct allocator_pool_t* allocator_find_pool(
  const void* in_memory)
{
  const uintptr_t address = (uintptr_t)in_memory;

  for (int tag = 0; tag < ALLOCATOR_TAG_COUNT; tag++) {
    struct allocator_pool_t* pool = allocator_tag_sources[tag].pool;

    if (pool == NULL) {
      continue;
    }

    const uintptr_t start = (uintptr_t)pool->memory;

    if ((address >= start) &&
        ((address - start) < pool->size_bytes))
    {
      return pool;
    }
  }

  return NULL;
}

/*******************************************************************************
 *
 *   Function: allocator_pool_get_block_size()
 *
 ******************************************************************************/
static bool allocator_pool_get_block_size(
  size_t in_size_bytes,
  size_t* out_block_size_bytes)
{
  const size_t alignment = alignof(struct allocator_pool_block_t);

  // A zero-byte allocation still gets a block of its own, as malloc(0)'s
  //  result may
  const size_t size_bytes =
    (in_size_bytes > 0) ?
      in_size_bytes :
      1;

  if (size_bytes > (SIZE_MAX - sizeof(struct allocator_pool_block_t) - alignment)) {
    return false;
  }

  const size_t rounded_size_bytes = ((size_bytes + (alignment - 1)) & ~(alignment - 1));

  *out_block_size_bytes = (sizeof(struct allocator_pool_block_t) + rounded_size_bytes);

  return true;
}

/*******************************************************************************
 *
 *   Function: allocator_pool_allocate()
 *
 ******************************************************************************/
static void* allocator_pool_allocate(
  struct allocator_pool_t* inout_pool,
  size_t in_size_bytes)
{
  size_t block_size_bytes;
  if (!allocator_pool_get_block_size(
        in_size_bytes,
        &(block_size_bytes)))
  {
    return NULL;
  }

  struct allocator_pool_block_t* previous = NULL;
  struct allocator_pool_block_t* block = inout_pool->first_free_block;

  while ((block != NULL) &&
         (block->size_bytes < block_size_bytes))
  {
    previous = block;
    block = block->next_free_block;
  }

  if (block == NULL) {
    return NULL;
  }

  if (previous != NULL) {
    previous->next_free_block = block->next_free_block;
  }
  else {
    inout_pool->first_free_block = block->next_free_block;
  }

  block->next_free_block = NULL;

  inout_pool->used_bytes += block->size_bytes;

  // What the allocation doesn't need goes back to the free blocks
  allocator_pool_trim_block(
    inout_pool,
    block,
    block_size_bytes);

  if (inout_pool->used_bytes > inout_pool->peak_used_bytes) {
    inout_pool->peak_used_bytes = inout_pool->used_bytes;
  }

  return (block + 1);
}

/*******************************************************************************
 *
 *   Function: allocator_pool_reallocate()
 *
 ******************************************************************************/
static void* allocator_pool_reallocate(
  struct allocator_pool_t* inout_pool,
  void* inout_memory,
  size_t in_size_bytes)
{
  size_t block_size_bytes;
  if (!allocator_pool_get_block_size(
        in_size_bytes,
        &(block_size_bytes)))
  {
    return NULL;
  }

  struct allocator_pool_block_t* block = ((struct allocator_pool_block_t*)inout_memory) - 1;

  // Grown in place when the block after it is free and large enough
  if (block->size_bytes < block_size_bytes) {
    struct allocator_pool_block_t* following =
      (struct allocator_pool_block_t*)((unsigned char*)block + block->size_bytes);

    struct allocator_pool_block_t* previous = NULL;
    struct allocator_pool_block_t* free_block = inout_pool->first_free_block;

    while ((free_block != NULL) &&
           (free_block < following))
    {
      previous = free_block;
      free_block = free_block->next_free_block;
    }

    if ((free_block == following) &&
        ((block->size_bytes + following->size_bytes) >= block_size_bytes))
    {
      if (previous != NULL) {
        previous->next_free_block = following->next_free_block;
      }
      else {
        inout_pool->first_free_block = following->next_free_block;
      }

      block->size_bytes += following->size_bytes;
      inout_pool->used_bytes += following->size_bytes;

      if (inout_pool->used_bytes > inout_pool->peak_used_bytes) {
        inout_pool->peak_used_bytes = inout_pool->used_bytes;
      }
    }
  }

  if (block->size_bytes >= block_size_bytes) {
    allocator_pool_trim_block(
      inout_pool,
      block,
      block_size_bytes);

    return inout_memory;
  }

  // Otherwise it moves, which needs room for both copies at once
  void* moved_memory =
    allocator_pool_allocate(
      inout_pool,
      in_size_bytes);

  if (moved_memory == NULL) {
    return NULL;
  }

  // Being smaller than the new block, the old one is copied whole
  memcpy(
    moved_memory,
    inout_memory,
    (block->size_bytes - sizeof(struct allocator_pool_block_t)));

  allocator_pool_free(
    inout_pool,
    inout_memory);

  return moved_memory;
}

/*******************************************************************************
 *
 *   Function: allocator_pool_free()
 *
 ******************************************************************************/
static void allocator_pool_free(
  struct allocator_pool_t* inout_pool,
  void* inout_memory)
{
  struct allocator_pool_block_t* block = ((struct allocator_pool_block_t*)inout_memory) - 1;

  inout_pool->used_bytes -= block->size_bytes;

  allocator_pool_insert_free_block(
    inout_pool,
    block);
}

/*******************************************************************************
 *
 *   Function: allocator_pool_trim_block()
 *
 ******************************************************************************/
static void allocator_pool_trim_block(
  struct allocator_pool_t* inout_pool,
  struct allocator_pool_block_t* inout_block,
  size_t in_block_size_bytes)
{
  const size_t smallest_block_size_bytes =
    (sizeof(struct allocator_pool_block_t) + alignof(struct allocator_pool_block_t));

  if ((inout_block->size_bytes - in_block_size_bytes) < smallest_block_size_bytes) {
    return;
  }

  struct allocator_pool_block_t* end =
    (struct allocator_pool_block_t*)((unsigned char*)inout_block + in_block_size_bytes);
  end->size_bytes = (inout_block->size_bytes - in_block_size_bytes);

  inout_block->size_bytes = in_block_size_bytes;
  inout_pool->used_bytes -= end->size_bytes;

  allocator_pool_insert_free_block(
    inout_pool,
    end);
}

/*******************************************************************************
 *
 *   Function: allocator_pool_insert_free_block()
 *
 ******************************************************************************/
static void allocator_pool_insert_free_block(
  struct allocator_pool_t* inout_pool,
  struct allocator_pool_block_t* inout_block)
{
  struct allocator_pool_block_t* previous = NULL;
  struct allocator_pool_block_t* next = inout_pool->first_free_block;

  while ((next != NULL) &&
         (next < inout_block))
  {
    previous = next;
    next = next->next_free_block;
  }

  inout_block->next_free_block = next;

  if (previous != NULL) {
    previous->next_free_block = inout_block;
  }
  else {
    inout_pool->first_free_block = inout_block;
  }

  // Merged with the free block after it, then the one before it, when they
  //  touch
  if ((next != NULL) &&
      (((unsigned char*)inout_block + inout_block->size_bytes) == (unsigned char*)next))
  {
    inout_block->size_bytes += next->size_bytes;
    inout_block->next_free_block = next->next_free_block;
  }

  if ((previous != NULL) &&
      (((unsigned char*)previous + previous->size_bytes) == (unsigned char*)inout_block))
  {
    previous->size_bytes += inout_block->size_bytes;
    previous->next_free_block = inout_block->next_free_block;
  }
}

#ifdef DEBUG

/*******************************************************************************
 *
 *   Function: _allocator_allocate_counted()
 *
 ******************************************************************************/
static void* _allocator_allocate_counted(
  size_t in_size_bytes,
  enum allocator_tag_t in_tag,
  bool in_is_zeroed)
{
  if (in_size_bytes > (SIZE_MAX - sizeof(struct allocator_header_t))) {
    return NULL;
  }

  const size_t block_size_bytes = (sizeof(struct allocator_header_t) + in_size_bytes);

  const enum allocator_tag_t tag = allocator_get_valid_tag(in_tag);

  struct allocator_header_t* header =
    (struct allocator_header_t*)allocator_source_allocate(
      block_size_bytes,
      tag,
      in_is_zeroed);

  if (header == NULL) {
    return NULL;
  }

  header->size_bytes = in_size_bytes;
  header->tag = tag;

  _allocator_count_allocation(
    tag,
    in_size_bytes);

  return (header + 1);
}

/*******************************************************************************
 *
 *   Function: _allocator_count_allocation()
 *
 ******************************************************************************/
static void _allocator_count_allocation(
  enum allocator_tag_t in_tag,
  size_t in_size_bytes)
{
  struct allocator_tag_stats_t* stats = &(allocator_tag_stats[in_tag]);

  stats->live_bytes += in_size_bytes;
  stats->live_count++;
  stats->total_count++;

  if (stats->live_bytes > stats->peak_live_bytes) {
    stats->peak_live_bytes = stats->live_bytes;
  }
}

/*******************************************************************************
 *
 *   Function: _allocator_count_release()
 *
 ******************************************************************************/
static void _allocator_count_release(
  enum allocator_tag_t in_tag,
  size_t in_size_bytes)
{
  struct allocator_tag_stats_t* stats = &(allocator_tag_stats[in_tag]);

  stats->live_bytes -= in_size_bytes;
  stats->live_count--;
}

#endif
