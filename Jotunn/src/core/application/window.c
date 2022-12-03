#include "application.h"
#include "fvector.h"
#include "render_api.h"
#include "window.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

extern uint8_t window_graphics_init(struct window_t* window);
extern void window_graphics_run(struct window_t* window);
extern void window_graphics_cleanup(struct window_t* window);
extern void window_graphics_set_context(struct window_t* window);
extern void window_graphics_release_context();
extern void window_graphics_set_background_color(struct window_t* window, const fvector4 color);

// Window Callbacks

// Helpers

static void window_set_metadata(struct window_data_t* metadata, const uint32_t width, const uint32_t height, const char* tag, struct application_t* app_parent, void (*function_event_notify)(struct application_t*, struct event_base_t*))
{
    // Store our window tag
    int tag_length = strlen(tag);
    metadata->tag = (char*) malloc(tag_length*sizeof(char));    
    strcpy(metadata->tag, tag);

    // Assign window properties
    metadata->width  = width;
    metadata->height = height;

    metadata->signaled_close = 0;
    metadata->visible = 1;

    metadata->parent_application    = app_parent;
    metadata->function_event_notify = function_event_notify;
}

// Exposed functions

uint8_t window_init(struct window_t* window, const uint32_t width, const uint32_t height, const char* tag, struct application_t* app_parent)
{
    memset((void*)window, 0, sizeof(struct window_t));

    window_set_metadata(&window->metadata, width, height, tag, app_parent, &application_on_event);

    uint8_t error = window_graphics_init(window);

    #ifdef DEBUG
        if (error) fprintf(stdout, "Error during window_graphics_init\n");
    #endif

    // fvector4 background_color;
    // fvector4_set(&background_color, 1.0f, 0.1f, 0.1f, 1.0f);

    // window_set_background_color(window, background_color);

    window->function_custom_window_run = 0;

    renderer_2d_init(&window->renderer_2d, window, "renderer_2d", 0, width, height, 0, -3.0f, 100.0f);

    return error;
}

// ---------------------------
// ---------------------------
// ---------------------------
// ---------------------------
// ---------------------------

uint8_t window_run(struct window_t* window)
{
    // #ifdef DEBUG
    //     fprintf(stdout, "Running window\n");
    // #endif

    window_set_context(window);
    
    renderer_2d_begin_scene(&window->renderer_2d);

    if (window->function_custom_window_run)
    {
        window->function_custom_window_run(window);
    }

    renderer_2d_end_scene(&window->renderer_2d);

    window_graphics_run(window);

    window_release_context();

    return window->metadata.signaled_close;
}

void window_cleanup(struct window_t* window)
{
    free(window->metadata.tag);
    window->metadata.tag = 0;

    renderer_2d_cleanup(&window->renderer_2d);

    window_graphics_cleanup(window);
}

void window_set_context(struct window_t* window)
{
    window_graphics_set_context(window);
}

void window_release_context()
{
    window_graphics_release_context();
}

void window_set_background_color(struct window_t* window, const fvector4 color)
{
    window_set_context(window);
    window_graphics_set_background_color(window, color);
    window_release_context();
}

void window_set_function_custom_window_run(struct window_t* window, void (*function_custom_window_run)(struct window_t* window))
{
    window->function_custom_window_run = function_custom_window_run;
}