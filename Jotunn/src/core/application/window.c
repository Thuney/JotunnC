/** File Include    **/
#include "window.h"

/** System Includes **/
#include <memory.h>
#ifdef DEBUG
  #include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>

/** Local Includes  **/
#include "allocator.h"
#include "application.h"
#include "framebuffer.h"
#include "fvector.h"
#include "log.h"
#include "render_api.h"
#include "window_layer.h"

#include "../diagnostics/diagnostics_internal.h"

#include <stdbool.h>

/************************************************************
 *
 * External implementation prototypes
 *  - Implemented per back-end
 *
 ************************************************************/
extern uint8_t window_graphics_init(struct window_t* window);
extern void window_graphics_poll_events(struct window_t* window);
extern void window_graphics_run(struct window_t* window);
extern void window_graphics_cleanup(struct window_t* window);
extern void window_graphics_set_context(struct window_t* window);
extern void window_graphics_release_context(struct window_t* window);
extern void window_graphics_set_viewport(
  uint32_t x,
  uint32_t y,
  uint32_t width,
  uint32_t height);
extern void window_graphics_set_background_color(
  struct window_t* window,
  const fvector4 color);
extern void window_graphics_set_cursor_shape(
  const struct window_t* in_window,
  const enum window_cursor_shape_t in_cursor_shape);
extern const char* window_graphics_get_clipboard_text(
  const struct window_t* in_window);
extern void window_graphics_set_clipboard_text(
  const struct window_t* in_window,
  const char* in_text);

// Renders a layer to its framebuffer, first resizing the framebuffer and
//  reprojecting the layer's camera to the window's size when asked to
//
//  in:     in_should_resize
//  in/out: inout_window, inout_layer
static void _window_render_layer(
  struct window_t* inout_window,
  struct window_layer_t* inout_layer,
  const bool in_should_resize);

/************************************************************
 *
 * Function: window_set_metadata()
 *
 ************************************************************/
static void window_set_metadata(
  struct window_data_t* metadata,
  const uint32_t width,
  const uint32_t height,
  const char* tag,
  struct application_t* app_parent,
  void (*function_event_notify)(
    struct application_t*,
    struct event_base_t*))
{
  // Store our window tag
  metadata->tag =
    allocator_duplicate_string(
      tag,
      ALLOCATOR_TAG_WINDOW);

  // Assign window properties
  metadata->width  = width;
  metadata->height = height;

  metadata->signaled_close = 0;
  metadata->visible = 1;

  metadata->parent_application    = app_parent;
  metadata->function_event_notify = function_event_notify;
}


/************************************************************
 *
 * Function: window_init()
 *
 ************************************************************/
uint8_t window_init(
  struct window_t* window,
  const uint8_t max_window_layers,
  const uint32_t width,
  const uint32_t height,
  const char* tag,
  struct application_t* app_parent)
{
  memset((void*)window, 0, sizeof(struct window_t));

  window_set_metadata(
    &(window->metadata),
    width,
    height,
    tag,
    app_parent,
    &application_on_event);

  const uint8_t error = window_graphics_init(window);

#ifdef DEBUG
  if (error) log_write(
    LOG_LEVEL_ERROR,
    "Error during window_graphics_init");
#endif

  if (!error)
  {
    window->function_event_react = 0;

    window_set_context(window);

    renderer_window_layer_init(
      &(window->window_layer_renderer),
      window,
      "Window Renderer");

    window->max_layers = max_window_layers;
    window->num_layers = 0;
    window->layers =
      (struct window_layer_t**)allocator_allocate(
        (window->max_layers * sizeof(struct window_layer_t*)),
        ALLOCATOR_TAG_WINDOW);
  }

  return error;
}

/************************************************************
 *
 * Function: window_add_layer()
 *
 ************************************************************/
uint8_t window_add_layer(
  struct window_t* window,
  struct window_layer_t* new_window_layer)
{
  #ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Adding layer to window");
  #endif

  uint8_t error = 0U;

  if (window->num_layers < window->max_layers)
  {
    struct window_layer_t** layer_ptr;
    layer_ptr = (window->layers + window->num_layers);
    *layer_ptr = new_window_layer;
    window->num_layers++;
  }
  else
  {
    error |= 1U;

    #ifdef DEBUG
    log_write(
      LOG_LEVEL_ERROR,
      "Error adding layer to window");
    #endif
  }

