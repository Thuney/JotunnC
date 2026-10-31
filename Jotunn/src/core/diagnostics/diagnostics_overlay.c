#include "diagnostics_internal.h"

#include "allocator.h"
#include "event.h"
#include "renderer.h"
#include "resource.h"
#include "ui_layer.h"
#include "util.h"
#include "window.h"
#include "window_layer.h"

#include "cimgui.h"

#include <stddef.h>

// Shows and hides the overlay in the window it's pressed in
#define _DIAGNOSTICS_OVERLAY_TOGGLE_KEY EVENT_KEY_CODE_F3

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// A window's diagnostics overlay. It's allocated the first time it's asked
//  for, but its layer, which holds a window-sized framebuffer, only once it's
//  first rendered, when the window's graphics context is sure to be current
struct diagnostics_overlay_t
{
  // First, so the layer's render callback can find its overlay
  struct ui_layer_t ui_layer;
  bool is_layer_created;
  bool is_visible;
};

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Creates the overlay's UI layer at the window's size
//
//  in/out: inout_window, inout_overlay
static void _diagnostics_overlay_create_layer(
  struct window_t* inout_window,
  struct diagnostics_overlay_t* inout_overlay);

// Draws the diagnostics panel as the overlay's UI, hiding the overlay when the
//  panel is closed
//
//  in/out: inout_ui_layer
static void _diagnostics_overlay_render(
  struct ui_layer_t* inout_ui_layer);

/*******************************************************************************
 *
 *   Function: diagnostics_internal_overlay_event_react()
 *
 ******************************************************************************/
bool diagnostics_internal_overlay_event_react(
  struct window_t* inout_window,
  struct event_base_t* inout_event)
{
  if (inout_event->event_type == EVENT_TYPE_KEY_PRESSED) {
    const struct event_key_pressed_t* key_event =
      container_of(
        inout_event,
        struct event_key_pressed_t,
        base);

    if ((key_event->key == _DIAGNOSTICS_OVERLAY_TOGGLE_KEY) &&
        !key_event->is_repeat &&
        (key_event->window_handle == inout_window->context_data.window_handle))
    {
      if (inout_window->diagnostics_overlay == NULL) {
        inout_window->diagnostics_overlay =
          (struct diagnostics_overlay_t*)allocator_allocate_zeroed(
            1,
            sizeof(struct diagnostics_overlay_t),
            ALLOCATOR_TAG_DIAGNOSTICS);
      }

      if (inout_window->diagnostics_overlay == NULL) {
        return false;
      }

      inout_window->diagnostics_overlay->is_visible =
        !inout_window->diagnostics_overlay->is_visible;
      inout_event->is_handled = 1U;

      return true;
    }
  }

  struct diagnostics_overlay_t* overlay = inout_window->diagnostics_overlay;

  if ((overlay == NULL) ||
      !overlay->is_visible ||
      !overlay->is_layer_created)
  {
    return false;
  }

  // The UI layer marks the input its UI captures as handled
  const bool was_handled = (inout_event->is_handled != 0U);
  struct window_layer_t* layer = &(overlay->ui_layer.ui_window_layer);

  layer->function_event_react(
    inout_window,
    layer,
    inout_event);

  return (!was_handled && (inout_event->is_handled != 0U));
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_overlay_prepare()
 *
 ******************************************************************************/
struct window_layer_t* diagnostics_internal_overlay_prepare(
  struct window_t* inout_window)
{
  struct diagnostics_overlay_t* overlay = inout_window->diagnostics_overlay;

  if ((overlay == NULL) ||
      !overlay->is_visible)
  {
    return NULL;
  }

  if (!overlay->is_layer_created) {
    _diagnostics_overlay_create_layer(
      inout_window,
      overlay);
  }

  return &(overlay->ui_layer.ui_window_layer);
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_overlay_cleanup()
 *
 ******************************************************************************/
void diagnostics_internal_overlay_cleanup(
  struct window_t* inout_window)
{
  struct diagnostics_overlay_t* overlay = inout_window->diagnostics_overlay;

  if (overlay == NULL) {
    return;
  }

  if (overlay->is_layer_created) {
    ui_layer_cleanup(&(overlay->ui_layer));
  }

  allocator_free(overlay);
  inout_window->diagnostics_overlay = NULL;
}

/*******************************************************************************
 *
 *   Function: _diagnostics_overlay_create_layer()
 *
 ******************************************************************************/
static void _diagnostics_overlay_create_layer(
  struct window_t* inout_window,
  struct diagnostics_overlay_t* inout_overlay)
{
  ui_layer_init(
    inout_window,
    &(inout_overlay->ui_layer),
    inout_window->metadata.width,
    inout_window->metadata.height);
  ui_layer_set_custom_render(
    &(inout_overlay->ui_layer),
    &(_diagnostics_overlay_render));

  // ui_layer_init() leaves the layer's new ImGui context current. The overlay
  //  keeps no settings file, so the application's imgui.ini stays its own
  igGetIO_Nil()->IniFilename = NULL;

  resource_set_name(
    &(inout_overlay->ui_layer.ui_framebuffer.resource),
    "diagnostics overlay");

  // Every UI layer's renderer shares one tag, so the overlay's is retagged to
  //  tell its draws apart from the application's UI
  inout_overlay->ui_layer.ui_renderer_2d.base.tag = "diagnostics overlay";

  inout_overlay->is_layer_created = true;
}

/*******************************************************************************
 *
 *   Function: _diagnostics_overlay_render()
 *
 ******************************************************************************/
static void _diagnostics_overlay_render(
  struct ui_layer_t* inout_ui_layer)
{
  struct diagnostics_overlay_t* overlay =
    container_of(
      inout_ui_layer,
      struct diagnostics_overlay_t,
      ui_layer);

  bool is_open = true;

  diagnostics_internal_panel_draw(
    &(is_open),
    &(overlay->ui_layer.ui_framebuffer.color_buffer.texture));

  if (!is_open) {
    overlay->is_visible = false;
  }
}
