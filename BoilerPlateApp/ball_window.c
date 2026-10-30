#include "ball_window.h"

#include <math.h>
#include <memory.h>
#include <stdio.h>

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Gives the ball its color, a 20 pixel radius and its starting motion at
//  (200, 200): moving slowly upward, with gravity pulling it down
//
//  out:    out_ball
static void ball_init(
  struct ball_t* out_ball);

// Moves the ball on by in_delta_time_seconds under its acceleration. Where it
//  would cross an edge of the window, its velocity across that edge is
//  reversed and cut to 60%, and it's kept from sinking below the bottom edge
//
//  in:     in_delta_time_seconds, in_window_width, in_window_height
//  in/out: inout_ball
static void ball_update(
  struct ball_t* inout_ball,
  const float in_delta_time_seconds,
  const uint32_t in_window_width,
  const uint32_t in_window_height);

// Finds whether the mouse is over the ball
//
//  in:     in_mouse_pos, in_ball
//  out:    out_mouse_diff (the mouse's offset from the ball's center, set
//            either way)
//  ret:    true when the mouse is within the ball's radius of its center
static bool ball_is_under_mouse(
  const union fvector2_t in_mouse_pos,
  const struct ball_t* in_ball,
  union fvector2_t* out_mouse_diff);

// Handles the ball window's events. Each tick moves the ball on unless it's
//  held, and marks the event handled. Pressing the left mouse button marks
//  the spot and picks the ball up if it's under the mouse; moving the mouse
//  drags a held ball, and releasing the button throws it with the mouse's
//  last movement
//
//  in/out: inout_window, inout_event
static void ball_window_on_event(
  struct window_t* inout_window,
  struct event_base_t* inout_event);

// Finds the transform that scales a unit circle to the ball's radius and
//  moves it to the ball's position
//
//  in:     in_ball
//  ret:    The ball's transform
static const union fmatrix_4x4_t ball_calculate_transform(
  const struct ball_t* in_ball);

// Finds the transform that scales a unit circle to the click marker's radius
//  and moves it to where the mouse was last pressed
//
//  in:     in_ball_window
//  ret:    The click marker's transform
static const union fmatrix_4x4_t ball_click_calculate_transform(
  const struct ball_window_t* in_ball_window);

/*******************************************************************************
 *
 *   Function: ball_init()
 *
 ******************************************************************************/
static void ball_init(
  struct ball_t* out_ball)
{
  out_ball->color = (union fvector4_t){1.0f, 0.0f, 0.0f, 1.0f};

  out_ball->radius_pixels       = 20.0f;
  out_ball->position_pixels     = (union fvector2_t){200.0f, 200.0f};
  out_ball->velocity_pixels_per_second     = (union fvector2_t){0.0f, 10.0f};
  out_ball->acceleration_pixels_per_second_squared = (union fvector2_t){0.0f, -800.0f};

  out_ball->is_held = false;
  out_ball->is_grounded = false;
}

/*******************************************************************************
 *
 *   Function: ball_window_init()
 *
 ******************************************************************************/
uint8_t ball_window_init(
  struct application_t* inout_app_parent,
  struct ball_window_t* out_ball_window)
{
  uint8_t error = 0U;

  const uint32_t width = 1200;
  const uint32_t height = 800;
  const uint8_t num_layers = 2;

  out_ball_window->clicked_indicator_radius_pixels = 4.0f;
  out_ball_window->last_click_position_pixels             = (union fvector2_t){0.0f, 0.0f};
  out_ball_window->clicked_color            = (union fvector4_t){0.0f, 1.0f, 0.0f, 1.0f};

  struct window_t* window_ptr = &(out_ball_window->window);
  struct camera_base_t* camera_base_ptr =
    (struct camera_base_t*)&(out_ball_window->camera_ortho);
  struct renderer_base_t* renderer_base_ptr =
    (struct renderer_base_t*)&(out_ball_window->renderer_2d);

  error |= window_init(
    window_ptr,
    num_layers,
    width,
    height,
    "JotunnWindow_BallWindow",
    inout_app_parent);

