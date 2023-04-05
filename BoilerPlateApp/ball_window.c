#include "ball_window.h"

#include <math.h>
#include <memory.h>
#include <stdio.h>

static void ball_init(struct ball_t* ball)
{
    ball->ball_color    = (fvector4) {1.0f, 0.0f, 0.0f, 1.0f};

    ball->ball_radius   = 20.0f;
    ball->ball_position = (fvector2) {200.0f, 200.0f};
    ball->ball_velocity = (fvector2) {0.0f, 10.0f};
    ball->ball_acceleration = (fvector2) {0.0f, -800.0f};

    ball->held = false;
    ball->grounded = false;
}

uint8_t ball_window_init(struct application_t* app_parent, struct ball_window_t* ball_window)
{
    uint8_t error = 0U;

    const uint32_t width  = 600;
    const uint32_t height = 400;

    struct window_t* window_ptr = &(ball_window->window);
    struct camera_base_t* camera_base_ptr = (struct camera_base_t*)&(ball_window->camera_ortho);
    struct renderer_base_t* renderer_base_ptr = (struct renderer_base_t*)&(ball_window->renderer_2d);

    error |= window_init(window_ptr, width, height, "JotunnWindow_BallWindow", app_parent);
    window_set_renderer(window_ptr, renderer_base_ptr, camera_base_ptr);
    
    window_bind_custom_events(window_ptr, &ball_window_on_event);

    fvector4 window_background_color;
    fvector4_set(&window_background_color, 0.1f, 0.1f, 0.1f, 1.0f);
    // fvector4_set(&window_background_color, 1.0f, 1.0f, 1.0f, 1.0f);

    // Camera stuff
    const fvector3 camera_position = (fvector3) { {0.0f, 0.0f,  2.0f} };
    const fvector3 camera_up       = (fvector3) { {0.0f, 1.0f,  0.0f} };
    const fvector3 camera_front    = (fvector3) { {0.0f, 0.0f, -1.0f} };

    const float ortho_left   = 0.0f; 
    const float ortho_right  = (float)width;
    const float ortho_top    = (float)height;
    const float ortho_bottom = 0.0f;
    const float ortho_near_plane = -3.0f;
    const float ortho_far_plane  = 100.0f;

    window_set_context(window_ptr);

    camera_init_orthographic(&(ball_window->camera_ortho), camera_position, camera_up, camera_front);
    camera_set_projection_orthographic(&(ball_window->camera_ortho), ortho_left, ortho_right, ortho_top, ortho_bottom, ortho_near_plane, ortho_far_plane);

    renderer_2d_init(&(ball_window->renderer_2d), camera_base_ptr, window_ptr, "ball_renderer_2d");

    window_set_background_color(window_ptr, window_background_color);

    window_release_context();

    ball_init(&(ball_window->ball));

    return error;
}

static void ball_update(struct ball_t* ball, const float delta_time_seconds, const uint32_t window_width, const uint32_t window_height)
{
    if (fabs(ball->ball_velocity.comp.x) <= 0.1) ball->ball_velocity.comp.x = 0.0f;
    if (fabs(ball->ball_velocity.comp.y) <= 0.1 && !ball->grounded)
    {
        ball->ball_velocity.comp.y = 0.0f;
        ball->grounded = true;
    }
    else
    {
        ball->grounded = false;
    }

    ball->ball_velocity.comp.x += (ball->ball_acceleration.comp.x * delta_time_seconds);
    ball->ball_velocity.comp.y += (!ball->grounded)*(ball->ball_acceleration.comp.y * delta_time_seconds);

    float new_x = ball->ball_position.comp.x + (ball->ball_velocity.comp.x * delta_time_seconds) + (0.5f)*(ball->ball_acceleration.comp.x * (delta_time_seconds*delta_time_seconds));
    float new_y = ball->ball_position.comp.y + (ball->ball_velocity.comp.y * delta_time_seconds) + (!ball->grounded)*((0.5f)*(ball->ball_acceleration.comp.y * (delta_time_seconds*delta_time_seconds)));

    if ( ((new_x - ball->ball_radius) <= 0) || ((new_x + (ball->ball_radius)) >= window_width) )
    {
        ball->ball_velocity.comp.x *= -0.6f;
        new_x = ball->ball_position.comp.x + (ball->ball_velocity.comp.x * delta_time_seconds) + (0.5f)*(ball->ball_acceleration.comp.x * (delta_time_seconds*delta_time_seconds));
    }

    if ( ((new_y - ball->ball_radius) <= 0) || ((new_y + ball->ball_radius) >= window_height) )
    {
        ball->ball_velocity.comp.y *= -0.6f;
        new_y = ball->ball_position.comp.y + (ball->ball_velocity.comp.y * delta_time_seconds) + (!ball->grounded)*((0.5f)*(ball->ball_acceleration.comp.y) * (delta_time_seconds*delta_time_seconds));
    }

    ball->ball_position.comp.x = new_x;
    ball->ball_position.comp.y = ((new_y - ball->ball_radius) <= 0) ? (ball->ball_radius) : (new_y);
}

