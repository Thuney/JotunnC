#include "demo_3d_window.h"

#include "testapp_paths.h"

#include <math.h>
#include <stdio.h>

#define DEMO_3D_PI 3.14159265f

static const float orbit_drag_sensitivity = 0.01f;
static const float orbit_auto_rotate_speed = 0.2f;
static const float orbit_min_pitch = -1.4f;
static const float orbit_max_pitch = 1.4f;
static const float orbit_min_distance = 3.0f;
static const float orbit_max_distance = 40.0f;

/******************************************************************************
 *
 * Function: demo_3d_hue_to_color()
 *
 ******************************************************************************/
static fvector4 demo_3d_hue_to_color(
  const float hue)
{
  // HSV to RGB with fixed saturation / value, hue in [0, 1)
  const float saturation = 0.65f;
  const float value = 0.95f;

  const float h = fmodf(hue, 1.0f) * 6.0f;
  const float chroma = value * saturation;
  const float x = chroma * (1.0f - fabsf(fmodf(h, 2.0f) - 1.0f));
  const float m = value - chroma;

  float r = 0.0f, g = 0.0f, b = 0.0f;

  switch ((int)h)
  {
    case 0: r = chroma; g = x; break;
    case 1: r = x; g = chroma; break;
    case 2: g = chroma; b = x; break;
    case 3: g = x; b = chroma; break;
    case 4: r = x; b = chroma; break;
    default: r = chroma; b = x; break;
  }

  return (fvector4){{r + m, g + m, b + m, 1.0f}};
}

/******************************************************************************
 *
 * Function: demo_3d_calculate_transform()
 *
 ******************************************************************************/
static fmatrix_4x4 demo_3d_calculate_transform(
  const fvector3 scale_factors,
  const fvector3 rotation_axis,
  const float rotation_radians,
  const fvector3 translation)
{
  fmatrix_4x4 transform;
  fmatrix_4x4_init(&transform);

  // Scale, then rotate, then translate
  transform = fmatrix_4x4_transform_scale(&transform, scale_factors);
  transform =
    fmatrix_4x4_transform_rotate(&transform, rotation_axis, rotation_radians);
  transform = fmatrix_4x4_transform_translate(&transform, translation);

  return transform;
}

/******************************************************************************
 *
 * Function: demo_3d_calculate_orbit_position()
 *
 ******************************************************************************/
static fvector3 demo_3d_calculate_orbit_position(
  const struct demo_3d_orbit_t* orbit)
{
  const float horizontal_distance = orbit->distance * cosf(orbit->pitch);

  return (fvector3){{orbit->target.comp.x +
                       (horizontal_distance * sinf(orbit->yaw)),
                     orbit->target.comp.y +
                       (orbit->distance * sinf(orbit->pitch)),
                     orbit->target.comp.z +
                       (horizontal_distance * cosf(orbit->yaw))}};
}

/******************************************************************************
 *
 * Function: demo_3d_update_camera()
 *
 ******************************************************************************/
static void demo_3d_update_camera(
  struct demo_3d_window_t* demo_window)
{
  const struct demo_3d_orbit_t* orbit = &(demo_window->orbit);

  const fvector3 camera_up = (fvector3){{0.0f, 1.0f, 0.0f}};
  const fvector3 camera_position = demo_3d_calculate_orbit_position(orbit);

  const fvector3 to_target =
    fvector3_subtract(&(orbit->target), &camera_position);
  const fvector3 camera_front = fvector3_normalize(&to_target);

  camera_set_view(
    &(demo_window->camera_perspective.base),
    camera_position,
    camera_up,
    camera_front);
}

/******************************************************************************
 *
 * Function: demo_3d_load_texture()
 *
 ******************************************************************************/
static void demo_3d_load_texture(
  struct texture_2d_t* texture,
  const char* texture_path)
{
  texture_2d_create_from_file_path(texture, texture_path, 1);

  if (!texture->is_loaded) {
    printf(
      "3D demo texture '%s' failed to load; using a plain cube\n",
      texture_path);
  }
}

/******************************************************************************
 *
 * Function: demo_3d_draw_grid()
 *
 ******************************************************************************/
