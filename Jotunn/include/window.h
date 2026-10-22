#pragma once

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

/************************************************************
 *
 * Enum Definitions
 *
 ************************************************************/

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

/************************************************************
 *
 * Struct Definitions
 *
 ************************************************************/
struct window_data_t
{
  char* tag;
  //
  uint32_t width;
  uint32_t height;
  //
  uint8_t resized;
  uint8_t signaled_close;
  uint8_t visible;
  //
  struct application_t* parent_application;
  void (*function_event_notify)(
    struct application_t* application,
    struct event_base_t* event);
};

struct window_context_data_t
{
  void* window_handle;
};

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
  //
  void (*function_event_react)(
    struct window_t* window,
    struct event_base_t* event);
};

/************************************************************
 *
 * Interface Functions
 *
 ************************************************************/
uint8_t window_init(
  struct window_t* window,
  const uint8_t max_window_layers,
  const uint32_t width,
  const uint32_t height,
  const char* tag,
  struct application_t* app_parent);

uint8_t window_add_layer(
  struct window_t* window,
  struct window_layer_t* new_window_layer);

void window_bind_custom_events(
  struct window_t* window,
  void (*custom_event_function)(
    struct window_t*,
    struct event_base_t*));
void window_handle_event(
  struct window_t* window,
  struct event_base_t* event);

void window_show(struct window_t* window);
void window_hide(struct window_t* window);

uint8_t window_run(struct window_t* window);

void window_cleanup(struct window_t* window);

void window_set_context(struct window_t* window);
void window_release_context(struct window_t* window);

void window_set_viewport(
  uint32_t x,
  uint32_t y,
  uint32_t width,
  uint32_t height);

void window_set_background_color(
  struct window_t* window,
  const fvector4 color);

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
