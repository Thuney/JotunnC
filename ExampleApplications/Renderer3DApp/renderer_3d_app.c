#include "renderer_3d_app.h"
#include "renderer_3d_app_scene.h"

#include "fmath.h"
#include "util.h"

#include <stdbool.h>
#include <stdio.h>

#define RENDERER_3D_APP_CUBE_TEXTURE_PATH EXAMPLE_RES_PATH "/textures/AaronShakespeare.png"

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Places the orbit the camera rides: looking at a point above the origin,
//  from a little above the ground, turning slowly by itself
//
//  out:    out_orbit
static void renderer_3d_app_init_orbit(
  struct camera_orbit_t* out_orbit);

// Opens the window, leaving its graphics context current. Its list of layers
//  is allocated by the engine, being given NULL storage; see
//  AllocationStorageApp for providing it
//
//  in/out: inout_app
//  ret:    0 on success; non-zero when the window can't be opened
static uint8_t renderer_3d_app_create_window(
  struct renderer_3d_app_t* inout_app);

// Sets up the scene's 3D layer, covering the window, lit by a warm light
//  from above, has it call renderer_3d_app_scene_run() each frame, and adds
//  it to the window. The renderer's batch storage is allocated by the
//  engine. The window's graphics context must be current
//
//  in/out: inout_app
//  ret:    0 on success; non-zero when the renderer's storage can't be
//            allocated or the layer can't be added
static uint8_t renderer_3d_app_create_scene_layer(
  struct renderer_3d_app_t* inout_app);

// Loads the cube's texture from an image file, flipped so that the image's
//  first row is at the bottom, as OpenGL expects. A file that can't be
//  loaded is reported on stderr, and Jotunn's placeholder texture made
//  instead
//
//  in/out: inout_app
static void renderer_3d_app_load_cube_texture(
  struct renderer_3d_app_t* inout_app);

// Hands each event the window passes on to the handler for its type. Bound
//  as the window's custom event handler, so inout_window is the window of a
//  renderer_3d_app_t
//
//  in/out: inout_window, inout_event
static void renderer_3d_app_on_event(
  struct window_t* inout_window,
  struct event_base_t* inout_event);

// Turns the orbit as time passes
//
//  in:     in_app_tick_event
//  in/out: inout_app
static void renderer_3d_app_on_app_tick(
  struct renderer_3d_app_t* inout_app,
  const struct event_app_tick_t* in_app_tick_event);

// Follows the mouse over this window with the orbit
//
//  in:     in_mouse_moved_event
//  in/out: inout_app
static void renderer_3d_app_on_mouse_moved(
  struct renderer_3d_app_t* inout_app,
  const struct event_mouse_moved_t* in_mouse_moved_event);

// Starts a drag when the left button is pressed over this window, and ends
//  it when the button is released anywhere
//
//  in:     in_mouse_button_event
//  in/out: inout_app
static void renderer_3d_app_on_mouse_button(
  struct renderer_3d_app_t* inout_app,
  const struct event_mouse_button_t* in_mouse_button_event);

// Zooms the orbit when the mouse is scrolled over this window
//
//  in:     in_mouse_scrolled_event
//  in/out: inout_app
static void renderer_3d_app_on_mouse_scrolled(
  struct renderer_3d_app_t* inout_app,
  const struct event_mouse_scrolled_t* in_mouse_scrolled_event);

/*******************************************************************************
 *
 *   Function: renderer_3d_app_init()
 *
 ******************************************************************************/
uint8_t renderer_3d_app_init(
  struct renderer_3d_app_t* out_app)
{
  const uint8_t max_windows = 1;

  renderer_3d_app_init_orbit(&(out_app->orbit));

  // The engine allocates the list of windows, being given NULL storage
  uint8_t error =
    application_init(
      &(out_app->application),
      "Renderer3DApp",
      NULL,
      max_windows);

  if (!error) {
    error = renderer_3d_app_create_window(out_app);
  }

  // Graphics resources belong to the context current as they're made, which
  //  the window's now is
  if (!error) {
    error = renderer_3d_app_create_scene_layer(out_app);
  }

