#include "application.h"
#include "fvector.h"
#include "window.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

extern int window_graphics_init(struct window_t* window);
extern void window_graphics_run(struct window_t* window);
extern void window_graphics_cleanup(struct window_t* window);
extern void window_graphics_set_background_color(struct window_t* window, const fvector4 color);

// Window Callbacks

// Helpers

static void window_set_metadata(struct window_data_t* metadata, int width, int height, char* tag, struct application_t* app_parent, void (*function_event_notify)(struct application_t*, struct event_base_t*))
{
    // Store our window tag
    int tag_length = strlen(tag);
    metadata->tag = (char*) malloc(tag_length*sizeof(char));    
    strcpy(metadata->tag, tag);

    // Assign window properties
    metadata->width  = width;
    metadata->height = height;

    metadata->signaled_close = 0;

    metadata->parent_application    = app_parent;
    metadata->function_event_notify = function_event_notify;
}

// Exposed functions

int window_init(struct window_t* window, int width, int height, char* tag, struct application_t* app_parent)
{
    window_set_metadata(&window->metadata, width, height, tag, app_parent, &application_on_event);

    int error = window_graphics_init(window);

    #ifdef DEBUG
        if (error) fprintf(stdout, "Error during window_graphics_init\n");
    #endif

    fvector4 background_color;
    fvector4_set(&background_color, 0.1f, 0.1f, 0.1f, 1.0f);
    window_set_background_color(window, background_color);

    renderer_2d_init(&window->renderer, "2DRenderer", 0.0f, (float)width, (float)height, 0.0f, -1.0f, 100.0f);
    // renderer_2d_init(&window->renderer, "2DRenderer", -(float)width/2.0f, (float)width/2.0f, (float)height/2.0f, -(float)height/2.0f, -1.0f, 100.0f);

    return error;
}

int window_run(struct window_t* window)
{
    renderer_2d_begin_scene(&window->renderer);

    // const fvector3 circle_position = { 500.0f, 500.0f, 0.0f};
    // const fvector4 circle_color    = { 1.0f, 0.0f, 0.0f, 1.0f };
    // renderer_2d_draw_circle(&window->renderer, circle_position, circle_color);

    const float spacing = 50.0f;
    const float color_increment = 0.01;

    unsigned int r, c; 

    const unsigned int dim = 12;

    const fvector3 grid_start_offset_position = { 100.0f, 100.0f, 0.0f };

    for (r = 0; r <= dim; r++)
    {
        for (c = 0; c <= dim; c += 3)
        {

            const fvector3 quad_position     = { grid_start_offset_position.comp.x + (spacing*c)    , grid_start_offset_position.comp.y + (spacing*r), 0.0f};
            const fvector3 triangle_position = { grid_start_offset_position.comp.x + (spacing*(c+1)), grid_start_offset_position.comp.y + (spacing*r), 0.0f};
            const fvector3 circle_position   = { grid_start_offset_position.comp.x + (spacing*(c+2)), grid_start_offset_position.comp.y + (spacing*r), 0.0f};

            const float val = (color_increment*(float)(r*dim + c));

            const fvector4 quad_color     = {  val, 0.0f, 0.0f, 1.0f };
            const fvector4 triangle_color = { 0.0f,  val, 0.0f, 1.0f };
            const fvector4 circle_color   = { 0.0f, 0.0f,  val, 1.0f };

            renderer_2d_draw_quad(&window->renderer, quad_position, quad_color);
            renderer_2d_draw_triangle(&window->renderer, triangle_position, triangle_color);
            renderer_2d_draw_circle(&window->renderer, circle_position, circle_color);
        }
    }

    renderer_2d_end_scene(&window->renderer);

    window_graphics_run(window);

    return window->metadata.signaled_close;
}

void window_cleanup(struct window_t* window)
{
    free(window->metadata.tag);
    window->metadata.tag = 0;

    renderer_2d_cleanup(&window->renderer);

    window_graphics_cleanup(window);
}

void window_set_background_color(struct window_t* window, const fvector4 color)
{
    window_graphics_set_background_color(window, color);
}