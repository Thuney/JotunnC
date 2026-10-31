#pragma once

// Where the allocation storage example's memory comes from, rather than the
//  C library's heap. Jotunn takes storage from the application for three
//  things: the list of windows, each window's list of layers, and each
//  renderer's batches. Those are all it allocates under the APPLICATION,
//  WINDOW and RENDERER allocator tags, so with them provided here, those tags
//  are forbidden, and anything that fell back to allocating would fail rather
//  than go unnoticed. Every other tag allocates from a pool, carved from
//  memory here too

#include "allocator.h"
#include "renderer_2d.h"
#include "window.h"

#include <stdbool.h>

#define ALLOCATION_STORAGE_APP_MEMORY_MAX_WINDOWS 1
#define ALLOCATION_STORAGE_APP_MEMORY_MAX_WINDOW_LAYERS 2

// Room for everything Jotunn allocates here, ImGui and the diagnostics
//  overlay (F3) included, with plenty to spare for the UI's blocks
#define ALLOCATION_STORAGE_APP_MEMORY_POOL_SIZE_BYTES (16u * 1024u * 1024u)

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// All of the application's memory for Jotunn, in one place. About 18 MB, so
//  it belongs in static storage rather than on the stack
struct allocation_storage_app_memory_t
{
  // Provided in place of what Jotunn would allocate: the lists of windows and
  //  layers, and both renderers' batches (about 840 KB each at the default
  //  batch size)
  struct window_t* window_storage[ALLOCATION_STORAGE_APP_MEMORY_MAX_WINDOWS];
  struct window_layer_t* window_layer_storage[ALLOCATION_STORAGE_APP_MEMORY_MAX_WINDOW_LAYERS];
  struct renderer_2d_storage_t scene_renderer_storage;
  struct renderer_2d_storage_t ui_renderer_storage;
  //
  // What every other tag allocates from, and the memory it's carved from
  struct allocator_pool_t pool;
  unsigned char pool_memory[ALLOCATION_STORAGE_APP_MEMORY_POOL_SIZE_BYTES];
};

/*******************************************************************************
 *
 *   SETUP
 *
 ******************************************************************************/

// Sets up the pool, and gives every allocator tag its policy: forbidden for
//  the tags whose memory is provided, and the pool for the rest. Must run
//  before anything is allocated, as a tag's policy should be set before it's
//  allocated with
//
//  in/out: inout_memory
//  ret:    true on success; false when the pool can't be set up or a tag
//            refuses its policy
bool allocation_storage_app_memory_init(
  struct allocation_storage_app_memory_t* inout_memory);
