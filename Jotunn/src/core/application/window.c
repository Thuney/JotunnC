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

#define NUM_WINDOW_QUADS 100
static const float increment = 1.0f/(float)NUM_WINDOW_QUADS;

static const unsigned int spacing = 10.0f, offset = 50.0f;

fvector3 quad_positions[NUM_WINDOW_QUADS];
fvector4 quad_colors[NUM_WINDOW_QUADS];

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

    // renderer_2d_init(&window->renderer, "2DRenderer", 0.0f, (float)width, (float)height, 0.0f, -1.0f, 100.0f);
    renderer_2d_init(&window->renderer, "2DRenderer", -(float)width/2.0f, (float)width/2.0f, (float)height/2.0f, -(float)height/2.0f, -1.0f, 100.0f);

    int r, c;
    for (r = 0; r < NUM_WINDOW_QUADS % 10; r++)
    {
        for (c = 0; c < NUM_WINDOW_QUADS % 10; c++)
        {
            quad_positions[r*10 + c] = (fvector3) { { c*spacing + offset, r*spacing + offset, 0.0f } };
            quad_colors[r*10 + c]    = (fvector4) { { c*increment, r*increment, 0.0f, 1.0f } };
        }
    }

    return error;
}

int window_run(struct window_t* window)
{
    renderer_2d_begin_scene(&window->renderer);

    for (int i = 0; i < NUM_WINDOW_QUADS; i++)
    {
        renderer_2d_draw_quad(&window->renderer, quad_positions[i], &quad_colors[i]);
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