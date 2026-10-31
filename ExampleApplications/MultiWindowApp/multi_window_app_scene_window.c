#include "multi_window_app_scene_window.h"

#include "color.h"
#include "fmath.h"
#include "util.h"

#include <math.h>
#include <stdbool.h>

#define MULTI_WINDOW_APP_SCENE_WINDOW_TYPEFACE_SIZE_PIXELS 18

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Sets up the scene and overlay layers and adds them to the window, bottom
//  first, then gives the overlay its event handler and Jotunn's typeface. The
//  window's graphics context must be current
//
//  in/out: inout_scene_window
//  ret:    0 on success; non-zero when a layer can't be set up or added
static uint8_t multi_window_app_scene_window_create_layers(
  struct multi_window_app_scene_window_t* inout_scene_window);

// Draws the ring of circles, turning with the application's clock. The
//  scene layer's custom run
//
//  in/out: inout_window_layer
static void multi_window_app_scene_window_run_scene(
  struct window_layer_t* inout_window_layer);

// Draws the markers, crosshairs at the mouse while it's over the window, and
//  a hint. The overlay layer's custom run
//
//  in/out: inout_window_layer
static void multi_window_app_scene_window_run_overlay(
  struct window_layer_t* inout_window_layer);

// Draws a small circle at each marker
//
//  in/out: inout_scene_window
static void multi_window_app_scene_window_draw_markers(
  struct multi_window_app_scene_window_t* inout_scene_window);

// Draws lines across the window through the mouse, while it's over the window
//
//  in/out: inout_scene_window
static void multi_window_app_scene_window_draw_crosshair(
  struct multi_window_app_scene_window_t* inout_scene_window);

// Draws a line of text along the top of the window, saying what the overlay
//  does
//
//  in/out: inout_scene_window
static void multi_window_app_scene_window_draw_hint(
  struct multi_window_app_scene_window_t* inout_scene_window);

// Hands each event reaching the overlay to the handler for its type. The
//  overlay layer's event handler; an event it marks handled goes no further
//
//  in/out: inout_parent_window, inout_window_layer, inout_event
static void multi_window_app_scene_window_overlay_on_event(
  struct window_t* inout_parent_window,
  struct window_layer_t* inout_window_layer,
  struct event_base_t* inout_event);

// Records where the mouse is, when it moved over this window
//
//  in:     in_mouse_moved_event
//  in/out: inout_scene_window
static void multi_window_app_scene_window_on_mouse_moved(
  struct multi_window_app_scene_window_t* inout_scene_window,
  const struct event_mouse_moved_t* in_mouse_moved_event);

// Records whether the mouse is over this window, when it crossed this
//  window's edge
//
//  in:     in_mouse_entered_event
//  in/out: inout_scene_window
static void multi_window_app_scene_window_on_mouse_entered(
  struct multi_window_app_scene_window_t* inout_scene_window,
  const struct event_mouse_entered_t* in_mouse_entered_event);

// Drops a marker where the mouse is when the left button is pressed over
//  this window, and takes the press, so the scene layer and the window never
//  see it
//
//  in/out: inout_scene_window, inout_mouse_button_event
static void multi_window_app_scene_window_on_mouse_button(
  struct multi_window_app_scene_window_t* inout_scene_window,
  struct event_mouse_button_t* inout_mouse_button_event);

/*******************************************************************************
 *
 *   Function: multi_window_app_scene_window_init()
 *
 ******************************************************************************/
uint8_t multi_window_app_scene_window_init(
  struct multi_window_app_scene_window_t* out_scene_window,
  struct application_t* in_application,
  struct multi_window_app_state_t* inout_state)
{
  const uint8_t max_window_layers = 2;

  out_scene_window->state = inout_state;

