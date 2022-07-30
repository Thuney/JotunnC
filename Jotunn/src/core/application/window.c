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

    // Triangle
    fvector4 triangle_color;
    fvector4_set(&triangle_color, 0.0f, 1.0f, 0.0f, 1.0f);

    const float triangle_scale_factor = 50.0f;
    const fvector3 triangle_scale_factors      = (fvector3) { {triangle_scale_factor, triangle_scale_factor, triangle_scale_factor} };
    const fvector3 triangle_translation_coords = (fvector3) { {400.0f, 500.0f, 0.0f} };

    rgba_triangle_2d_init(&window->triangle, triangle_color);

    fmatrix_4x4 triangle_transform_matrix;

    fmatrix_4x4_init(&triangle_transform_matrix);
    triangle_transform_matrix = fmatrix_4x4_transform_scale(&triangle_transform_matrix, triangle_scale_factors);
    triangle_transform_matrix = fmatrix_4x4_transform_translate(&triangle_transform_matrix, triangle_translation_coords);

    window->triangle.renderable_data.model_matrix = triangle_transform_matrix;

    // Square
    fvector4 square_color;
    fvector4_set(&square_color, 1.0f, 1.0f, 0.0f, 1.0f);

    const float square_scale_factor = 25.0f;
    const fvector3 square_scale_factors      = (fvector3) { {square_scale_factor, square_scale_factor, square_scale_factor} };
    const fvector3 square_translation_coords = (fvector3) { {200.0f, 250.0f, 0.0f} };

    rgba_square_2d_init(&window->square, square_color);

    fmatrix_4x4 square_transform_matrix;

    fmatrix_4x4_init(&square_transform_matrix);
    square_transform_matrix = fmatrix_4x4_transform_scale(&square_transform_matrix, square_scale_factors);
    square_transform_matrix = fmatrix_4x4_transform_translate(&square_transform_matrix, square_translation_coords);

    window->square.renderable_data.model_matrix = square_transform_matrix;

    return error;
}

int window_run(struct window_t* window)
{
    renderer_2d_begin(&window->renderer);

    renderer_2d_submit_2d_prim(&window->renderer, &window->triangle.renderable_data);
    renderer_2d_submit_2d_prim(&window->renderer, &window->square.renderable_data);

    renderer_2d_end(&window->renderer);

    window_graphics_run(window);

    return window->metadata.signaled_close;
}

void window_cleanup(struct window_t* window)
{
    free(window->metadata.tag);
    window->metadata.tag = 0;

    renderable_2d_cleanup(&window->triangle.renderable_data);
    renderable_2d_cleanup(&window->square.renderable_data);

    renderer_2d_cleanup(&window->renderer);

    window_graphics_cleanup(window);
}

void window_set_background_color(struct window_t* window, const fvector4 color)
{
    window_graphics_set_background_color(window, color);
}