// A Jotunn application with two windows, each with two layers, sharing one
//  set of state:
//
//  - The scene window draws a ring of circles in a scene layer, with an
//    overlay layer above it. The overlay follows the mouse, and takes left
//    clicks for itself, dropping a marker where each lands
//  - The controls window draws a strip in the circles' color in a 2D layer,
//    under an ImGui layer whose controls change the scene window's circles
//
// Each window has its own graphics context, so each layer's framebuffer,
//  renderer and textures are made while its window's context is current.
//  Closing either window stops the application.
//
// Reading order:
//  - main.c: the application's lifetime and main loop
//  - multi_window_app.c: setting up the application and adding its windows
//  - multi_window_app_state.h: the state the windows share
//  - multi_window_app_scene_window.c: the scene window, its layers, and the
//    overlay's handling of the mouse
//  - multi_window_app_controls_window.c: the controls window and its layers

#include "multi_window_app.h"

#include <stdint.h>

static struct multi_window_app_t multi_window_app;

/*******************************************************************************
 *
 *   Function: main()
 *
 ******************************************************************************/
int main(void)
{
  struct multi_window_app_t* app = &(multi_window_app);

  const uint8_t error = multi_window_app_init(app);

  // Runs frames of every window until either is closed
  if (!error) {
    application_run_until_closed(&(app->application));

    multi_window_app_cleanup(app);
  }

  return error;
}
