// A Jotunn application choosing where Jotunn's memory comes from, rather than
//  leaving it all to the C library's heap:
//
//  - The application provides the storage for its list of windows, its
//    window's list of layers, and each renderer_2d's batches. Those are all
//    Jotunn allocates under the APPLICATION, WINDOW and RENDERER allocator
//    tags, so those tags are forbidden
//  - Every other tag allocates from a pool carved from the application's
//    memory
//
// The window has a 2D layer showing how full the pool is, under a UI layer
//  listing each tag's policy and memory, with buttons to allocate from the
//  pool and to try a forbidden tag. Set up this way, the application runs the
//  same in builds with JOTUNN_ENABLE_HEAP switched off, which have no heap.
//
// Reading order:
//  - main.c: the application's lifetime and main loop
//  - allocation_storage_app_memory.c: the memory the application provides,
//    and configuring the allocator to use it
//  - allocation_storage_app.c: setting up the application, its window and its
//    layers on that memory
//  - allocation_storage_app_blocks.c: allocating through Jotunn's allocator
//  - allocation_storage_app_scene.c: the pool's bar the 2D layer draws
//  - allocation_storage_app_ui.c: the interface the UI layer draws

#include "allocation_storage_app.h"

#include <stdint.h>

// Static rather than on the stack, as it holds all of the application's
//  memory for Jotunn, pool included
static struct allocation_storage_app_t allocation_storage_app;

/*******************************************************************************
 *
 *   Function: main()
 *
 ******************************************************************************/
int main(void)
{
  struct allocation_storage_app_t* app = &(allocation_storage_app);

  const uint8_t error = allocation_storage_app_init(app);

  // Runs frames until the window is closed
  if (!error) {
    application_run_until_closed(&(app->application));

    allocation_storage_app_cleanup(app);
  }

  return error;
}
