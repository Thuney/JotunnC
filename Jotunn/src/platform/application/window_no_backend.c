#include <stdio.h>

#include "window.h"

// Extern function definitions

/*******************************************************************************
 *
 * Function: window_graphics_init()
 *
 ******************************************************************************/
uint8_t window_graphics_init(
  struct window_t* window)
{
   #ifdef DEBUG
  printf("No graphics backend!\n");
   #endif

  window->metadata.signaled_close = 1;

  return 1;
}

/*******************************************************************************
 *
 * Function: window_graphics_poll_events()
 *
 ******************************************************************************/
void window_graphics_poll_events(
  struct window_t* window)
{
  return;
}

/*******************************************************************************
 *
 * Function: window_graphics_run()
 *
 ******************************************************************************/
void window_graphics_run(
  struct window_t* window)
{
  return;
}

/*******************************************************************************
 *
 * Function: window_graphics_cleanup()
 *
 ******************************************************************************/
void window_graphics_cleanup(
  struct window_t* window)
{
  return;
}

/*******************************************************************************
 *
 * Function: window_graphics_set_context()
 *
 ******************************************************************************/
void window_graphics_set_context(
  struct window_t* window)
{
  return;
}

/*******************************************************************************
 *
 * Function: window_graphics_release_context()
 *
 ******************************************************************************/
void window_graphics_release_context(
  struct window_t* window)
{
  return;
}

/*******************************************************************************
 *
 * Function: window_graphics_set_background_color()
 *
 ******************************************************************************/
void window_graphics_set_background_color(
  struct window_t* window,
  float red,
  float green,
  float blue,
  float alpha)
{
  return;
}
