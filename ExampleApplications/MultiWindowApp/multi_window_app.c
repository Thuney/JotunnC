#include "multi_window_app.h"

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Adds both windows to the application, which runs a frame of each in the
//  order they're added and routes their events
//
//  in/out: inout_app
//  ret:    0 on success; non-zero when the application has no room for them
static uint8_t multi_window_app_add_windows(
  struct multi_window_app_t* inout_app);

/*******************************************************************************
 *
 *   Function: multi_window_app_init()
 *
 ******************************************************************************/
uint8_t multi_window_app_init(
  struct multi_window_app_t* out_app)
{
  const uint8_t max_windows = 2;

  multi_window_app_state_init(&(out_app->state));

  // The engine allocates the list of windows, being given NULL storage
  uint8_t error =
    application_init(
      &(out_app->application),
      "MultiWindowApp",
      NULL,
      max_windows);

  // Each window has its own graphics context, which its init leaves current
  //  while it makes its layers
  if (!error) {
    error =
      multi_window_app_scene_window_init(
        &(out_app->scene_window),
        &(out_app->application),
        &(out_app->state));
  }

  if (!error) {
    error =
      multi_window_app_controls_window_init(
        &(out_app->controls_window),
        &(out_app->application),
        &(out_app->state));
  }

  if (!error) {
    error = multi_window_app_add_windows(out_app);
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: multi_window_app_cleanup()
 *
 ******************************************************************************/
void multi_window_app_cleanup(
  struct multi_window_app_t* inout_app)
{
  // Each releases its layers in its own graphics context
  multi_window_app_scene_window_cleanup(&(inout_app->scene_window));
  multi_window_app_controls_window_cleanup(&(inout_app->controls_window));

  // Cleans up both windows too, but leaves their memory, which is ours
  application_cleanup(&(inout_app->application));
}

/*******************************************************************************
 *
 *   Function: multi_window_app_add_windows()
 *
 ******************************************************************************/
static uint8_t multi_window_app_add_windows(
  struct multi_window_app_t* inout_app)
{
  uint8_t error =
    application_add_window(
      &(inout_app->application),
      &(inout_app->scene_window.window));

  error |=
    application_add_window(
      &(inout_app->application),
      &(inout_app->controls_window.window));

  return error;
}
