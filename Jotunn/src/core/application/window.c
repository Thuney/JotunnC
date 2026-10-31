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
#include "window_internal.h"

#include <stdbool.h>

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Fills in a new window's metadata, copying in_tag
//
//  in:     in_width, in_height, in_tag, in_app_parent,
//            in_function_event_notify
//  out:    out_metadata
static void window_set_metadata(
  struct window_data_t* out_metadata,
  uint32_t in_width,
  uint32_t in_height,
  const char* in_tag,
  struct application_t* in_app_parent,
  void (*in_function_event_notify)(
    struct application_t* inout_application,
    struct event_base_t* inout_event));

// Renders a layer to its framebuffer, first resizing the framebuffer and
//  reprojecting the layer's camera to the window's size when asked to
//
//  in:     in_window, in_should_resize
//  in/out: inout_layer
static void _window_render_layer(
  const struct window_t* in_window,
  struct window_layer_t* inout_layer,
  bool in_should_resize);

/*******************************************************************************
 *
 *   Function: window_init()
 *
 ******************************************************************************/
uint8_t window_init(
  struct window_t* out_window,
  uint8_t in_max_window_layers,
  uint32_t in_width,
  uint32_t in_height,
  const char* in_tag,
  struct application_t* in_app_parent)
{
  memset(
    (void*)out_window,
    0,
    sizeof(struct window_t));

  window_set_metadata(
    &(out_window->metadata),
    in_width,
    in_height,
    in_tag,
    in_app_parent,
    &(application_on_event));

  const uint8_t error = window_internal_platform_init(out_window);

#ifdef DEBUG
  if (error) {
    log_write(
      LOG_LEVEL_ERROR,
      "Error during window_internal_platform_init");
  }
#endif

