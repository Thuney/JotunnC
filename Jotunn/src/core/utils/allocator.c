#include "allocator.h"

#include <assert.h>
#include <stdalign.h>
#include <stdlib.h>
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
  const enum allocator_tag_t in_tag);

#ifdef DEBUG

// Allocates memory behind a header recording its size and tag, and counts it
//  against the tag; an invalid tag is counted as ALLOCATOR_TAG_GENERAL
//
//  in:     in_size_bytes, in_tag, in_is_zeroed
//  ret:    The memory after the header; NULL when it can't be allocated
static void* _allocator_allocate_counted(
  const size_t in_size_bytes,
  const enum allocator_tag_t in_tag,
  const bool in_is_zeroed);

// Adds an allocation to a tag's counts
//
//  in:     in_tag, in_size_bytes
static void _allocator_count_allocation(
  const enum allocator_tag_t in_tag,
  const size_t in_size_bytes);

// Removes an allocation from a tag's counts
//
//  in:     in_tag, in_size_bytes
static void _allocator_count_release(
  const enum allocator_tag_t in_tag,
  const size_t in_size_bytes);

#endif

/*******************************************************************************
 *
 *   Function: allocator_allocate()
 *
 ******************************************************************************/
void* allocator_allocate(
  const size_t in_size_bytes,
  const enum allocator_tag_t in_tag)
{
#ifdef DEBUG
  return _allocator_allocate_counted(
    in_size_bytes,
    in_tag,
    false);
#else
  (void)in_tag;

  return malloc(in_size_bytes);
#endif
}

/*******************************************************************************
 *
 *   Function: allocator_allocate_zeroed()
 *
 ******************************************************************************/
void* allocator_allocate_zeroed(
  const size_t in_count,
  const size_t in_element_size_bytes,
  const enum allocator_tag_t in_tag)
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
  (void)in_tag;

  return calloc(
    in_count,
    in_element_size_bytes);
#endif
}

/*******************************************************************************
 *
 *   Function: allocator_reallocate()
 *
 ******************************************************************************/
void* allocator_reallocate(
  void* inout_memory,
  const size_t in_size_bytes,
  const enum allocator_tag_t in_tag)
{
  if (inout_memory == NULL)
  {
    return allocator_allocate(
      in_size_bytes,
      in_tag);
  }

  if (in_size_bytes == 0)
  {
    allocator_free(inout_memory);

    return NULL;
  }

#ifdef DEBUG
  if (in_size_bytes > (SIZE_MAX - sizeof(struct allocator_header_t)))
  {
    return NULL;
  }

  struct allocator_header_t* header = ((struct allocator_header_t*)inout_memory) - 1;

  // The header can't be read once realloc() has succeeded
  const size_t previous_size_bytes = header->size_bytes;
  const enum allocator_tag_t previous_tag = header->tag;

  struct allocator_header_t* resized_header =
    (struct allocator_header_t*)realloc(
      header,
      (sizeof(struct allocator_header_t) + in_size_bytes));

  if (resized_header == NULL)
  {
    return NULL;
  }

  const enum allocator_tag_t tag =
    _allocator_is_tag_valid(in_tag) ?
      in_tag :
      ALLOCATOR_TAG_GENERAL;

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
  (void)in_tag;

  return realloc(
    inout_memory,
    in_size_bytes);
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
  if (inout_memory == NULL)
  {
    return;
  }

#ifdef DEBUG
  struct allocator_header_t* header = ((struct allocator_header_t*)inout_memory) - 1;

  _allocator_count_release(
    header->tag,
    header->size_bytes);

  free(header);
#else
  free(inout_memory);
#endif
}

/*******************************************************************************
 *
 *   Function: allocator_duplicate_string()
 *
 ******************************************************************************/
char* allocator_duplicate_string(
  const char* in_string,
  const enum allocator_tag_t in_tag)
{
  const size_t size_bytes = (strlen(in_string) + 1);

  char* copy =
    (char*)allocator_allocate(
      size_bytes,
      in_tag);

  if (copy != NULL)
  {
    memcpy(
      copy,
      in_string,
      size_bytes);
  }

  return copy;
}

/*******************************************************************************
 *
 *   Function: allocator_get_tag_stats()
 *
 ******************************************************************************/
bool allocator_get_tag_stats(
  const enum allocator_tag_t in_tag,
  struct allocator_tag_stats_t* out_stats)
{
  memset(
    out_stats,
    0,
    sizeof(struct allocator_tag_stats_t));

#ifdef DEBUG
  if (_allocator_is_tag_valid(in_tag))
  {
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
  const enum allocator_tag_t in_tag)
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
  const enum allocator_tag_t in_tag)
{
  // An enum may be signed, so a negative tag wraps past the end here too
  return ((unsigned int)in_tag < (unsigned int)ALLOCATOR_TAG_COUNT);
}

#ifdef DEBUG

/*******************************************************************************
 *
 *   Function: _allocator_allocate_counted()
 *
 ******************************************************************************/
static void* _allocator_allocate_counted(
  const size_t in_size_bytes,
  const enum allocator_tag_t in_tag,
  const bool in_is_zeroed)
{
  if (in_size_bytes > (SIZE_MAX - sizeof(struct allocator_header_t)))
  {
    return NULL;
  }

  const size_t block_size_bytes = (sizeof(struct allocator_header_t) + in_size_bytes);

  struct allocator_header_t* header;
  if (in_is_zeroed)
  {
    header =
      (struct allocator_header_t*)calloc(
        1,
        block_size_bytes);
  }
  else
  {
    header = (struct allocator_header_t*)malloc(block_size_bytes);
  }

  if (header == NULL)
  {
    return NULL;
  }

  const enum allocator_tag_t tag =
    _allocator_is_tag_valid(in_tag) ?
      in_tag :
      ALLOCATOR_TAG_GENERAL;

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
  const enum allocator_tag_t in_tag,
  const size_t in_size_bytes)
{
  struct allocator_tag_stats_t* stats = &(allocator_tag_stats[in_tag]);

  stats->live_bytes += in_size_bytes;
  stats->live_count++;
  stats->total_count++;

  if (stats->live_bytes > stats->peak_live_bytes)
  {
    stats->peak_live_bytes = stats->live_bytes;
  }
}

/*******************************************************************************
 *
 *   Function: _allocator_count_release()
 *
 ******************************************************************************/
static void _allocator_count_release(
  const enum allocator_tag_t in_tag,
  const size_t in_size_bytes)
{
  struct allocator_tag_stats_t* stats = &(allocator_tag_stats[in_tag]);

  stats->live_bytes -= in_size_bytes;
  stats->live_count--;
}

#endif
