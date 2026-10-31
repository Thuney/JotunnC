#include "allocation_storage_app_memory.h"

#include <stddef.h>

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Finds whether the application provides the memory Jotunn would allocate
//  under a tag
//
//  in:     in_tag
//  ret:    true for the APPLICATION, WINDOW and RENDERER tags; otherwise
//            false
static inline bool allocation_storage_app_memory_is_tag_provided(
  enum allocator_tag_t in_tag);

/*******************************************************************************
 *
 *   Function: allocation_storage_app_memory_init()
 *
 ******************************************************************************/
bool allocation_storage_app_memory_init(
  struct allocation_storage_app_memory_t* inout_memory)
{
  // Some of the memory is lost to aligning the pool's start
  if (!allocator_pool_init(
        &(inout_memory->pool),
        inout_memory->pool_memory,
        sizeof(inout_memory->pool_memory)))
  {
    return false;
  }

  bool is_configured = true;

  for (unsigned int i = 0; i < ALLOCATOR_TAG_COUNT; i++) {
    const enum allocator_tag_t tag = (enum allocator_tag_t)i;

    // A forbidden tag needs no pool
    if (allocation_storage_app_memory_is_tag_provided(tag)) {
      is_configured &=
        allocator_set_tag_policy(
          tag,
          ALLOCATOR_POLICY_FORBIDDEN,
          NULL);
    }
    else {
      is_configured &=
        allocator_set_tag_policy(
          tag,
          ALLOCATOR_POLICY_POOL,
          &(inout_memory->pool));
    }
  }

  return is_configured;
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_memory_is_tag_provided()
 *
 ******************************************************************************/
static inline bool allocation_storage_app_memory_is_tag_provided(
  enum allocator_tag_t in_tag)
{
  return ((in_tag == ALLOCATOR_TAG_APPLICATION) ||
          (in_tag == ALLOCATOR_TAG_WINDOW) ||
          (in_tag == ALLOCATOR_TAG_RENDERER));
}
