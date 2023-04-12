#include "application.h"
#include "fvector.h"
#include "render_api.h"
#include "window.h"

#include <memory.h>

#ifdef DEBUG
    #include <stdio.h>
#endif

#include <stdlib.h> 
#include <string.h>

extern uint8_t window_graphics_init(struct window_t* window);
extern void window_graphics_poll_events(struct window_t* window);
extern void window_graphics_run(struct window_t* window);
extern void window_graphics_cleanup(struct window_t* window);
extern void window_graphics_set_context(struct window_t* window);
extern void window_graphics_release_context();
extern void window_graphics_set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
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

    if (!error)
    {
        window->function_custom_window_run = 0;
        window->function_event_react = 0;
        window->camera   = 0;
        window->renderer = 0;

        window_set_context(window);

        renderer_window_layer_init(&window->window_layer_renderer, NULL, window, "Window Renderer");

        window_release_context();
    }

    return error;
}

void window_set_renderer(struct window_t* window, struct renderer_base_t* renderer, struct camera_base_t* camera)
{
    window->renderer = renderer;
    window->camera   = camera;
}

void window_bind_custom_events(struct window_t* window, void (*custom_event_function)(struct window_t*, struct event_base_t*))
{
    window->function_event_react = custom_event_function;
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

    window_graphics_poll_events(window);

    window_set_context(window);

    if (window->metadata.resized)
    {
        window_set_viewport(0, 0, window->metadata.width, window->metadata.height);
        window->camera->camera_reproject(window->camera, (float)window->metadata.width, (float)window->metadata.height, window->camera->near_plane, window->camera->far_plane);
        window->metadata.resized = 0;
    }
    
    renderer_base_begin_scene(window->renderer);

    if (window->function_custom_window_run)
    {
        window->function_custom_window_run(window);
    }

    renderer_base_end_scene(window->renderer);

    window_graphics_run(window);

    window_release_context();

    return window->metadata.signaled_close;
}

void window_cleanup(struct window_t* window)
{
    free(window->metadata.tag);
    window->metadata.tag = 0;

    renderer_window_layer_cleanup(&(window->window_layer_renderer));

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

void window_set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    window_graphics_set_viewport(x, y, width, height);
}

void window_set_background_color(struct window_t* window, const fvector4 color)
{
    window_graphics_set_background_color(window, color);
}

void window_set_function_custom_window_run(struct window_t* window, void (*function_custom_window_run)(struct window_t* window))
{
    window->function_custom_window_run = function_custom_window_run;
}