static void demo_3d_draw_grid(
  struct demo_3d_window_t* demo_window)
{
  struct renderer_3d_t* renderer = &(demo_window->renderer_3d);

  const int half_extent = 8;
  const float extent = (float)half_extent;

  const fvector4 grid_color = {{0.35f, 0.35f, 0.4f, 1.0f}};
  const fvector4 x_axis_color = {{0.9f, 0.25f, 0.25f, 1.0f}};
  const fvector4 y_axis_color = {{0.25f, 0.9f, 0.25f, 1.0f}};
  const fvector4 z_axis_color = {{0.25f, 0.45f, 0.95f, 1.0f}};

  // Grid lines on the XZ plane, with the axes drawn through the origin
  for (int i = -half_extent; i <= half_extent; i++) {
    const float offset = (float)i;

    renderer_3d_draw_line(
      renderer,
      (fvector3){{-extent, 0.0f, offset}},
      (fvector3){{extent, 0.0f, offset}},
      (i == 0) ? x_axis_color : grid_color);
    renderer_3d_draw_line(
      renderer,
      (fvector3){{offset, 0.0f, -extent}},
      (fvector3){{offset, 0.0f, extent}},
      (i == 0) ? z_axis_color : grid_color);
  }

  renderer_3d_draw_line(
    renderer,
    (fvector3){{0.0f, 0.0f, 0.0f}},
    (fvector3){{0.0f, 3.5f, 0.0f}},
    y_axis_color);
}

/******************************************************************************
 *
 * Function: demo_3d_draw_centerpiece()
 *
 ******************************************************************************/
static void demo_3d_draw_centerpiece(
  struct demo_3d_window_t* demo_window)
{
  struct renderer_3d_t* renderer = &(demo_window->renderer_3d);

  const float t = demo_window->elapsed_seconds;

  const fvector3 position = {{0.0f, 1.5f, 0.0f}};
  const fvector3 spin_axis = {{1.0f, 1.0f, 0.0f}};

  // Textured cube, tumbling about a diagonal axis
  const fmatrix_4x4 cube_transform =
    demo_3d_calculate_transform(
      (fvector3){{1.5f, 1.5f, 1.5f}},
      spin_axis,
      0.7f * t,
      position);

  if (demo_window->cube_texture.is_loaded) {
    renderer_3d_draw_textured_cube(
      renderer,
      &cube_transform,
      &(demo_window->cube_texture));
  }
  else {
    renderer_3d_draw_cube(
      renderer,
      &cube_transform,
      (fvector4){{0.85f, 0.85f, 0.85f, 1.0f}});
  }

  // Wireframe cage, counter-rotating around it
  const fmatrix_4x4 cage_transform =
    demo_3d_calculate_transform(
      (fvector3){{2.6f, 2.6f, 2.6f}},
      spin_axis,
      -0.4f * t,
      position);

  renderer_3d_draw_unfilled_cube(
    renderer,
    &cage_transform,
    (fvector4){{1.0f, 0.85f, 0.3f, 1.0f}});
}

/******************************************************************************
 *
 * Function: demo_3d_draw_orbiting_shapes()
 *
 ******************************************************************************/
static void demo_3d_draw_orbiting_shapes(
  struct demo_3d_window_t* demo_window)
{
  struct renderer_3d_t* renderer = &(demo_window->renderer_3d);

  const float t = demo_window->elapsed_seconds;

  const unsigned int num_shapes = 9;
  const float ring_radius = 4.5f;

  const fvector3 y_axis = {{0.0f, 1.0f, 0.0f}};

