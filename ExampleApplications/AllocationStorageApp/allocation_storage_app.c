#include "allocation_storage_app.h"
#include "allocation_storage_app_scene.h"
#include "allocation_storage_app_ui.h"

#include <stdio.h>

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Opens the window, leaving its graphics context current, with its list of
//  layers kept in the app's provided memory rather than allocated
//
//  in/out: inout_app
//  ret:    0 on success; non-zero when the window can't be opened
static uint8_t allocation_storage_app_create_window(
  struct allocation_storage_app_t* inout_app);

// Sets up the 2D layer, covering the window, with a renderer keeping its
//  batches in the app's provided memory, has it call
//  allocation_storage_app_scene_run() each frame, and adds it to the window.
//  The window's graphics context must be current
//
//  in/out: inout_app
//  ret:    0 on success; non-zero when the layer can't be added
static uint8_t allocation_storage_app_create_scene_layer(
  struct allocation_storage_app_t* inout_app);

// Sets up the UI layer, its renderer keeping its batches in the app's
//  provided memory, has it call allocation_storage_app_ui_render() each
//  frame, and adds it to the window, over the 2D layer. ImGui's own memory
//  comes from the pool, under the IMGUI and UI tags. The window's graphics
//  context must be current
//
//  in/out: inout_app
//  ret:    0 on success; non-zero when the layer can't be added
static uint8_t allocation_storage_app_create_ui_layer(
  struct allocation_storage_app_t* inout_app);

/*******************************************************************************
 *
 *   Function: allocation_storage_app_init()
 *
 ******************************************************************************/
uint8_t allocation_storage_app_init(
  struct allocation_storage_app_t* out_app)
{
  // First, before anything is allocated
  if (!allocation_storage_app_memory_init(&(out_app->memory))) {
    fprintf(
      stderr,
      "Couldn't configure the allocator\n");

    return 1;
  }

  allocation_storage_app_blocks_init(&(out_app->blocks));

  // The list of windows kept in the app's provided memory, rather than
  //  allocated under the APPLICATION tag
  uint8_t error =
    application_init(
      &(out_app->application),
      "AllocationStorageApp",
      out_app->memory.window_storage,
      ALLOCATION_STORAGE_APP_MEMORY_MAX_WINDOWS);

  if (!error) {
    error = allocation_storage_app_create_window(out_app);
  }

  // Graphics resources belong to the context current as they're made, which
  //  the window's now is
  if (!error) {
    error = allocation_storage_app_create_scene_layer(out_app);
  }

  if (!error) {
    error = allocation_storage_app_create_ui_layer(out_app);
  }

  if (!error) {
    window_set_background_color(
      &(out_app->window),
      (union fvector4_t){{0.12f, 0.12f, 0.14f, 1.0f}});

    error =
      application_add_window(
        &(out_app->application),
        &(out_app->window));
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_cleanup()
 *
 ******************************************************************************/
void allocation_storage_app_cleanup(
  struct allocation_storage_app_t* inout_app)
{
  // Back to the pool before application_cleanup() looks for leaks
  allocation_storage_app_blocks_release_all(&(inout_app->blocks));

  // The window runs leave no graphics context current, and these were made
  //  in the window's
  window_set_context(&(inout_app->window));

  window_layer_2d_cleanup(&(inout_app->scene_layer));

  // Makes the window's graphics context current itself, and releases it
  //  after
  ui_layer_cleanup(&(inout_app->ui_layer));

  // Cleans up the window too, but leaves its memory and the storage the
  //  application provided, which are ours
  application_cleanup(&(inout_app->application));
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_create_window()
 *
 ******************************************************************************/
static uint8_t allocation_storage_app_create_window(
  struct allocation_storage_app_t* inout_app)
{
  // The list of layers kept in the app's provided memory, rather than
  //  allocated under the WINDOW tag
  const uint8_t error =
    window_init(
      &(inout_app->window),
      inout_app->memory.window_layer_storage,
      ALLOCATION_STORAGE_APP_MEMORY_MAX_WINDOW_LAYERS,
      ALLOCATION_STORAGE_APP_WIDTH_PIXELS,
      ALLOCATION_STORAGE_APP_HEIGHT_PIXELS,
      "Jotunn - Allocation Storage Example",
      &(inout_app->application));

  return error;
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_create_scene_layer()
 *
 ******************************************************************************/
static uint8_t allocation_storage_app_create_scene_layer(
  struct allocation_storage_app_t* inout_app)
{
  // The batches kept in the app's provided memory, rather than allocated
  //  under the RENDERER tag, so this can't fail for want of memory
  uint8_t error =
    window_layer_2d_init(
      &(inout_app->window),
      &(inout_app->scene_layer),
      &(inout_app->memory.scene_renderer_storage),
      "allocation_storage_app_scene_renderer",
      &(allocation_storage_app_scene_run));

  if (!error) {
    error =
      window_add_layer(
        &(inout_app->window),
        &(inout_app->scene_layer.window_layer));
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_create_ui_layer()
 *
 ******************************************************************************/
static uint8_t allocation_storage_app_create_ui_layer(
  struct allocation_storage_app_t* inout_app)
{
  // The UI renderer's batches kept in the app's provided memory too
  uint8_t error =
    ui_layer_init(
      &(inout_app->window),
      &(inout_app->ui_layer),
      &(inout_app->memory.ui_renderer_storage));

  if (!error) {
    ui_layer_set_custom_render(
      &(inout_app->ui_layer),
      &(allocation_storage_app_ui_render));

    // Added after the 2D layer, so drawn over it
    error =
      window_add_layer(
        &(inout_app->window),
        &(inout_app->ui_layer.layer_2d.window_layer));
  }

  return error;
}
