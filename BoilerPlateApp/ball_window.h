#include "application.h"
#include "camera.h"
#include "event.h"
#include "fmatrix.h"
#include "fvector.h"
#include "renderer_2d.h"
#include "ball_ui.h"

#include <stdbool.h>

struct ball_t
{
    float ball_radius;
    fvector2 ball_position;
    fvector2 ball_velocity;
    fvector2 ball_acceleration;
    //
    fvector4 ball_color;
    //
    bool held;
    bool grounded;
};

struct ball_window_t
{
    // Base window structure
    struct window_t window;
    //
    struct window_layer_t window_layer;
    struct framebuffer_t framebuffer;
    //
    struct ball_ui_layer_t ball_ui_layer;
    //
    struct camera_ortho_t camera_ortho;
    struct renderer_2d_t renderer_2d;
    //
    struct ball_t ball;
    //
    float clicked_indicator_radius;
    fvector2 last_clicked;
    fvector4 clicked_color;
};

uint8_t ball_window_init(struct application_t* app_parent, struct ball_window_t* ball_window);
void ball_window_on_event(struct window_t* window, struct event_base_t* event);

void ball_window_run(struct window_layer_t* window_layer);
void ball_ui_window_run(struct window_t* parent_window, struct window_layer_t* window_layer);

void ball_window_cleanup(struct ball_window_t* ball_window);