  // The engine allocates the list of layers, being given NULL storage.
  //  window_init() leaves the window's graphics context current, for its
  //  layers to be made in
  uint8_t error =
    window_init(
      &(out_scene_window->window),
      NULL,
      max_window_layers,
      MULTI_WINDOW_APP_SCENE_WINDOW_WIDTH_PIXELS,
      MULTI_WINDOW_APP_SCENE_WINDOW_HEIGHT_PIXELS,
      "Jotunn - Multi-Window Example: Scene",
      in_application);

  if (!error) {
    error = multi_window_app_scene_window_create_layers(out_scene_window);
  }

  if (!error) {
    window_set_background_color(
      &(out_scene_window->window),
      (union fvector4_t){{0.1f, 0.1f, 0.12f, 1.0f}});
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: multi_window_app_scene_window_cleanup()
 *
 ******************************************************************************/
void multi_window_app_scene_window_cleanup(
  struct multi_window_app_scene_window_t* inout_scene_window)
{
  window_set_context(&(inout_scene_window->window));

  font_typeface_cleanup(&(inout_scene_window->overlay_typeface));
  window_layer_2d_cleanup(&(inout_scene_window->overlay_layer));
  window_layer_2d_cleanup(&(inout_scene_window->scene_layer));
}

/*******************************************************************************
 *
 *   Function: multi_window_app_scene_window_create_layers()
 *
 ******************************************************************************/
static uint8_t multi_window_app_scene_window_create_layers(
  struct multi_window_app_scene_window_t* inout_scene_window)
{
  struct window_t* window = &(inout_scene_window->window);

  // Each covers the window, and has its renderer's batch storage allocated
  //  by the engine, being given NULL
  uint8_t error =
    window_layer_2d_init(
      window,
      &(inout_scene_window->scene_layer),
      NULL,
      "multi_window_app_scene_renderer",
      &(multi_window_app_scene_window_run_scene));

  if (!error) {
    error =
      window_layer_2d_init(
        window,
        &(inout_scene_window->overlay_layer),
        NULL,
        "multi_window_app_overlay_renderer",
        &(multi_window_app_scene_window_run_overlay));
  }

  if (error) {
    return error;
  }

  // Drawn in the order they're added, so the overlay goes over the scene,
  //  and offered events in the reverse order
  error |=
    window_add_layer(
      window,
      &(inout_scene_window->scene_layer.window_layer));
  error |=
    window_add_layer(
      window,
      &(inout_scene_window->overlay_layer.window_layer));

  window_layer_set_event_react(
    &(inout_scene_window->overlay_layer.window_layer),
    &(multi_window_app_scene_window_overlay_on_event));

  font_typeface_init(
    &(inout_scene_window->overlay_typeface),
    FONT_DEFAULT_TYPEFACE_PATH,
    MULTI_WINDOW_APP_SCENE_WINDOW_TYPEFACE_SIZE_PIXELS);

  return error;
}

/*******************************************************************************
 *
 *   Function: multi_window_app_scene_window_run_scene()
 *
 ******************************************************************************/
static void multi_window_app_scene_window_run_scene(
  struct window_layer_t* inout_window_layer)
{
  struct multi_window_app_scene_window_t* scene_window =
    container_of(
      inout_window_layer,
      struct multi_window_app_scene_window_t,
      scene_layer.window_layer);
  const struct multi_window_app_state_t* state = scene_window->state;

  const float ring_radius_pixels = 180.0f;
  const float circle_radius_pixels = 22.0f;

  const union fvector2_t center_pixels =
    {{((float)scene_window->window.metadata.width / 2.0f),
      ((float)scene_window->window.metadata.height / 2.0f)}};
  const union fvector4_t color =
    color_from_rgb(
      state->circle_color,
      1.0f);
  const float elapsed_seconds =
    (float)application_get_elapsed_seconds(scene_window->window.metadata.parent_application);

  for (int i = 0; i < state->circle_count; i++) {
    const float angle_radians =
      (((2.0f * FMATH_PI * (float)i) / (float)state->circle_count) +
       (state->orbit_speed_radians_per_second * elapsed_seconds));

    // The unit circle, scaled by the radius wanted and moved by its center
    const union fmatrix_4x4_t transform =
      fmatrix_4x4_transform_compose_2d(
        (union fvector2_t){{0.0f, 0.0f}},
        (union fvector2_t){{circle_radius_pixels, circle_radius_pixels}},
        0.0f,
        (union fvector2_t){{(center_pixels.comp.x + (ring_radius_pixels * cosf(angle_radians))),
                            (center_pixels.comp.y + (ring_radius_pixels * sinf(angle_radians)))}});

    renderer_2d_draw_circle(
      &(scene_window->scene_layer.renderer_2d),
      &(transform),
      color);
  }
}

/*******************************************************************************
 *
 *   Function: multi_window_app_scene_window_run_overlay()
 *
 ******************************************************************************/
static void multi_window_app_scene_window_run_overlay(
  struct window_layer_t* inout_window_layer)
{
  struct multi_window_app_scene_window_t* scene_window =
    container_of(
      inout_window_layer,
      struct multi_window_app_scene_window_t,
      overlay_layer.window_layer);

  multi_window_app_scene_window_draw_markers(scene_window);
  multi_window_app_scene_window_draw_crosshair(scene_window);
  multi_window_app_scene_window_draw_hint(scene_window);
}

/*******************************************************************************
 *
 *   Function: multi_window_app_scene_window_draw_markers()
 *
 ******************************************************************************/
static void multi_window_app_scene_window_draw_markers(
  struct multi_window_app_scene_window_t* inout_scene_window)
{
  const struct multi_window_app_state_t* state = inout_scene_window->state;

  const float marker_radius_pixels = 6.0f;
  const union fvector4_t marker_color = {{0.95f, 0.45f, 0.35f, 1.0f}};

  for (unsigned int i = 0; i < state->marker_count; i++) {
    const union fmatrix_4x4_t transform =
      fmatrix_4x4_transform_compose_2d(
        (union fvector2_t){{0.0f, 0.0f}},
        (union fvector2_t){{marker_radius_pixels, marker_radius_pixels}},
        0.0f,
        state->marker_positions_pixels[i]);

    renderer_2d_draw_circle(
      &(inout_scene_window->overlay_layer.renderer_2d),
      &(transform),
      marker_color);
  }
}

/*******************************************************************************
 *
 *   Function: multi_window_app_scene_window_draw_crosshair()
 *
 ******************************************************************************/
static void multi_window_app_scene_window_draw_crosshair(
  struct multi_window_app_scene_window_t* inout_scene_window)
{
  const struct multi_window_app_state_t* state = inout_scene_window->state;

  if (!state->is_mouse_over_scene) {
    return;
  }

  const float width_pixels = (float)inout_scene_window->window.metadata.width;
  const float height_pixels = (float)inout_scene_window->window.metadata.height;
  const union fvector2_t mouse = state->mouse_position_pixels;

  const union fvector4_t crosshair_color = {{0.95f, 0.95f, 0.95f, 0.5f}};

  renderer_2d_draw_line(
    &(inout_scene_window->overlay_layer.renderer_2d),
    (union fvector3_t){{0.0f, mouse.comp.y, 0.0f}},
    (union fvector3_t){{width_pixels, mouse.comp.y, 0.0f}},
    crosshair_color);
  renderer_2d_draw_line(
    &(inout_scene_window->overlay_layer.renderer_2d),
    (union fvector3_t){{mouse.comp.x, 0.0f, 0.0f}},
    (union fvector3_t){{mouse.comp.x, height_pixels, 0.0f}},
    crosshair_color);
}

/*******************************************************************************
 *
 *   Function: multi_window_app_scene_window_draw_hint()
 *
 ******************************************************************************/
static void multi_window_app_scene_window_draw_hint(
  struct multi_window_app_scene_window_t* inout_scene_window)
{
  const float margin_pixels = 16.0f;
  const float height_pixels = (float)inout_scene_window->window.metadata.height;

  // Strings are placed by their top-left corner
  renderer_2d_draw_string(
    &(inout_scene_window->overlay_layer.renderer_2d),
    &(inout_scene_window->overlay_typeface),
    (union fvector3_t){{margin_pixels, (height_pixels - margin_pixels), 0.0f}},
    "Overlay layer: click to drop a marker");
}

/*******************************************************************************
 *
 *   Function: multi_window_app_scene_window_overlay_on_event()
 *
 ******************************************************************************/
static void multi_window_app_scene_window_overlay_on_event(
  struct window_t* inout_parent_window,
  struct window_layer_t* inout_window_layer,
  struct event_base_t* inout_event)
{
  (void)inout_parent_window;

  struct multi_window_app_scene_window_t* scene_window =
    container_of(
      inout_window_layer,
      struct multi_window_app_scene_window_t,
      overlay_layer.window_layer);

  // Each event embeds an event_base_t, whose type says which struct it is
  switch (inout_event->event_type)
  {
    case EVENT_TYPE_MOUSE_MOVED:
    {
      multi_window_app_scene_window_on_mouse_moved(
        scene_window,
        container_of(
          inout_event,
          struct event_mouse_moved_t,
          base));
    }
    break;

    case EVENT_TYPE_MOUSE_ENTERED:
    {
      multi_window_app_scene_window_on_mouse_entered(
        scene_window,
        container_of(
          inout_event,
          struct event_mouse_entered_t,
          base));
    }
    break;

    case EVENT_TYPE_MOUSE_BUTTON:
    {
      multi_window_app_scene_window_on_mouse_button(
        scene_window,
        container_of(
          inout_event,
          struct event_mouse_button_t,
          base));
    }
    break;

    default:
    {
    }
    break;
  }
}

/*******************************************************************************
 *
 *   Function: multi_window_app_scene_window_on_mouse_moved()
 *
 ******************************************************************************/
static void multi_window_app_scene_window_on_mouse_moved(
  struct multi_window_app_scene_window_t* inout_scene_window,
  const struct event_mouse_moved_t* in_mouse_moved_event)
{
  if (window_is_event_source(
        &(inout_scene_window->window),
        in_mouse_moved_event->window_handle))
  {
    inout_scene_window->state->is_mouse_over_scene = true;
    inout_scene_window->state->mouse_position_pixels =
      (union fvector2_t){{in_mouse_moved_event->x, in_mouse_moved_event->y}};
  }
}

/*******************************************************************************
 *
 *   Function: multi_window_app_scene_window_on_mouse_entered()
 *
 ******************************************************************************/
static void multi_window_app_scene_window_on_mouse_entered(
  struct multi_window_app_scene_window_t* inout_scene_window,
  const struct event_mouse_entered_t* in_mouse_entered_event)
{
  if (window_is_event_source(
        &(inout_scene_window->window),
        in_mouse_entered_event->window_handle))
  {
    inout_scene_window->state->is_mouse_over_scene = in_mouse_entered_event->is_entered;
  }
}

/*******************************************************************************
 *
 *   Function: multi_window_app_scene_window_on_mouse_button()
 *
 ******************************************************************************/
static void multi_window_app_scene_window_on_mouse_button(
  struct multi_window_app_scene_window_t* inout_scene_window,
  struct event_mouse_button_t* inout_mouse_button_event)
{
  const bool is_left_press =
    ((inout_mouse_button_event->button == EVENT_MOUSE_BUTTON_CODE_LEFT) &&
     (inout_mouse_button_event->action == EVENT_MOUSE_ACTION_PRESS));

  if (is_left_press &&
      window_is_event_source(
        &(inout_scene_window->window),
        inout_mouse_button_event->window_handle))
  {
    multi_window_app_state_add_marker(
      inout_scene_window->state,
      inout_scene_window->state->mouse_position_pixels);

    // Taken: the layers beneath and the window never see it
    inout_mouse_button_event->base.is_handled = 1U;
  }
}