  return error;
}

/************************************************************
 *
 * Function: window_bind_custom_events()
 *
 ************************************************************/
void window_bind_custom_events(
  struct window_t* window,
  void (*custom_event_function)(
    struct window_t*,
    struct event_base_t*))
{
  window->function_event_react = custom_event_function;
}

/************************************************************
 *
 * Function: window_handle_event()
 *
 ************************************************************/
void window_handle_event(
  struct window_t* window,
  struct event_base_t* event)
{
  // The diagnostics overlay is drawn over every layer, so it sees events first
  const bool is_taken_by_overlay =
    diagnostics_internal_overlay_event_react(
      window,
      event);

  if (is_taken_by_overlay)
  {
    return;
  }

  // Layers see events top to bottom, so that one drawn over another (such as
  //  a UI) can claim an event before anything underneath it reacts
  for (uint8_t i = window->num_layers; i > 0; i--)
  {
    struct window_layer_t* current_layer = window->layers[i - 1];

    if (current_layer->function_event_react)
    {
      current_layer->function_event_react(
        window,
        current_layer,
        event);

      if (event->handled)
      {
        return;
      }
    }
  }

  if (window->function_event_react)
  {
    window->function_event_react(
      window,
      event);
  }
}

/************************************************************
 *
 * Function: window_run()
 *
 ************************************************************/
uint8_t window_run(
  struct window_t* window)
{
  // #ifdef DEBUG
  //     fprintf(stdout, "Running window\n");
  // #endif

  window_set_context(window);

  diagnostics_zone_begin(DIAGNOSTICS_INTERNAL_ZONE_EVENTS);
  window_graphics_poll_events(window);
  diagnostics_zone_end(DIAGNOSTICS_INTERNAL_ZONE_EVENTS);

  if (window->metadata.resized)
  {
    window_set_viewport(0, 0, window->metadata.width, window->metadata.height);
  }

  struct window_layer_t** current_layer;

  // The GPU's time on each part of the frame is marked as it's submitted
  diagnostics_internal_gpu_frame_begin(window);

  // Render our layers to their respective framebuffers, bottom to top
  diagnostics_zone_begin(DIAGNOSTICS_INTERNAL_ZONE_LAYERS);

  current_layer = window->layers;
  for (uint8_t i = 0; i < window->num_layers; i++, current_layer++)
  {
    const uint16_t layer_zone =
      diagnostics_internal_get_layer_zone(
        window,
        i);

    diagnostics_zone_begin(layer_zone);

    _window_render_layer(
      window,
      *current_layer,
      window->metadata.resized);

    diagnostics_zone_end(layer_zone);

    diagnostics_internal_gpu_mark_layer(
      window,
      i);
  }

  diagnostics_zone_end(DIAGNOSTICS_INTERNAL_ZONE_LAYERS);

  if (window->metadata.resized)
  {
    window->metadata.resized = 0;
  }

  // The diagnostics overlay, while shown, goes over every layer. It isn't
  //  rendered while hidden, so its size is checked rather than the window's
  //  resized flag
  diagnostics_zone_begin(DIAGNOSTICS_INTERNAL_ZONE_OVERLAY);

  struct window_layer_t* overlay_layer = diagnostics_internal_overlay_prepare(window);
  if (overlay_layer != NULL)
  {
    const struct texture_2d_t* overlay_texture =
      &(overlay_layer->framebuffer->color_buffer.texture);
    const bool is_overlay_resized =
      ((overlay_texture->width != window->metadata.width) ||
       (overlay_texture->height != window->metadata.height));

    _window_render_layer(
      window,
      overlay_layer,
      is_overlay_resized);

    diagnostics_internal_gpu_mark(
      window,
      "diagnostics overlay");
  }

  diagnostics_zone_end(DIAGNOSTICS_INTERNAL_ZONE_OVERLAY);

  // Render our layers to the default (window) framebuffer, bottom to top
  diagnostics_zone_begin(DIAGNOSTICS_INTERNAL_ZONE_COMPOSITE);

  renderer_window_layer_begin_scene(&(window->window_layer_renderer));

