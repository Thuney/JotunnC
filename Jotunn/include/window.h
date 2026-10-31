#pragma once

// A window is a platform window with its own graphics context. Each frame it
//  renders its layers, each to its own framebuffer, then composites them onto
//  the window. The platform half lives in a backend (window_gl.c with GLFW
//  and OpenGL)

/** System Includes **/
#include <stdint.h>

/** Local Includes  **/
#include "renderer_window_layer.h"

/** Forward Declarations **/
struct application_t;
struct camera_base_t;
struct diagnostics_overlay_t;
struct event_base_t;
struct renderer_base_t;
struct window_t;

/*******************************************************************************
 *
 *   CURSOR SHAPES
 *
 ******************************************************************************/

// Mouse cursor shapes a window can show while the cursor is over it
enum window_cursor_shape_t
{
  WINDOW_CURSOR_SHAPE_HIDDEN = 0x00,
  WINDOW_CURSOR_SHAPE_ARROW,
  WINDOW_CURSOR_SHAPE_TEXT_INPUT,
  WINDOW_CURSOR_SHAPE_RESIZE_ALL,
  WINDOW_CURSOR_SHAPE_RESIZE_NS,
  WINDOW_CURSOR_SHAPE_RESIZE_EW,
  WINDOW_CURSOR_SHAPE_RESIZE_NESW,
  WINDOW_CURSOR_SHAPE_RESIZE_NWSE,
  WINDOW_CURSOR_SHAPE_HAND,
  WINDOW_CURSOR_SHAPE_WAIT,
  WINDOW_CURSOR_SHAPE_PROGRESS,
  WINDOW_CURSOR_SHAPE_NOT_ALLOWED,
  WINDOW_CURSOR_SHAPE_COUNT
};

/*******************************************************************************
 *
 *   WINDOW
 *
 ******************************************************************************/

// The state a window's backend reads and updates as the window's events
//  arrive. The backend finds it by address, so it must not move while the
//  window is open
struct window_data_t
{
  // Names the window, and titles it
  char* tag;
  //
  uint32_t width;
  uint32_t height;
  //
  // Set when the window's size changes, until the next frame has resized the
  //  viewport and the layers
  uint8_t is_resized;
  // Set when the window has been asked to close
  uint8_t should_close;
  // Only visible windows are run
  uint8_t is_visible;
  //
  struct application_t* parent_application;
  // Where the backend sends the window's events; application_on_event()
  void (*function_event_notify)(
    struct application_t* inout_application,
    struct event_base_t* inout_event);
};

// The backend's handle on the window; a GLFWwindow* with the GL backend
struct window_context_data_t
{
  void* window_handle;
};

// A window, the layers drawn in it and the renderer that composites them
struct window_t
{
  struct window_data_t metadata;
  struct window_context_data_t context_data;

  struct window_layer_t** layers;
  uint8_t num_layers;
  uint8_t max_layers;

  struct renderer_window_layer_t window_layer_renderer;
  // Created when diagnostics are first shown in the window, and only in builds
  //  with JOTUNN_DIAGNOSTICS; otherwise NULL
  struct diagnostics_overlay_t* diagnostics_overlay;
  // Sees the events no layer has handled; see window_bind_custom_events()
  void (*function_event_react)(
    struct window_t* inout_window,
    struct event_base_t* inout_event);
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Opens a window of the given size, titled and tagged in_tag, whose events go
//  to in_app_parent, with room for in_max_window_layers layers. On success
//  the window's graphics context is left current. The window keeps its own
//  copy of in_tag, and must stay where it is until window_cleanup(), since
//  its backend finds it by address
//
//  in:     in_max_window_layers, in_width, in_height, in_tag, in_app_parent
//  out:    out_window
//  ret:    0 on success; 1 when the backend couldn't open the window
uint8_t window_init(
  struct window_t* out_window,
  const uint8_t in_max_window_layers,
  const uint32_t in_width,
  const uint32_t in_height,
  const char* in_tag,
  struct application_t* in_app_parent);

// Adds a layer to the window, over those already added. The window keeps the
//  pointer but doesn't own the layer
//
//  in:     in_new_window_layer
//  in/out: inout_window
//  ret:    0 on success; 1 when the window already has as many layers as it
//            has room for
uint8_t window_add_layer(
  struct window_t* inout_window,
  struct window_layer_t* in_new_window_layer);

// Not defined: no backend implements it yet
//
//  in/out: inout_window
void window_show(
  struct window_t* inout_window);

// Not defined: no backend implements it yet
//
//  in/out: inout_window
void window_hide(
  struct window_t* inout_window);

// Runs a frame of the window: handles pending events, renders each layer to
//  its framebuffer, bottom to top, then the diagnostics overlay if it's
//  shown, composites them onto the window and presents the result. Layers
//  are resized to the window first when its size has changed. Leaves no
//  graphics context current
//
//  in/out: inout_window
//  ret:    Nonzero when the window has been asked to close; otherwise 0
uint8_t window_run(
  struct window_t* inout_window);

// Closes the window and releases what window_init() allocated, along with
//  the window's diagnostics. Its layers, which the window doesn't own, are
//  left alone. With the GL backend this also shuts GLFW down, closing any
//  other window still open
//
//  in/out: inout_window
void window_cleanup(
  struct window_t* inout_window);

/*******************************************************************************
 *
 *   EVENTS
 *
 ******************************************************************************/

// Binds the handler that sees the window's events once its layers have
//  passed on them; NULL unbinds it
//
//  in:     in_custom_event_function
//  in/out: inout_window
void window_bind_custom_events(
  struct window_t* inout_window,
  void (*in_custom_event_function)(
    struct window_t* inout_window,
    struct event_base_t* inout_event));

// Offers an event to the diagnostics overlay, then to the window's layers
//  from top to bottom, then to the window's custom handler, stopping at the
//  first to take it
//
//  in/out: inout_window, inout_event
void window_handle_event(
  struct window_t* inout_window,
  struct event_base_t* inout_event);

/*******************************************************************************
 *
 *   GRAPHICS CONTEXT
 *
 ******************************************************************************/

// Makes the window's graphics context current on the calling thread
//
//  in:     in_window
void window_set_context(
  const struct window_t* in_window);

// Leaves the calling thread with no current graphics context, whichever
//  window's it was
//
//  in:     in_window
void window_release_context(
  const struct window_t* in_window);

// Sets the rectangle of the current context's render target that drawing
//  maps to, in pixels with the origin at the bottom-left corner
//
//  in:     in_x_pixels, in_y_pixels, in_width_pixels, in_height_pixels
void window_set_viewport(
  const uint32_t in_x_pixels,
  const uint32_t in_y_pixels,
  const uint32_t in_width_pixels,
  const uint32_t in_height_pixels);

// Sets the color the window is cleared to each frame, showing where no layer
//  covers it
//
//  in:     in_color
//  in/out: inout_window
void window_set_background_color(
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
void window_set_cursor_shape(
  const struct window_t* in_window,
  const enum window_cursor_shape_t in_cursor_shape);

// Reads the system clipboard as UTF-8 text
//
//  in:     in_window
//  ret:    The clipboard's text, valid until the clipboard next changes or is
//            read; NULL when it holds no text
const char* window_get_clipboard_text(
  const struct window_t* in_window);

// Replaces the system clipboard's contents with UTF-8 text
//
//  in:     in_window, in_text
void window_set_clipboard_text(
  const struct window_t* in_window,
  const char* in_text);
