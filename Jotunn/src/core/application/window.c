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

void window_set_metadata(struct window_data_t* metadata, int width, int height, char* tag)
{
    // Store our window tag
    int tag_length = strlen(tag);
    metadata->tag = (char*) malloc(tag_length*sizeof(char));    
    strcpy(metadata->tag, tag);

    // Assign window properties
    metadata->width  = width;
    metadata->height = height;

    metadata->signaled_close = 0;
}

// Exposed functions

int window_init(struct window_t* window, int width, int height, char* tag)
{
    window_set_metadata(&window->metadata, width, height, tag);

    int error = window_graphics_init(window);

    #ifdef DEBUG
        if (error) fprintf(stdout, "Error during window_graphics_init\n");
    #endif

    fvector4 background_color;
    fvector4_set(&background_color, 0.1f, 0.1f, 0.1f, 1.0f);
    window_set_background_color(window, background_color);

    // renderer_2d_init(&window->renderer, "2DRenderer", -100.0f, 100.0f, 100.0f, -100.0f, 0.1f, 100.0f);
    renderer_2d_init(&window->renderer, "2DRenderer", 0.0f, (float)width, (float)height, 0.0f, 0.1f, 100.0f);

    fvector4 triangle_color, square_color;
    fvector4_set(&triangle_color, 0.0f, 1.0f, 0.0f, 1.0f);
    fvector4_set(&square_color, 1.0f, 1.0f, 0.0f, 1.0f);

    rgba_triangle_2d_init(&window->triangle, triangle_color);
    // rgba_square_2d_init(&window->square, square_color);

    // fmatrix_4x4_scale(&window->triangle.renderable_data.model_matrix, 0.1f);
    // fmatrix_4x4_scale(&window->square.renderable_data.model_matrix, 5.0f);

    return error;
}

int window_run(struct window_t* window)
{
    renderer_2d_begin(&window->renderer);

    renderer_2d_submit_2d_prim(&window->renderer, &window->triangle.renderable_data);
    // renderer_2d_submit_2d_prim(&window->renderer, &window->square.renderable_data);

    renderer_2d_end(&window->renderer);

    window_graphics_run(window);

    return window->metadata.signaled_close;
}

void window_cleanup(struct window_t* window)
{
    free(window->metadata.tag);
    window->metadata.tag = 0;

    renderable_2d_cleanup(&window->triangle.renderable_data);
    // renderable_2d_cleanup(&window->square.renderable_data);

    renderer_2d_cleanup(&window->renderer);

    window_graphics_cleanup(window);
}

void window_set_background_color(struct window_t* window, const fvector4 color)
{
    window_graphics_set_background_color(window, color);
}