  current_layer = window->layers;
  for (uint8_t i = 0; i < window->num_layers; i++, current_layer++)
  {
    renderer_window_layer_draw_layer(
      &(window->window_layer_renderer), *current_layer, i);
  }

  if (overlay_layer != NULL)
  {
    renderer_window_layer_draw_layer(
      &(window->window_layer_renderer),
      overlay_layer,
      window->num_layers);
  }

  renderer_window_layer_end_scene(&(window->window_layer_renderer));

  diagnostics_zone_end(DIAGNOSTICS_INTERNAL_ZONE_COMPOSITE);

  diagnostics_internal_gpu_mark(
    window,
    "composite");
  diagnostics_internal_gpu_frame_end(window);

  diagnostics_zone_begin(DIAGNOSTICS_INTERNAL_ZONE_PRESENT);
  window_graphics_run(window);
  diagnostics_zone_end(DIAGNOSTICS_INTERNAL_ZONE_PRESENT);

  window_release_context(window);

  return (window->metadata.signaled_close);
}

/************************************************************
 *
 * Function: window_cleanup()
 *
 ************************************************************/
void window_cleanup(
  struct window_t* window)
{
  allocator_free(window->metadata.tag);
  window->metadata.tag = 0;

  diagnostics_internal_overlay_cleanup(window);

  // Releasing the window renderer's graphics resources needs the window's
  //  graphics context, which nothing else leaves current
  window_set_context(window);

  diagnostics_internal_gpu_cleanup(window);

  renderer_window_layer_cleanup(&(window->window_layer_renderer));

  window->max_layers = 0;
  window->num_layers = 0;
  allocator_free(window->layers);

  window_graphics_cleanup(window);
}

/************************************************************
 *
 * Function: window_set_context()
 *
 ************************************************************/
void window_set_context(
  struct window_t* window)
{
  window_graphics_set_context(window);
}

/************************************************************
 *
 * Function: window_release_context()
 *
 ************************************************************/
void window_release_context(
  struct window_t* window)
{
  window_graphics_release_context(window);
}

/************************************************************
 *
 * Function: window_set_viewport()
 *
 ************************************************************/
void window_set_viewport(
  uint32_t x,
  uint32_t y,
  uint32_t width,
  uint32_t height)
{
  window_graphics_set_viewport(x, y, width, height);
}

/************************************************************
 *
 * Function: window_set_background_color()
 *
 ************************************************************/
void window_set_background_color(
  struct window_t* window,
  const fvector4 color)
{
  window_graphics_set_background_color(window, color);
}

/*******************************************************************************
 *
 *   Function: window_set_cursor_shape()
 *
 ******************************************************************************/
void window_set_cursor_shape(
  const struct window_t* in_window,
  const enum window_cursor_shape_t in_cursor_shape)
{
  window_graphics_set_cursor_shape(
    in_window,
    in_cursor_shape);
}

/*******************************************************************************
 *
 *   Function: window_get_clipboard_text()
 *
 ******************************************************************************/
const char* window_get_clipboard_text(
  const struct window_t* in_window)
{
  return window_graphics_get_clipboard_text(in_window);
}

/*******************************************************************************
 *
 *   Function: window_set_clipboard_text()
 *
 ******************************************************************************/
void window_set_clipboard_text(
  const struct window_t* in_window,
  const char* in_text)
{
  window_graphics_set_clipboard_text(
    in_window,
    in_text);
}

/*******************************************************************************
 *
 *   Function: _window_render_layer()
 *
 ******************************************************************************/
static void _window_render_layer(
  struct window_t* inout_window,
  struct window_layer_t* inout_layer,
  const bool in_should_resize)
{
  framebuffer_bind(inout_layer->framebuffer);

  if (in_should_resize)
  {
    framebuffer_resize(
      inout_layer->framebuffer,
      inout_window->metadata.width,
      inout_window->metadata.height);

    inout_layer->camera->camera_reproject(
      inout_layer->camera,
      (float)inout_window->metadata.width,
      (float)inout_window->metadata.height,
      inout_layer->camera->near_plane,
      inout_layer->camera->far_plane);
  }

  renderer_base_begin_scene(inout_layer->renderer);

  if (inout_layer->function_custom_window_layer_run)
  {
    inout_layer->function_custom_window_layer_run(inout_layer);
  }

  renderer_base_end_scene(inout_layer->renderer);
}
