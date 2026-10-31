#include "renderer_2d_app.h"
#include "renderer_2d_app_scene.h"

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Opens the window, leaving its graphics context current. Its list of layers
//  is allocated by the engine, being given NULL storage; see
//  AllocationStorageApp for providing it
//
//  in/out: inout_app
//  ret:    0 on success; non-zero when the window can't be opened
static uint8_t renderer_2d_app_create_window(
  struct renderer_2d_app_t* inout_app);

// Sets up the scene's 2D layer, covering the window, has it call
//  renderer_2d_app_scene_run() each frame, and adds it to the window. The
//  renderer's batch storage is allocated by the engine. The window's
//  graphics context must be current
//
//  in/out: inout_app
//  ret:    0 on success; non-zero when the renderer's storage can't be
//            allocated or the layer can't be added
static uint8_t renderer_2d_app_create_scene_layer(
  struct renderer_2d_app_t* inout_app);

/*******************************************************************************
 *
 *   Function: renderer_2d_app_init()
 *
 ******************************************************************************/
uint8_t renderer_2d_app_init(
  struct renderer_2d_app_t* out_app)
{
  const uint8_t max_windows = 1;

  // The engine allocates the list of windows, being given NULL storage
  uint8_t error =
    application_init(
      &(out_app->application),
      "Renderer2DApp",
      NULL,
      max_windows);

  if (!error) {
    error = renderer_2d_app_create_window(out_app);
  }

  // Graphics resources belong to the context current as they're made, which
  //  the window's now is
  if (!error) {
    error = renderer_2d_app_create_scene_layer(out_app);
  }

  if (!error) {
    renderer_2d_app_resources_load(&(out_app->resources));

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
 *   Function: renderer_2d_app_cleanup()
 *
 ******************************************************************************/
void renderer_2d_app_cleanup(
  struct renderer_2d_app_t* inout_app)
{
  // The window runs leave no graphics context current, and these were made
  //  in the window's
  window_set_context(&(inout_app->window));

  renderer_2d_app_resources_release(&(inout_app->resources));
  window_layer_2d_cleanup(&(inout_app->scene_layer));

  // Cleans up the window too, but leaves its memory, which is ours
  application_cleanup(&(inout_app->application));
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_create_window()
 *
 ******************************************************************************/
static uint8_t renderer_2d_app_create_window(
  struct renderer_2d_app_t* inout_app)
{
  const uint8_t max_window_layers = 1;

  const uint8_t error =
    window_init(
      &(inout_app->window),
      NULL,
      max_window_layers,
      RENDERER_2D_APP_WIDTH_PIXELS,
      RENDERER_2D_APP_HEIGHT_PIXELS,
      "Jotunn - Renderer 2D Example",
      &(inout_app->application));

  return error;
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_create_scene_layer()
 *
 ******************************************************************************/
static uint8_t renderer_2d_app_create_scene_layer(
  struct renderer_2d_app_t* inout_app)
{
  // A framebuffer of the window's size, a camera mapping one unit to one
  //  pixel from the window's bottom-left corner, and a renderer drawing
  //  through it, tied together as a layer. The window begins the renderer's
  //  scene, calls the layer's run, then ends the scene, each frame. The
  //  engine allocates the batch storage, being given NULL
  uint8_t error =
    window_layer_2d_init(
      &(inout_app->window),
      &(inout_app->scene_layer),
      NULL,
      "renderer_2d_app_scene_renderer",
      &(renderer_2d_app_scene_run));

  if (!error) {
    error =
      window_add_layer(
        &(inout_app->window),
        &(inout_app->scene_layer.window_layer));
  }

  return error;
}
