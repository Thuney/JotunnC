#include "ball_ui.h"

#include "cimgui.h"

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Draws the UI each frame: a test window and the ImGui demo window
//
//  in/out: inout_ui_layer
static void ball_ui_layer_render(
  struct ui_layer_t* inout_ui_layer);

/*******************************************************************************
 *
 *   Function: ball_ui_layer_init()
 *
 ******************************************************************************/
void ball_ui_layer_init(
  struct window_t* inout_parent_window,
  struct ball_ui_layer_t* out_ball_layer,
  uint16_t in_width,
  uint16_t in_height)
{
  ui_layer_init(
    inout_parent_window,
    &(out_ball_layer->ui_layer),
    in_width,
    in_height);

  ui_layer_set_custom_render(
    &(out_ball_layer->ui_layer),
    &(ball_ui_layer_render));
}

/*******************************************************************************
 *
 *   Function: ball_ui_layer_cleanup()
 *
 ******************************************************************************/
void ball_ui_layer_cleanup(
  struct ball_ui_layer_t* inout_ball_layer)
{
  ui_layer_cleanup(&(inout_ball_layer->ui_layer));
}

/*******************************************************************************
 *
 *   Function: ball_ui_layer_render()
 *
 ******************************************************************************/
static void ball_ui_layer_render(
  struct ui_layer_t* inout_ui_layer)
{
  igBegin(
    "Test",
    NULL,
    0);
  igText("Test");
  igEnd();

  // // Normally user code doesn't need/want to call this because positions are
  //  saved in .ini file anyway.
  // // Here we just want to make the demo initial state a bit more friendly!
  // igSetNextWindowPos((struct ImVec2){0,0}, ImGuiCond_FirstUseEver,(struct
  //  ImVec2){0,0} );
  igShowDemoWindow(NULL);
}