  if (!error) {
    out_window->function_event_react = 0;

    window_set_context(out_window);

    renderer_window_layer_init(
      &(out_window->window_layer_renderer),
      out_window,
      "Window Renderer");

    out_window->max_layers = in_max_window_layers;
    out_window->num_layers = 0;
    out_window->layers =
      (struct window_layer_t**)allocator_allocate(
        (out_window->max_layers * sizeof(struct window_layer_t*)),
        ALLOCATOR_TAG_WINDOW);
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: window_add_layer()
 *
 ******************************************************************************/
uint8_t window_add_layer(
  struct window_t* inout_window,
  struct window_layer_t* in_new_window_layer)
{
  #ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Adding layer to window");
  #endif

  uint8_t error = 0U;

  if (inout_window->num_layers < inout_window->max_layers) {
    struct window_layer_t** layer_ptr;
    layer_ptr = (inout_window->layers + inout_window->num_layers);
    *layer_ptr = in_new_window_layer;
    inout_window->num_layers++;
  }
  else {
    error |= 1U;

    #ifdef DEBUG
    log_write(
      LOG_LEVEL_ERROR,
      "Error adding layer to window");
    #endif
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: window_bind_custom_events()
 *
 ******************************************************************************/
void window_bind_custom_events(
  struct window_t* inout_window,
  void (*in_custom_event_function)(
    struct window_t* inout_window,
    struct event_base_t* inout_event))
{
  inout_window->function_event_react = in_custom_event_function;
}

/*******************************************************************************
 *
 *   Function: window_handle_event()
 *
 ******************************************************************************/
void window_handle_event(
  struct window_t* inout_window,
  struct event_base_t* inout_event)
{
  // The diagnostics overlay is drawn over every layer, so it sees events first
  const bool is_taken_by_overlay =
    diagnostics_internal_overlay_event_react(
      inout_window,
      inout_event);

  if (is_taken_by_overlay) {
    return;
  }

  // Layers see events top to bottom, so that one drawn over another (such as
  //  a UI) can claim an event before anything underneath it reacts
  for (uint8_t i = inout_window->num_layers; i > 0; i--) {
    struct window_layer_t* current_layer = inout_window->layers[i - 1];

    if (current_layer->function_event_react) {
      current_layer->function_event_react(
        inout_window,
        current_layer,
        inout_event);

      if (inout_event->is_handled) {
        return;
      }
    }
  }

  if (inout_window->function_event_react) {
    inout_window->function_event_react(
      inout_window,
      inout_event);
  }
}

/*******************************************************************************
 *
 *   Function: window_run()
 *
 ******************************************************************************/
uint8_t window_run(
  struct window_t* inout_window)
{
  // #ifdef DEBUG
  //     fprintf(stdout, "Running window\n");
  // #endif

  window_set_context(inout_window);

  diagnostics_zone_begin(DIAGNOSTICS_INTERNAL_ZONE_EVENTS);
  window_internal_platform_poll_events(inout_window);
  diagnostics_zone_end(DIAGNOSTICS_INTERNAL_ZONE_EVENTS);

  if (inout_window->metadata.is_resized) {
    window_set_viewport(
      0,
      0,
      inout_window->metadata.width,
      inout_window->metadata.height);
  }

  struct window_layer_t** current_layer;

  // The GPU's time on each part of the frame is marked as it's submitted
  diagnostics_internal_gpu_frame_begin(inout_window);

  // Render our layers to their respective framebuffers, bottom to top
  diagnostics_zone_begin(DIAGNOSTICS_INTERNAL_ZONE_LAYERS);

  current_layer = inout_window->layers;
  for (uint8_t i = 0; i < inout_window->num_layers; i++, current_layer++) {
    const uint16_t layer_zone =
      diagnostics_internal_get_layer_zone(
        inout_window,
        i);

    diagnostics_zone_begin(layer_zone);

    _window_render_layer(
      inout_window,
      *current_layer,
      inout_window->metadata.is_resized);

    diagnostics_zone_end(layer_zone);

    diagnostics_internal_gpu_mark_layer(
      inout_window,
      i);
  }

  diagnostics_zone_end(DIAGNOSTICS_INTERNAL_ZONE_LAYERS);

  if (inout_window->metadata.is_resized) {
    inout_window->metadata.is_resized = 0;
  }

  // The diagnostics overlay, while shown, goes over every layer. It isn't
  //  rendered while hidden, so its size is checked rather than the window's
  //  resized flag
  diagnostics_zone_begin(DIAGNOSTICS_INTERNAL_ZONE_OVERLAY);

  struct window_layer_t* overlay_layer = diagnostics_internal_overlay_prepare(inout_window);
  if (overlay_layer != NULL) {
    const struct texture_2d_t* overlay_texture =
      &(overlay_layer->framebuffer->color_buffer.texture);
    const bool is_overlay_resized =
      ((overlay_texture->width != inout_window->metadata.width) ||
       (overlay_texture->height != inout_window->metadata.height));

    _window_render_layer(
      inout_window,
      overlay_layer,
      is_overlay_resized);

    diagnostics_internal_gpu_mark(
      inout_window,
      "diagnostics overlay");
  }

  diagnostics_zone_end(DIAGNOSTICS_INTERNAL_ZONE_OVERLAY);

  // Render our layers to the default (window) framebuffer, bottom to top
  diagnostics_zone_begin(DIAGNOSTICS_INTERNAL_ZONE_COMPOSITE);

  renderer_window_layer_begin_scene(&(inout_window->window_layer_renderer.base));

  current_layer = inout_window->layers;
  for (uint8_t i = 0; i < inout_window->num_layers; i++, current_layer++) {
    renderer_window_layer_draw(
      &(inout_window->window_layer_renderer),
      *current_layer,
      i);
  }

  if (overlay_layer != NULL) {
    renderer_window_layer_draw(
      &(inout_window->window_layer_renderer),
      overlay_layer,
      inout_window->num_layers);
  }

  renderer_window_layer_end_scene(&(inout_window->window_layer_renderer.base));

  diagnostics_zone_end(DIAGNOSTICS_INTERNAL_ZONE_COMPOSITE);

  diagnostics_internal_gpu_mark(
    inout_window,
    "composite");
  diagnostics_internal_gpu_frame_end(inout_window);

  diagnostics_zone_begin(DIAGNOSTICS_INTERNAL_ZONE_PRESENT);
  window_internal_platform_run(inout_window);
  diagnostics_zone_end(DIAGNOSTICS_INTERNAL_ZONE_PRESENT);

  window_release_context(inout_window);

  return (inout_window->metadata.should_close);
}

/*******************************************************************************
 *
 *   Function: window_cleanup()
 *
 ******************************************************************************/
void window_cleanup(
  struct window_t* inout_window)
{
  allocator_free(inout_window->metadata.tag);
  inout_window->metadata.tag = 0;

  diagnostics_internal_overlay_cleanup(inout_window);

  // Releasing the window renderer's graphics resources needs the window's
  //  graphics context, which nothing else leaves current
  window_set_context(inout_window);

  diagnostics_internal_gpu_cleanup(inout_window);

  renderer_window_layer_cleanup(&(inout_window->window_layer_renderer));

  inout_window->max_layers = 0;
  inout_window->num_layers = 0;
  allocator_free(inout_window->layers);

  window_internal_platform_cleanup(inout_window);
}

/*******************************************************************************
 *
 *   Function: window_set_context()
 *
 ******************************************************************************/
void window_set_context(
  const struct window_t* in_window)
{
  window_internal_platform_set_context(in_window);
}

/*******************************************************************************
 *
 *   Function: window_release_context()
 *
 ******************************************************************************/
void window_release_context(
  const struct window_t* in_window)
{
  window_internal_platform_release_context(in_window);
}

/*******************************************************************************
 *
 *   Function: window_set_viewport()
 *
 ******************************************************************************/
void window_set_viewport(
  uint32_t in_x_pixels,
  uint32_t in_y_pixels,
  uint32_t in_width_pixels,
  uint32_t in_height_pixels)
{
  window_internal_platform_set_viewport(
    in_x_pixels,
    in_y_pixels,
    in_width_pixels,
    in_height_pixels);
}

/*******************************************************************************
 *
 *   Function: window_set_background_color()
 *
 ******************************************************************************/
void window_set_background_color(
  struct window_t* inout_window,
  union fvector4_t in_color)
{
  window_internal_platform_set_background_color(
    inout_window,
    in_color);
}

/*******************************************************************************
 *
 *   Function: window_set_cursor_shape()
 *
 ******************************************************************************/
void window_set_cursor_shape(
  const struct window_t* in_window,
  enum window_cursor_shape_t in_cursor_shape)
{
  window_internal_platform_set_cursor_shape(
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
  return window_internal_platform_get_clipboard_text(in_window);
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
  window_internal_platform_set_clipboard_text(
    in_window,
    in_text);
}

/*******************************************************************************
 *
 *   Function: window_set_metadata()
 *
 ******************************************************************************/
static void window_set_metadata(
  struct window_data_t* out_metadata,
  uint32_t in_width,
  uint32_t in_height,
  const char* in_tag,
  struct application_t* in_app_parent,
  void (*in_function_event_notify)(
    struct application_t* inout_application,
    struct event_base_t* inout_event))
{
  // Store our window tag
  out_metadata->tag =
    allocator_duplicate_string(
      in_tag,
      ALLOCATOR_TAG_WINDOW);

  // Assign window properties
  out_metadata->width  = in_width;
  out_metadata->height = in_height;

  out_metadata->should_close = 0;
  out_metadata->is_visible = 1;

  out_metadata->parent_application    = in_app_parent;
  out_metadata->function_event_notify = in_function_event_notify;
}

/*******************************************************************************
 *
 *   Function: _window_render_layer()
 *
 ******************************************************************************/
static void _window_render_layer(
  const struct window_t* in_window,
  struct window_layer_t* inout_layer,
  bool in_should_resize)
{
  framebuffer_bind(inout_layer->framebuffer);

  if (in_should_resize) {
    framebuffer_resize(
      inout_layer->framebuffer,
      in_window->metadata.width,
      in_window->metadata.height);

    inout_layer->camera->function_reproject(
      inout_layer->camera,
      (float)in_window->metadata.width,
      (float)in_window->metadata.height,
      inout_layer->camera->near_plane,
      inout_layer->camera->far_plane);
  }

  renderer_base_begin_scene(inout_layer->renderer);

  if (inout_layer->function_custom_window_layer_run) {
    inout_layer->function_custom_window_layer_run(inout_layer);
  }

  renderer_base_end_scene(inout_layer->renderer);
}
