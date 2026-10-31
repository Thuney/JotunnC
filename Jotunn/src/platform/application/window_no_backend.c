#include <stdio.h>

#include "window.h"
#include "log.h"

#include "../../core/application/window_internal.h"

// Stands in for a windowing backend when Jotunn is built without one: windows
//  fail to open, and the other hooks do nothing

/*******************************************************************************
 *
 *   BACKEND HOOKS
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *   Function: window_internal_platform_init()
 *
 ******************************************************************************/
uint8_t window_internal_platform_init(
  struct window_t* inout_window)
{
   #ifdef DEBUG
  log_write(
    LOG_LEVEL_ERROR,
    "No graphics backend!");
   #endif

  inout_window->metadata.should_close = 1;

  return 1;
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_poll_events()
 *
 ******************************************************************************/
void window_internal_platform_poll_events(
  struct window_t* inout_window)
{
  return;
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_run()
 *
 ******************************************************************************/
void window_internal_platform_run(
  const struct window_t* in_window)
{
  return;
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_cleanup()
 *
 ******************************************************************************/
void window_internal_platform_cleanup(
  const struct window_t* in_window)
{
  return;
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_set_context()
 *
 ******************************************************************************/
void window_internal_platform_set_context(
  const struct window_t* in_window)
{
  return;
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_release_context()
 *
 ******************************************************************************/
void window_internal_platform_release_context(
  const struct window_t* in_window)
{
  return;
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_set_viewport()
 *
 ******************************************************************************/
void window_internal_platform_set_viewport(
  uint32_t in_x_pixels,
  uint32_t in_y_pixels,
  uint32_t in_width_pixels,
  uint32_t in_height_pixels)
{
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_set_background_color()
 *
 ******************************************************************************/
void window_internal_platform_set_background_color(
  struct window_t* inout_window,
  union fvector4_t in_color)
{
  return;
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_set_cursor_shape()
 *
 ******************************************************************************/
void window_internal_platform_set_cursor_shape(
  const struct window_t* in_window,
  enum window_cursor_shape_t in_cursor_shape)
{
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_get_clipboard_text()
 *
 ******************************************************************************/
const char* window_internal_platform_get_clipboard_text(
  const struct window_t* in_window)
{
  return NULL;
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_set_clipboard_text()
 *
 ******************************************************************************/
void window_internal_platform_set_clipboard_text(
  const struct window_t* in_window,
  const char* in_text)
{
}