static bool mouse_intersects_ball(const fvector2 mouse_pos, struct ball_t* ball, fvector2* mouse_diff)
{
    const fvector2 ball_center = (fvector2) { (ball->ball_position.comp.x),  (ball->ball_position.comp.y)};

    const float diff_x = (mouse_pos.comp.x - ball_center.comp.x);
    const float diff_y = (mouse_pos.comp.y - ball_center.comp.y);

    mouse_diff->comp.x = diff_x;
    mouse_diff->comp.y = diff_y;

    const float hypotenuse = sqrtf((diff_x*diff_x) + (diff_y*diff_y));

    return (hypotenuse < ball->ball_radius);
}

void ball_window_on_event(struct window_t* window, struct event_base_t* event)
{
    struct ball_window_t* ball_window = (struct ball_window_t*)window;
    struct ball_t* ball = &(ball_window->ball);

    static fvector2 cur_mouse;
    static fvector2 delta_mouse;

    static fvector2 intersection_diff;

    switch (event->event_type)
    {
        case EVENT_APP_TICK:
        {
            struct event_app_tick_t* app_tick_event = (struct event_app_tick_t*)event;

            double delta_time_seconds = app_tick_event->delta_time_seconds;

            if (!ball->held) ball_update(ball, delta_time_seconds, ball_window->window.metadata.width, ball_window->window.metadata.height);

            event->handled = 1U;
        }
        break;

        case EVENT_MOUSE_MOVED:
        {
            struct event_mouse_moved_t* event_mouse_moved = (struct event_mouse_moved_t*)event;

            float mouse_x = event_mouse_moved->x;
            float mouse_y = (ball_window->window.metadata.height - event_mouse_moved->y);

            delta_mouse.comp.x = (cur_mouse.comp.x - mouse_x);
            delta_mouse.comp.y = (cur_mouse.comp.y - mouse_y);

            cur_mouse.comp.x = mouse_x;
            cur_mouse.comp.y = mouse_y;

            if (ball->held)
            {
                ball->ball_position.comp.x = (cur_mouse.comp.x - intersection_diff.comp.x);
                ball->ball_position.comp.y = (cur_mouse.comp.y - intersection_diff.comp.y);
            }
        }
        break;

        case EVENT_MOUSE_BUTTON:
        {
            struct event_mouse_button_t* event_mouse_button = (struct event_mouse_button_t*)event;

            if (event_mouse_button->button == 0)
            {
                switch(event_mouse_button->action)
                {
                    // Pressed
                    case 1:
                    {
                        if (mouse_intersects_ball(cur_mouse, ball, &intersection_diff))
                        {
                            ball->held = true;

                            ball->ball_velocity.comp.x = 0.0f;
                            ball->ball_velocity.comp.y = 0.0f;
                        }
                    }
                    break;
                    // Released
                    case 0:
                    {
                        if (ball->held)
                        {
                            ball->held = false;

                            ball->ball_velocity.comp.x = (-delta_mouse.comp.x * 300.0f);
                            ball->ball_velocity.comp.y = (-delta_mouse.comp.y * 300.0f);
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

static const fmatrix_4x4 ball_calculate_transform(struct ball_t* ball)
{
    fmatrix_4x4 transform_matrix;
    {
        fmatrix_4x4 scale_matrix, translation_matrix;

        const fvector3 scale_factors = (fvector3) { {ball->ball_radius, ball->ball_radius, ball->ball_radius} };
        const fvector3 translation_vector = (fvector3) { {ball->ball_position.comp.x, ball->ball_position.comp.y, 0.0f} };

        fmatrix_4x4_init(&scale_matrix);
        fmatrix_4x4_init(&translation_matrix);
        fmatrix_4x4_init(&transform_matrix);

        scale_matrix = fmatrix_4x4_transform_scale(&scale_matrix, scale_factors);
        translation_matrix = fmatrix_4x4_transform_translate(&translation_matrix, translation_vector);
        transform_matrix = fmatrix_4x4_multiply(&scale_matrix, &translation_matrix);
    }
    return transform_matrix;
}

void ball_window_run(struct window_t* window)
{
    struct ball_window_t* ball_window = (struct ball_window_t*)window;

    const fmatrix_4x4 transform = ball_calculate_transform(&(ball_window->ball));

    renderer_2d_draw_circle(&(ball_window->renderer_2d), &(transform), ball_window->ball.ball_color);
}

void ball_window_cleanup(struct ball_window_t* ball_window)
{
    renderer_2d_cleanup(&(ball_window->renderer_2d));
}