  for (unsigned int i = 0; i < num_shapes; i++) {
    const float fraction = (float)i / (float)num_shapes;
    const float angle = (2.0f * DEMO_3D_PI * fraction) + (0.35f * t);

    const fvector3 position =
      (fvector3){{ring_radius * cosf(angle),
                  1.0f + (0.5f * sinf((1.5f * t) + (float)i)),
                  ring_radius * sinf(angle)}};

    const fvector4 color = demo_3d_hue_to_color(fraction);
    const float spin = (1.2f * t) + (float)i;

    switch (i % 3)
    {
      case 0:
      {
        const fmatrix_4x4 transform =
          demo_3d_calculate_transform(
            (fvector3){{0.8f, 0.8f, 0.8f}},
            y_axis,
            spin,
            position);
        renderer_3d_draw_cube(renderer, &transform, color);
      }
      break;
      case 1:
      {
        const fmatrix_4x4 transform =
          demo_3d_calculate_transform(
            (fvector3){{0.9f, 1.1f, 0.9f}},
            y_axis,
            spin,
            position);
        renderer_3d_draw_pyramid(renderer, &transform, color);
      }
      break;
      default:
      {
        const fmatrix_4x4 transform =
          demo_3d_calculate_transform(
            (fvector3){{0.5f, 0.5f, 0.5f}},
            y_axis,
            spin,
            position);
        renderer_3d_draw_sphere(renderer, &transform, color);
      }
      break;
    }
  }
}

/******************************************************************************
 *
 * Function: demo_3d_window_init()
 *
 ******************************************************************************/
uint8_t demo_3d_window_init(
  struct application_t* app_parent,
  struct demo_3d_window_t* demo_window)
{
  uint8_t error = 0U;

  const uint32_t width = 800;
  const uint32_t height = 600;
  const uint8_t num_layers = 1;

  struct window_t* window_ptr = &(demo_window->window);
  struct camera_base_t* camera_base_ptr =
    (struct camera_base_t*)&(demo_window->camera_perspective);
  struct renderer_base_t* renderer_base_ptr =
    (struct renderer_base_t*)&(demo_window->renderer_3d);

  error |= window_init(
    window_ptr,
    num_layers,
    width,
    height,
    "JotunnWindow3D",
    app_parent);

  if (!error) {
    window_set_context(window_ptr);

    // GL objects aren't shared between windows, so load our own copy
    demo_3d_load_texture(
      &(demo_window->cube_texture),
      PATH_FROM_TESTAPP_ROOT("/res/textures/lenna.png"));

    demo_window->elapsed_seconds = 0.0f;

    demo_window->orbit.yaw = 0.6f;
    demo_window->orbit.pitch = 0.45f;
    demo_window->orbit.distance = 12.0f;
    demo_window->orbit.target = (fvector3){{0.0f, 1.0f, 0.0f}};
    demo_window->orbit.dragging = 0U;
    demo_window->orbit.last_mouse = (fvector2){{0.0f, 0.0f}};

    // Camera stuff
    const fvector3 camera_position =
      demo_3d_calculate_orbit_position(&(demo_window->orbit));
    const fvector3 camera_up = (fvector3){{0.0f, 1.0f, 0.0f}};
    const fvector3 to_target =
      fvector3_subtract(&(demo_window->orbit.target), &camera_position);
    const fvector3 camera_front = fvector3_normalize(&to_target);

    const float field_of_view = 60.0f;
    const float aspect_ratio = (float)width / (float)height;
    const float near_plane = 0.1f;
    const float far_plane = 100.0f;

    camera_init_perspective(
      &(demo_window->camera_perspective),
      camera_position,
      camera_up,
      camera_front);

    camera_set_projection_perspective(
      &(demo_window->camera_perspective),
      field_of_view,
      aspect_ratio,
      near_plane,
      far_plane);

    framebuffer_init(
      &(demo_window->framebuffer),
      width,
      height);

    renderer_3d_init(
      &(demo_window->renderer_3d),
      camera_base_ptr,
      window_ptr,
      "demo_3d_renderer");

    renderer_3d_set_light(
      &(demo_window->renderer_3d),
      (fvector3){{-0.5f, -1.0f, -0.3f}},
      (fvector3){{1.0f, 0.97f, 0.9f}},
      0.3f);

    window_layer_init(
      window_ptr,
      &(demo_window->window_layer),
      &(demo_window->framebuffer),
      camera_base_ptr,
      renderer_base_ptr);

    window_layer_set_custom_layer_run(
      &(demo_window->window_layer),
      &demo_3d_window_run);

    window_add_layer(
      window_ptr,
      &(demo_window->window_layer));

    window_bind_custom_events(
      window_ptr,
      &demo_3d_window_on_event);

    fvector4 background_color;
    fvector4_set(&(background_color), 0.12f, 0.13f, 0.16f, 1.0f);

    window_set_background_color(
      window_ptr,
      background_color);
  }

