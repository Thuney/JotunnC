// A Jotunn application drawing a 2D scene with renderer_2d: each of its
//  primitives, a pair spinning with the application's clock, a texture loaded
//  from a file, part of that texture, a texture made in code, and text.
//
// Reading order:
//  - main.c: the application's lifetime and main loop
//  - renderer_2d_app.c: setting up the application, its window, and the
//    2D layer the scene is drawn in
//  - renderer_2d_app_resources.c: loading and making the textures and
//    typeface
//  - renderer_2d_app_scene.c: the scene the layer draws each frame

#include "renderer_2d_app.h"

#include <stdint.h>

static struct renderer_2d_app_t renderer_2d_app;

/*******************************************************************************
 *
 *   Function: main()
 *
 ******************************************************************************/
int main(void)
{
  struct renderer_2d_app_t* app = &(renderer_2d_app);

  const uint8_t error = renderer_2d_app_init(app);

  // Runs frames until the window is closed
  if (!error) {
    application_run_until_closed(&(app->application));

    renderer_2d_app_cleanup(app);
  }

  return error;
}
