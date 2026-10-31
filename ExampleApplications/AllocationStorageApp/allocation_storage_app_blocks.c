#include "allocation_storage_app_blocks.h"

#include "allocator.h"

#include <stddef.h>

/*******************************************************************************
 *
 *   Function: allocation_storage_app_blocks_init()
 *
 ******************************************************************************/
void allocation_storage_app_blocks_init(
  struct allocation_storage_app_blocks_t* out_blocks)
{
  out_blocks->count = 0;
  out_blocks->status_message = "";
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_blocks_release_all()
 *
 ******************************************************************************/
void allocation_storage_app_blocks_release_all(
  struct allocation_storage_app_blocks_t* inout_blocks)
{
  while (inout_blocks->count > 0) {
    inout_blocks->count--;
    allocator_free(inout_blocks->blocks[inout_blocks->count]);
  }
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_blocks_allocate()
 *
 ******************************************************************************/
void allocation_storage_app_blocks_allocate(
  struct allocation_storage_app_blocks_t* inout_blocks)
{
  void* block = NULL;

  // GENERAL's policy is the pool, so the block comes from there
  if (inout_blocks->count < ALLOCATION_STORAGE_APP_BLOCKS_MAX_COUNT) {
    block =
      allocator_allocate(
        ALLOCATION_STORAGE_APP_BLOCKS_BLOCK_SIZE_BYTES,
        ALLOCATOR_TAG_GENERAL);
  }

  if (block != NULL) {
    inout_blocks->blocks[inout_blocks->count] = block;
    inout_blocks->count++;
    inout_blocks->status_message = "Allocated a block from the pool";
  }
  else {
    inout_blocks->status_message = "No room for another block";
  }
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_blocks_free_last()
 *
 ******************************************************************************/
void allocation_storage_app_blocks_free_last(
  struct allocation_storage_app_blocks_t* inout_blocks)
{
  if (inout_blocks->count > 0) {
    inout_blocks->count--;
    allocator_free(inout_blocks->blocks[inout_blocks->count]);
    inout_blocks->status_message = "Freed a block back to the pool";
  }
  else {
    inout_blocks->status_message = "No blocks to free";
  }
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_blocks_try_forbidden()
 *
 ******************************************************************************/
void allocation_storage_app_blocks_try_forbidden(
  struct allocation_storage_app_blocks_t* inout_blocks)
{
  void* block =
    allocator_allocate(
      1,
      ALLOCATOR_TAG_RENDERER);

  inout_blocks->status_message =
    (block == NULL) ?
      "Refused: RENDERER is forbidden, its storage being provided" :
      "Allocated under RENDERER, which should have been forbidden";

  // Ignored when NULL
  allocator_free(block);
}