  if (!error)
  {
    window_set_context(window_ptr);

    // Camera stuff
    const union fvector3_t camera_position = (union fvector3_t){{0.0f, 0.0f, 2.0f}};
    const union fvector3_t camera_up       = (union fvector3_t){{0.0f, 1.0f, 0.0f}};
    const union fvector3_t camera_front    = (union fvector3_t){{0.0f, 0.0f, -1.0f}};

    const float ortho_left = 0.0f;
    const float ortho_right = (float)width;
    const float ortho_top = (float)height;
    const float ortho_bottom = 0.0f;
    const float ortho_near_plane = -3.0f;
    const float ortho_far_plane = 100.0f;

    camera_orthographic_init(
      &(out_ball_window->camera_ortho),
      camera_position,
      camera_up,
      camera_front);

    camera_orthographic_set_projection(
      &(out_ball_window->camera_ortho),
      ortho_left,
      ortho_right,
      ortho_top,
      ortho_bottom,
      ortho_near_plane,
      ortho_far_plane);

    renderer_2d_init(
      &(out_ball_window->renderer_2d),
      camera_base_ptr,
      window_ptr,
      "ball_app_renderer_2d");

    framebuffer_init(
      &(out_ball_window->framebuffer),
      width,
      height);

    window_layer_init(
      window_ptr,
      &(out_ball_window->window_layer),
      &(out_ball_window->framebuffer),
      camera_base_ptr,
      renderer_base_ptr);

    window_add_layer(
      &(out_ball_window->window),
      &(out_ball_window->window_layer));

    ball_ui_layer_init(
      &(out_ball_window->window),
      &(out_ball_window->ball_ui_layer),
      width,
      height);

    window_add_layer(
      &(out_ball_window->window),
      &(out_ball_window->ball_ui_layer.ui_layer.ui_window_layer));

    window_bind_custom_events(
      window_ptr,
      &ball_window_on_event);

    union fvector4_t window_background_color;
    fvector4_set(&(window_background_color), 1.0f, 1.0f, 1.0f, 0.0f);

    window_set_background_color(
      window_ptr,
      window_background_color);

    ball_init(&(out_ball_window->ball));
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: ball_update()
 *
 ******************************************************************************/
static void ball_update(
  struct ball_t* inout_ball,
  const float in_delta_time_seconds,
  const uint32_t in_window_width,
  const uint32_t in_window_height)
{
  union fvector2_t *pos   = &(inout_ball->position_pixels);
  union fvector2_t *vel   = &(inout_ball->velocity_pixels_per_second);
  union fvector2_t *accel = &(inout_ball->acceleration_pixels_per_second_squared);


  if (fabs(vel->comp.x) <= 0.1)
  {
    vel->comp.x = 0.0f;
  }

  if (fabs(vel->comp.y) <= 0.1 && !inout_ball->is_grounded)
  {
    vel->comp.y = 0.0f;
    inout_ball->is_grounded = true;
  }
  else
  {
    inout_ball->is_grounded = false;
  }

  vel->comp.x +=
    (accel->comp.x * in_delta_time_seconds);
  vel->comp.y +=
    (!inout_ball->is_grounded) * (accel->comp.y * in_delta_time_seconds);

  float new_x = pos->comp.x +
    (vel->comp.x * in_delta_time_seconds) +
    (0.5f) *
      (accel->comp.x *
       (in_delta_time_seconds * in_delta_time_seconds));
  float new_y = pos->comp.y +
    (vel->comp.y * in_delta_time_seconds) +
    (!inout_ball->is_grounded) *
      ((0.5f) *
       (accel->comp.y *
        (in_delta_time_seconds * in_delta_time_seconds)));

  if (((new_x - inout_ball->radius_pixels) <= 0) ||
      ((new_x + (inout_ball->radius_pixels)) >= in_window_width))
  {
    vel->comp.x *= -0.6f;
    new_x = pos->comp.x +
      (vel->comp.x * in_delta_time_seconds) +
      (0.5f) *
        (accel->comp.x *
         (in_delta_time_seconds * in_delta_time_seconds));
  }

  if (((new_y - inout_ball->radius_pixels) <= 0) ||
      ((new_y + inout_ball->radius_pixels) >= in_window_height))
  {
    vel->comp.y *= -0.6f;
    new_y = pos->comp.y +
      (vel->comp.y * in_delta_time_seconds) +
      (!inout_ball->is_grounded) *
        ((0.5f) * (accel->comp.y) *
         (in_delta_time_seconds * in_delta_time_seconds));
  }

  pos->comp.x = new_x;
  pos->comp.y =
    ((new_y - inout_ball->radius_pixels) <= 0) ? (inout_ball->radius_pixels) : (new_y);
}

/*******************************************************************************
 *
 *   Function: ball_is_under_mouse()
 *
 ******************************************************************************/
static bool ball_is_under_mouse(
  const union fvector2_t in_mouse_pos,
  const struct ball_t* in_ball,
  union fvector2_t* out_mouse_diff)
{
  const union fvector2_t ball_center =
    (union fvector2_t){(in_ball->position_pixels.comp.x), (in_ball->position_pixels.comp.y)};

  const float diff_x = (in_mouse_pos.comp.x - ball_center.comp.x);
  const float diff_y = (in_mouse_pos.comp.y - ball_center.comp.y);

  out_mouse_diff->comp.x = diff_x;
  out_mouse_diff->comp.y = diff_y;

  const float hypotenuse = sqrtf((diff_x * diff_x) + (diff_y * diff_y));

  return (hypotenuse < in_ball->radius_pixels);
}

/*******************************************************************************
 *
 *   Function: ball_window_on_event()
 *
 ******************************************************************************/
static void ball_window_on_event(
  struct window_t* inout_window,
  struct event_base_t* inout_event)
{
  struct ball_window_t* ball_window = (struct ball_window_t*)inout_window;
  struct ball_t* ball = &(ball_window->ball);

  static union fvector2_t cur_mouse;
  static union fvector2_t delta_mouse;

  static union fvector2_t intersection_diff;

  switch (inout_event->event_type)
  {
    case EVENT_TYPE_APP_TICK:
    {
      struct event_app_tick_t* app_tick_event = (struct event_app_tick_t*)inout_event;

      double delta_time_seconds = app_tick_event->delta_time_seconds;

      if (!ball->is_held)
      {
        ball_update(
          ball,
          delta_time_seconds,
          ball_window->window.metadata.width,
          ball_window->window.metadata.height);
      }

      inout_event->is_handled = 1U;
    }
    break;

    case EVENT_TYPE_MOUSE_MOVED:
    {
      struct event_mouse_moved_t* event_mouse_moved =
        (struct event_mouse_moved_t*)inout_event;

      float mouse_x = event_mouse_moved->x;
      float mouse_y = event_mouse_moved->y;

      delta_mouse.comp.x = (mouse_x - cur_mouse.comp.x);
      delta_mouse.comp.y = (mouse_y - cur_mouse.comp.y);

      cur_mouse.comp.x = mouse_x;
      cur_mouse.comp.y = mouse_y;

      if (ball->is_held)
      {
        ball->position_pixels.comp.x =
          (cur_mouse.comp.x - intersection_diff.comp.x);
        ball->position_pixels.comp.y =
          (cur_mouse.comp.y - intersection_diff.comp.y);
      }
    }
    break;

    case EVENT_TYPE_MOUSE_BUTTON:
    {
      struct event_mouse_button_t* event_mouse_button =
        (struct event_mouse_button_t*)inout_event;

      if (event_mouse_button->button == 0)
      {
        switch (event_mouse_button->action)
        {
          // Pressed
          case 1:
          {
            ball_window->last_click_position_pixels.comp.x = cur_mouse.comp.x;
            ball_window->last_click_position_pixels.comp.y = cur_mouse.comp.y;

            // #ifdef DEBUG
            //     fprintf(stdout, "Updating last clicked position at: x = %f, y = %f\n", ball_window->last_clicked.comp.x, ball_window->last_clicked.comp.y);
            // #endif

            if (ball_is_under_mouse(cur_mouse, ball, &intersection_diff))
            {
              ball->is_held = true;

              ball->velocity_pixels_per_second.comp.x = 0.0f;
              ball->velocity_pixels_per_second.comp.y = 0.0f;
            }
          }
          break;
          // Released
          case 0:
          {
            if (ball->is_held)
            {
              ball->is_held = false;

              ball->velocity_pixels_per_second.comp.x = (delta_mouse.comp.x * 300.0f);
              ball->velocity_pixels_per_second.comp.y = (delta_mouse.comp.y * 300.0f);
            }
          }
          break;
        }
      }
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
 *   Function: ball_calculate_transform()
 *
 ******************************************************************************/
static const union fmatrix_4x4_t ball_calculate_transform(
  const struct ball_t* in_ball)
{
  union fmatrix_4x4_t transform_matrix;
  {
    union fmatrix_4x4_t scale_matrix, translation_matrix;

    const union fvector3_t scale_factors =
      (union fvector3_t){{in_ball->radius_pixels, in_ball->radius_pixels, in_ball->radius_pixels}};
    const union fvector3_t translation_vector = (union fvector3_t){
      {in_ball->position_pixels.comp.x, in_ball->position_pixels.comp.y, 0.0f}};

    fmatrix_4x4_init(&(scale_matrix));
    fmatrix_4x4_init(&(translation_matrix));
    fmatrix_4x4_init(&(transform_matrix));

    scale_matrix = fmatrix_4x4_transform_scale(&(scale_matrix), scale_factors);
    translation_matrix =
      fmatrix_4x4_transform_translate(&(translation_matrix), translation_vector);
    transform_matrix = fmatrix_4x4_multiply(&(scale_matrix), &(translation_matrix));
  }
  return transform_matrix;
}

/*******************************************************************************
 *
 *   Function: ball_click_calculate_transform()
 *
 ******************************************************************************/
static const union fmatrix_4x4_t ball_click_calculate_transform(
  const struct ball_window_t* in_ball_window)
{
  union fmatrix_4x4_t transform_matrix;
  {
    union fmatrix_4x4_t scale_matrix, translation_matrix;

    const union fvector3_t scale_factors =
      (union fvector3_t){{in_ball_window->clicked_indicator_radius_pixels,
                  in_ball_window->clicked_indicator_radius_pixels,
                  in_ball_window->clicked_indicator_radius_pixels}};
    const union fvector3_t translation_vector =
      (union fvector3_t){{in_ball_window->last_click_position_pixels.comp.x,
                  in_ball_window->last_click_position_pixels.comp.y,
                  0.0f}};

    fmatrix_4x4_init(&(scale_matrix));
    fmatrix_4x4_init(&(translation_matrix));
    fmatrix_4x4_init(&(transform_matrix));

    scale_matrix = fmatrix_4x4_transform_scale(&(scale_matrix), scale_factors);
    translation_matrix =
      fmatrix_4x4_transform_translate(&(translation_matrix), translation_vector);
    transform_matrix = fmatrix_4x4_multiply(&(scale_matrix), &(translation_matrix));
  }
  return transform_matrix;
}

/*******************************************************************************
 *
 *   Function: ball_window_run()
 *
 ******************************************************************************/
void ball_window_run(
  struct window_layer_t* inout_window_layer)
{
  struct ball_window_t* ball_window =
    (struct ball_window_t*)inout_window_layer->parent_window;

  const union fmatrix_4x4_t ball_transform =
    ball_calculate_transform(&(ball_window->ball));

  const union fmatrix_4x4_t click_transform = ball_click_calculate_transform(ball_window);

  renderer_2d_draw_circle(
    &(ball_window->renderer_2d),
    &(ball_transform),
    ball_window->ball.color);

  renderer_2d_draw_circle(
    &(ball_window->renderer_2d),
    &(click_transform),
    ball_window->clicked_color);
}

/*******************************************************************************
 *
 *   Function: ball_ui_window_run()
 *
 ******************************************************************************/
void ball_ui_window_run(
  struct window_t* inout_parent_window,
  const struct window_layer_t* in_window_layer)
{
  struct ball_window_t* ball_window = (struct ball_window_t*)inout_parent_window;

  union fmatrix_4x4_t transform_matrix;
  {
    union fmatrix_4x4_t scale_matrix, translation_matrix;

    const union fvector3_t scale_factors = (union fvector3_t){{20.0f, 20.0f, 20.0f}};
    const union fvector3_t translation_vector = (union fvector3_t){{200.0f, 200.0f, 0.0f}};

    fmatrix_4x4_init(&(scale_matrix));
    fmatrix_4x4_init(&(translation_matrix));
    fmatrix_4x4_init(&(transform_matrix));

    scale_matrix = fmatrix_4x4_transform_scale(&(scale_matrix), scale_factors);
    translation_matrix =
      fmatrix_4x4_transform_translate(&(translation_matrix), translation_vector);
    transform_matrix = fmatrix_4x4_multiply(&(scale_matrix), &(translation_matrix));
  }

  renderer_2d_draw_quad(
    &(ball_window->renderer_2d),
    &(transform_matrix),
    ball_window->clicked_color);
}

/*******************************************************************************
 *
 *   Function: ball_window_cleanup()
 *
 ******************************************************************************/
void ball_window_cleanup(
  struct ball_window_t* inout_ball_window)
{
  framebuffer_cleanup(&(inout_ball_window->framebuffer));
  renderer_2d_cleanup(&(inout_ball_window->renderer_2d));

  ball_ui_layer_cleanup(&(inout_ball_window->ball_ui_layer));
}
