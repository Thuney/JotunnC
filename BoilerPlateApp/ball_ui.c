#include "ball_ui.h"

#include "cimgui.h"

static void ball_custom_ui_layer_render(struct ui_layer_t* ui_layer);

/*************************************
 *
 * Function: ball_ui_layer_init()
 *
 *************************************/
void ball_ui_layer_init(
  struct window_t *parent_window,
  struct ball_ui_layer_t *ball_layer,
  uint16_t width,
  uint16_t height)
{
  ui_layer_init(parent_window, &(ball_layer->ui_layer), width, height);

  ui_layer_set_custom_render(
    &(ball_layer->ui_layer),
    &ball_custom_ui_layer_render);
}

/*************************************
 *
 * Function: ball_ui_layer_cleanup()
 *
 *************************************/
void ball_ui_layer_cleanup(struct ball_ui_layer_t *ball_layer)
{
  ui_layer_cleanup(&(ball_layer->ui_layer));
}

static void ball_custom_ui_layer_render(struct ui_layer_t* ui_layer)
{
  igBegin("Test", NULL, 0);
  igText("Test");
  igEnd();

  // // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway.
  // // Here we just want to make the demo initial state a bit more friendly!
  // igSetNextWindowPos((struct ImVec2){0,0}, ImGuiCond_FirstUseEver,(struct ImVec2){0,0} );
  igShowDemoWindow(NULL);
}