  return error;
}

/******************************************************************************
 *
 * Function: demo_3d_window_on_event()
 *
 ******************************************************************************/
void demo_3d_window_on_event(
  struct window_t* window,
  struct event_base_t* event)
{
  struct demo_3d_window_t* demo_window = (struct demo_3d_window_t*)window;
  struct demo_3d_orbit_t* orbit = &(demo_window->orbit);

  // The application forwards every window's events to every window, so
  //  input events are checked against this window's handle
  const void* this_window_handle = window->context_data.window_handle;

  switch (event->event_type)
  {
    case EVENT_APP_TICK:
    {
      const struct event_app_tick_t* app_tick_event =
        (struct event_app_tick_t*)event;

      const float delta_time_seconds = app_tick_event->delta_time_seconds;

      demo_window->elapsed_seconds += delta_time_seconds;

      if (!orbit->dragging) {
        orbit->yaw += (orbit_auto_rotate_speed * delta_time_seconds);
      }
    }
    break;

    case EVENT_MOUSE_MOVED:
    {
      const struct event_mouse_moved_t* mouse_moved_event =
        (struct event_mouse_moved_t*)event;

      const uint8_t is_this_window =
        (mouse_moved_event->window_handle == this_window_handle);

      if (is_this_window) {
        const fvector2 mouse = {{mouse_moved_event->x, mouse_moved_event->y}};

        if (orbit->dragging) {
          const float delta_x = mouse.comp.x - orbit->last_mouse.comp.x;
          const float delta_y = mouse.comp.y - orbit->last_mouse.comp.y;

          orbit->yaw -= (delta_x * orbit_drag_sensitivity);
          orbit->pitch = fminf(
            fmaxf(
              orbit->pitch - (delta_y * orbit_drag_sensitivity),
              orbit_min_pitch),
            orbit_max_pitch);
        }

        orbit->last_mouse = mouse;
      }
    }
    break;

    case EVENT_MOUSE_BUTTON:
    {
      const struct event_mouse_button_t* mouse_button_event =
        (struct event_mouse_button_t*)event;

      const uint8_t is_this_window =
        (mouse_button_event->window_handle == this_window_handle);

      // Left button - pressed (1) in this window starts a drag, released (0)
      //  anywhere ends it
      if (mouse_button_event->button == 0) {
        if (mouse_button_event->action == 1) {
          orbit->dragging = is_this_window;
        }
        else if (mouse_button_event->action == 0) {
          orbit->dragging = 0U;
        }
      }
    }
    break;

    case EVENT_MOUSE_SCROLLED:
    {
      const struct event_mouse_scrolled_t* mouse_scrolled_event =
        (struct event_mouse_scrolled_t*)event;

      const uint8_t is_this_window =
        (mouse_scrolled_event->window_handle == this_window_handle);

      if (is_this_window) {
        orbit->distance = fminf(
          fmaxf(
            orbit->distance * (1.0f - (0.1f * mouse_scrolled_event->y_offset)),
            orbit_min_distance),
          orbit_max_distance);
      }
    }
    break;

    default:
    {
    }
    break;
  }
}

/******************************************************************************
 *
 * Function: demo_3d_window_run()
 *
 ******************************************************************************/
void demo_3d_window_run(
  struct window_layer_t* window_layer)
{
  struct demo_3d_window_t* demo_window =
    (struct demo_3d_window_t*)window_layer->parent_window;

  demo_3d_update_camera(demo_window);

  demo_3d_draw_grid(demo_window);
  demo_3d_draw_centerpiece(demo_window);
  demo_3d_draw_orbiting_shapes(demo_window);
}

/******************************************************************************
 *
 * Function: demo_3d_window_cleanup()
 *
 ******************************************************************************/
void demo_3d_window_cleanup(
  struct demo_3d_window_t* demo_window)
{
  // Our GL objects belong to this window's context
  window_set_context(&(demo_window->window));

  renderer_3d_cleanup(&(demo_window->renderer_3d));
  framebuffer_cleanup(&(demo_window->framebuffer));
  texture_2d_cleanup(&(demo_window->cube_texture));
}
