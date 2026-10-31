#pragma once

// The allocation storage example's application: one window with a 2D layer
//  under a UI layer, set up entirely on memory the application provides.
//  Setting it up is here, in allocation_storage_app.c; where its memory
//  comes from is in allocation_storage_app_memory.c, what the 2D layer draws
//  is in allocation_storage_app_scene.c, and the interface the UI layer
//  draws is in allocation_storage_app_ui.c

#include "allocation_storage_app_blocks.h"
#include "allocation_storage_app_memory.h"
#include "application.h"
#include "ui_layer.h"
#include "window_layer_2d.h"

#include <stdint.h>

#define ALLOCATION_STORAGE_APP_WIDTH_PIXELS 900
#define ALLOCATION_STORAGE_APP_HEIGHT_PIXELS 640

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// Everything the application owns. The layers' callbacks get back to it with
//  container_of()
struct allocation_storage_app_t
{
  struct application_t application;
  struct window_t window;
  //
  // The 2D layer, with the framebuffer, camera and renderer it draws with
  struct window_layer_2d_t scene_layer;
  //
  struct ui_layer_t ui_layer;
  //
  // The memory Jotunn uses, all of it provided here
  struct allocation_storage_app_memory_t memory;
  // The application's own allocations, made from the UI
  struct allocation_storage_app_blocks_t blocks;
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Configures the allocator, before anything is allocated, then sets up the
//  application and its window, with the 2D layer under the UI layer, all on
//  the app's provided memory
//
//  out:    out_app
//  ret:    0 on success; non-zero when the allocator can't be configured, or
//            the application, its window or its layers can't be set up
uint8_t allocation_storage_app_init(
  struct allocation_storage_app_t* out_app);

// Releases what allocation_storage_app_init() set up, and any blocks the UI
//  left allocated
//
//  in/out: inout_app
void allocation_storage_app_cleanup(
  struct allocation_storage_app_t* inout_app);
