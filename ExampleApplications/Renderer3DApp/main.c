// A Jotunn application drawing a lit 3D scene with renderer_3d, through a
//  perspective camera orbiting the scene's center: drag with the left mouse
//  button to orbit, and scroll to move in and out. The scene is a grid on the
//  ground with the axes through it, a textured cube tumbling inside a
//  wireframe cage, and a ring of cubes, pyramids and spheres.
//
// Reading order:
//  - main.c: the application's lifetime and main loop
//  - renderer_3d_app.c: setting up the application, its window, the 3D
//    layer the scene is drawn in and the orbit its camera rides, and feeding
//    the window's mouse events to the orbit
//  - renderer_3d_app_scene.c: the scene the layer draws each frame

#include "renderer_3d_app.h"

#include <stdint.h>

static struct renderer_3d_app_t renderer_3d_app;

/*******************************************************************************
 *
 *   Function: main()
 *
 ******************************************************************************/
int main(void)
{
  struct renderer_3d_app_t* app = &(renderer_3d_app);

  const uint8_t error = renderer_3d_app_init(app);

  // Runs frames until the window is closed
  if (!error) {
    application_run_until_closed(&(app->application));

    renderer_3d_app_cleanup(app);
  }

  return error;
}
