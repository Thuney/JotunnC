#include "ball_window.h"

#include <memory.h>

static void ball_init(struct ball_t* ball)
{
    ball->ball_color    = (fvector4) {1.0f, 0.0f, 0.0f, 1.0f};

    ball->ball_radius   = 20.0f;
    ball->ball_position = (fvector2) {200.0f, 200.0f};
    ball->ball_velocity = (fvector2) {400.0f, 600.0f};
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
    float new_x = ball->ball_position.comp.x + (ball->ball_velocity.comp.x * delta_time_seconds);
    float new_y = ball->ball_position.comp.y + (ball->ball_velocity.comp.y * delta_time_seconds);

    if ( (new_x <= 0) || ((new_x + (2.0f*ball->ball_radius)) >= window_width) )
    {
        ball->ball_velocity.comp.x *= -1.0f;
        new_x = ball->ball_position.comp.x + (ball->ball_velocity.comp.x * delta_time_seconds);
    }

    if ( (new_y <= 0) || ((new_y + (2.0f*ball->ball_radius)) >= window_height) )
    {
        ball->ball_velocity.comp.y *= -1.0f;
        new_y = ball->ball_position.comp.y + (ball->ball_velocity.comp.y * delta_time_seconds);
    }

    ball->ball_position.comp.x = new_x;
    ball->ball_position.comp.y = new_y;
}

void ball_window_on_event(struct window_t* window, struct event_base_t* event)
{
    struct ball_window_t* ball_window = (struct ball_window_t*)window;
    struct ball_t* ball = &(ball_window->ball);

    switch (event->event_type)
    {
        case EVENT_APP_TICK:
        {
            struct event_app_tick_t* app_tick_event = (struct event_app_tick_t*)event;

            float delta_time_seconds = app_tick_event->delta_time_seconds;

            ball_update(ball, delta_time_seconds, ball_window->window.metadata.width, ball_window->window.metadata.height);

            event->handled = 1U;
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