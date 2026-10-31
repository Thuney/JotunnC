#pragma once

// The allocation storage example's own allocations, made through Jotunn's
//  allocator as the UI's buttons ask: blocks under the GENERAL tag, which
//  come from the pool, and an attempt under the forbidden RENDERER tag,
//  which is refused. Memory from the allocator goes back through it, with
//  allocator_free(), never free()

#define ALLOCATION_STORAGE_APP_BLOCKS_MAX_COUNT 8
#define ALLOCATION_STORAGE_APP_BLOCKS_BLOCK_SIZE_BYTES (1024u * 1024u)

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// The blocks allocated so far, and what the last request did
struct allocation_storage_app_blocks_t
{
  void* blocks[ALLOCATION_STORAGE_APP_BLOCKS_MAX_COUNT];
  unsigned int count;
  // Describes the last request, for the UI to show
  const char* status_message;
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Starts with no blocks
//
//  out:    out_blocks
void allocation_storage_app_blocks_init(
  struct allocation_storage_app_blocks_t* out_blocks);

// Frees every block still allocated, back to where it came from
//
//  in/out: inout_blocks
void allocation_storage_app_blocks_release_all(
  struct allocation_storage_app_blocks_t* inout_blocks);

/*******************************************************************************
 *
 *   REQUESTS
 *
 ******************************************************************************/

// Allocates another block under the GENERAL tag, unless there are
//  ALLOCATION_STORAGE_APP_BLOCKS_MAX_COUNT already or the pool has no room
//
//  in/out: inout_blocks
void allocation_storage_app_blocks_allocate(
  struct allocation_storage_app_blocks_t* inout_blocks);

// Frees the block allocated last, if there is one
//
//  in/out: inout_blocks
void allocation_storage_app_blocks_free_last(
  struct allocation_storage_app_blocks_t* inout_blocks);

// Tries to allocate under the RENDERER tag, which is forbidden, and reports
//  whether it was refused. Anything allocated after all is freed at once
//
//  in/out: inout_blocks
void allocation_storage_app_blocks_try_forbidden(
  struct allocation_storage_app_blocks_t* inout_blocks);