  if (!error) {
    renderer_3d_app_load_cube_texture(out_app);

    window_bind_custom_events(
      &(out_app->window),
      &(renderer_3d_app_on_event));

    window_set_background_color(
      &(out_app->window),
      (union fvector4_t){{0.12f, 0.13f, 0.16f, 1.0f}});

    error =
      application_add_window(
        &(out_app->application),
        &(out_app->window));
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_app_cleanup()
 *
 ******************************************************************************/
void renderer_3d_app_cleanup(
  struct renderer_3d_app_t* inout_app)
{
  // The window runs leave no graphics context current, and these were made
  //  in the window's
  window_set_context(&(inout_app->window));

  texture_2d_cleanup(&(inout_app->cube_texture));
  window_layer_3d_cleanup(&(inout_app->scene_layer));

  // Cleans up the window too, but leaves its memory, which is ours
  application_cleanup(&(inout_app->application));
}

/*******************************************************************************
 *
 *   Function: renderer_3d_app_init_orbit()
 *
 ******************************************************************************/
static void renderer_3d_app_init_orbit(
  struct camera_orbit_t* out_orbit)
{
  camera_orbit_init(
    out_orbit,
    (union fvector3_t){{0.0f, 1.0f, 0.0f}},
    12.0f,
    0.6f,
    0.45f);

  // Changed from the defaults: turning by itself while it isn't dragged, and
  //  zooming within limits suiting the scene
  out_orbit->auto_turn_radians_per_second = 0.2f;
  out_orbit->min_distance = 3.0f;
  out_orbit->max_distance = 40.0f;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_app_create_window()
 *
 ******************************************************************************/
static uint8_t renderer_3d_app_create_window(
  struct renderer_3d_app_t* inout_app)
{
  const uint8_t max_window_layers = 1;

  const uint8_t error =
    window_init(
      &(inout_app->window),
      NULL,
      max_window_layers,
      RENDERER_3D_APP_WIDTH_PIXELS,
      RENDERER_3D_APP_HEIGHT_PIXELS,
      "Jotunn - Renderer 3D Example",
      &(inout_app->application));

  return error;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_app_create_scene_layer()
 *
 ******************************************************************************/
static uint8_t renderer_3d_app_create_scene_layer(
  struct renderer_3d_app_t* inout_app)
{
  const float field_of_view_radians = fmath_degrees_to_radians(60.0f);
  const float near_plane = 0.1f;
  const float far_plane = 100.0f;

  // A framebuffer of the window's size, a perspective camera over the
  //  window's aspect ratio, and a renderer drawing through it, tied together
  //  as a layer. The window begins the renderer's scene, calls the layer's
  //  run, then ends the scene, each frame. Where the camera looks from is
  //  left to the orbit, which places it each frame. The engine allocates the
  //  batch storage, being given NULL
  uint8_t error =
    window_layer_3d_init(
      &(inout_app->window),
      &(inout_app->scene_layer),
      NULL,
      "renderer_3d_app_scene_renderer",
      field_of_view_radians,
      near_plane,
      far_plane,
      &(renderer_3d_app_scene_run));

  if (!error) {
    // The direction the light travels in, its color, and how much of it
    //  reaches surfaces facing away
    renderer_3d_set_light(
      &(inout_app->scene_layer.renderer_3d),
      (union fvector3_t){{-0.5f, -1.0f, -0.3f}},
      (union fvector3_t){{1.0f, 0.97f, 0.9f}},
      0.3f);

    error =
      window_add_layer(
        &(inout_app->window),
        &(inout_app->scene_layer.window_layer));
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_app_load_cube_texture()
 *
 ******************************************************************************/
static void renderer_3d_app_load_cube_texture(
  struct renderer_3d_app_t* inout_app)
{
  const unsigned int should_flip_vertically = 1;

  texture_2d_create_from_file_path(
    &(inout_app->cube_texture),
    RENDERER_3D_APP_CUBE_TEXTURE_PATH,
    should_flip_vertically);

  // Something to draw all the same, which makes the failure plain to see
  if (!inout_app->cube_texture.is_loaded) {
    fprintf(
      stderr,
      "Couldn't load '%s'; drawing a placeholder instead\n",
      RENDERER_3D_APP_CUBE_TEXTURE_PATH);

    texture_2d_create_placeholder(&(inout_app->cube_texture));
  }
}

/*******************************************************************************
 *
 *   Function: renderer_3d_app_on_event()
 *
 ******************************************************************************/
static void renderer_3d_app_on_event(
  struct window_t* inout_window,
  struct event_base_t* inout_event)
{
  struct renderer_3d_app_t* app =
    container_of(
      inout_window,
      struct renderer_3d_app_t,
      window);

  // Each event embeds an event_base_t, whose type says which struct it is
  switch (inout_event->event_type)
  {
    case EVENT_TYPE_APP_TICK:
    {
      renderer_3d_app_on_app_tick(
        app,
        container_of(
          inout_event,
          struct event_app_tick_t,
          base));
    }
    break;

    case EVENT_TYPE_MOUSE_MOVED:
    {
      renderer_3d_app_on_mouse_moved(
        app,
        container_of(
          inout_event,
          struct event_mouse_moved_t,
          base));
    }
    break;

    case EVENT_TYPE_MOUSE_BUTTON:
    {
      renderer_3d_app_on_mouse_button(
        app,
        container_of(
          inout_event,
          struct event_mouse_button_t,
          base));
    }
    break;

    case EVENT_TYPE_MOUSE_SCROLLED:
    {
      renderer_3d_app_on_mouse_scrolled(
        app,
        container_of(
          inout_event,
          struct event_mouse_scrolled_t,
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
 *   Function: renderer_3d_app_on_app_tick()
 *
 ******************************************************************************/
static void renderer_3d_app_on_app_tick(
  struct renderer_3d_app_t* inout_app,
  const struct event_app_tick_t* in_app_tick_event)
{
  camera_orbit_advance(
    &(inout_app->orbit),
    in_app_tick_event->delta_time_seconds);
}

/*******************************************************************************
 *
 *   Function: renderer_3d_app_on_mouse_moved()
 *
 ******************************************************************************/
static void renderer_3d_app_on_mouse_moved(
  struct renderer_3d_app_t* inout_app,
  const struct event_mouse_moved_t* in_mouse_moved_event)
{
  // Every window is offered every window's input
  if (window_is_event_source(
        &(inout_app->window),
        in_mouse_moved_event->window_handle))
  {
    camera_orbit_move_mouse(
      &(inout_app->orbit),
      (union fvector2_t){{in_mouse_moved_event->x, in_mouse_moved_event->y}});
  }
}

/*******************************************************************************
 *
 *   Function: renderer_3d_app_on_mouse_button()
 *
 ******************************************************************************/
static void renderer_3d_app_on_mouse_button(
  struct renderer_3d_app_t* inout_app,
  const struct event_mouse_button_t* in_mouse_button_event)
{
  const bool is_left_button = (in_mouse_button_event->button == EVENT_MOUSE_BUTTON_CODE_LEFT);
  const bool is_press = (in_mouse_button_event->action == EVENT_MOUSE_ACTION_PRESS);

  if (is_left_button) {
    const bool is_drag_started =
      (is_press &&
       window_is_event_source(
         &(inout_app->window),
         in_mouse_button_event->window_handle));

    camera_orbit_set_dragging(
      &(inout_app->orbit),
      is_drag_started);
  }
}

/*******************************************************************************
 *
 *   Function: renderer_3d_app_on_mouse_scrolled()
 *
 ******************************************************************************/
static void renderer_3d_app_on_mouse_scrolled(
  struct renderer_3d_app_t* inout_app,
  const struct event_mouse_scrolled_t* in_mouse_scrolled_event)
{
  if (window_is_event_source(
        &(inout_app->window),
        in_mouse_scrolled_event->window_handle))
  {
    camera_orbit_zoom(
      &(inout_app->orbit),
      in_mouse_scrolled_event->y_offset);
  }
}
