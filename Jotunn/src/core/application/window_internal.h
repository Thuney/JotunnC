#pragma once

// Private to Jotunn: the hooks through which window.c drives the windowing
//  backend. Each backend defines all of them: window_gl.c with GLFW and
//  OpenGL, and window_no_backend.c, whose windows fail to open, when Jotunn is
//  built without a graphics backend

#include "fvector.h"
#include "window.h"

#include <stdint.h>

/*******************************************************************************
 *
 *   WINDOW LIFETIME
 *
 ******************************************************************************/

// Opens the window's platform window, sized and titled from its metadata, and
//  makes its graphics context current with the viewport covering it. The
//  first window opened also initializes the backend's libraries. The window's
//  metadata must stay where it is while the window is open, since the
//  backend's event callbacks find it by address
//
//  in/out: inout_window
//  ret:    0 on success; 1 when the backend's libraries couldn't be
//            initialized, or there is no backend
uint8_t window_internal_platform_init(
  struct window_t* inout_window);

// Handles the windowing system's pending events, which may include other
//  windows' as well as this one's, passing each to the function_event_notify
//  of the window it's for. Windows' sizes and close flags may change as their
//  events are handled
//
//  in/out: inout_window
void window_internal_platform_poll_events(
  struct window_t* inout_window);

// Presents the frame drawn to the window's default framebuffer
//
//  in:     in_window
void window_internal_platform_run(
  const struct window_t* in_window);

// Closes the window's platform window. The GL backend also shuts GLFW down,
//  which closes any other window still open
//
//  in:     in_window
void window_internal_platform_cleanup(
  const struct window_t* in_window);

/*******************************************************************************
 *
 *   GRAPHICS CONTEXT
 *
 ******************************************************************************/

// Makes the window's graphics context current on the calling thread
//
//  in:     in_window
void window_internal_platform_set_context(
  const struct window_t* in_window);

// Leaves the calling thread with no current graphics context, whichever
//  window's it was
//
//  in:     in_window
void window_internal_platform_release_context(
  const struct window_t* in_window);

// Sets the rectangle of the current context's render target that drawing
//  maps to, in pixels with the origin at the bottom-left corner
//
//  in:     in_x_pixels, in_y_pixels, in_width_pixels, in_height_pixels
void window_internal_platform_set_viewport(
  const uint32_t in_x_pixels,
  const uint32_t in_y_pixels,
  const uint32_t in_width_pixels,
  const uint32_t in_height_pixels);

// Sets the color the window's default framebuffer is cleared to each frame
//
//  in:     in_color
//  in/out: inout_window
void window_internal_platform_set_background_color(
  struct window_t* inout_window,
  const union fvector4_t in_color);

/*******************************************************************************
 *
 *   CURSOR AND CLIPBOARD
 *
 ******************************************************************************/

// Changes the cursor shown while the cursor is over the window. Shapes the
//  platform lacks fall back to the arrow
//
//  in:     in_window, in_cursor_shape
void window_internal_platform_set_cursor_shape(
  const struct window_t* in_window,
  const enum window_cursor_shape_t in_cursor_shape);

// Reads the system clipboard as UTF-8 text
//
//  in:     in_window
//  ret:    The clipboard's text, valid until the clipboard next changes or is
//            read; NULL when it holds no text
const char* window_internal_platform_get_clipboard_text(
  const struct window_t* in_window);

// Replaces the system clipboard's contents with UTF-8 text
//
//  in:     in_window, in_text
void window_internal_platform_set_clipboard_text(
  const struct window_t* in_window,
  const char* in_text);
