// A Jotunn application with one window whose only layer is a Dear ImGui
//  interface: a few widgets, a color picker driving the window's background
//  and an ImPlot plot, plus ImGui's own demo window on request.
//
// Reading order:
//  - main.c: the application's lifetime and main loop
//  - imgui_app.c: setting up the application, its window and its UI layer
//  - imgui_app_ui.c: the interface the UI layer draws each frame

#include "imgui_app.h"

#include <stdint.h>

static struct imgui_app_t imgui_app;

/*******************************************************************************
 *
 *   Function: main()
 *
 ******************************************************************************/
int main(void)
{
  struct imgui_app_t* app = &(imgui_app);

  const uint8_t error = imgui_app_init(app);

  // Runs frames until the window is closed
  if (!error) {
    application_run_until_closed(&(app->application));

    imgui_app_cleanup(app);
  }

  return error;
}
