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

uint8_t window_init(struct window_t* window, const uint8_t max_window_layers, const uint32_t width, const uint32_t height, const char* tag, struct application_t* app_parent)
{
    memset((void*)window, 0, sizeof(struct window_t));

    window_set_metadata(&window->metadata, width, height, tag, app_parent, &application_on_event);

    uint8_t error = window_graphics_init(window);

    #ifdef DEBUG
        if (error) fprintf(stdout, "Error during window_graphics_init\n");
    #endif

    if (!error)
    {
        window->function_event_react = 0;

        window_set_context(window);

        renderer_window_layer_init(&window->window_layer_renderer, window, "Window Renderer");

        window->max_layers = max_window_layers;
        window->num_layers = 0;
        window->layers = (struct window_layer_t**) malloc(window->max_layers*sizeof(struct window_layer_t*));
    }

    return error;
}

uint8_t window_add_layer(struct window_t* window, struct window_layer_t* new_window_layer)
{
    #ifdef DEBUG
        fprintf(stdout, "Adding layer to window\n");
    #endif  

    uint8_t error = 0U;

    if (window->num_layers < window->max_layers)
    {
        struct window_layer_t** layer_ptr;
        layer_ptr = (window->layers + window->num_layers);
        *layer_ptr = new_window_layer;
        window->num_layers++;
    }
    else
    {
        error |= 1U;

        #ifdef DEBUG
            fprintf(stdout, "Error adding layer to window\n");
        #endif  
    }

    return error;
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

    window_set_context(window);
    window_graphics_poll_events(window);

    struct window_layer_t** current_layer;

    // Render our layers to their respective framebuffers, bottom to top    
    current_layer = window->layers;
    for (uint8_t i = 0; i < window->num_layers; i++, current_layer++)
    {
        framebuffer_bind(((*current_layer)->framebuffer));

        if (window->metadata.resized)
        {
            // framebuffer_unbind();
            // framebuffer_bind(((*current_layer)->framebuffer));
            framebuffer_resize(((*current_layer)->framebuffer), window->metadata.width, window->metadata.height);
            
            window_set_viewport(0, 0, window->metadata.width, window->metadata.height);
            (*current_layer)->camera->camera_reproject((*current_layer)->camera, (float)window->metadata.width, (float)window->metadata.height, (*current_layer)->camera->near_plane, (*current_layer)->camera->far_plane);
        }

        renderer_base_begin_scene(((*current_layer)->renderer));

        if ((*current_layer)->function_custom_window_layer_run)
        {
            (*current_layer)->function_custom_window_layer_run(window, *(current_layer));
        }

        renderer_base_end_scene(((*current_layer)->renderer));
    }

    if (window->metadata.resized)
    {
        window->metadata.resized = 0;
    }
    
    // Render our layers to the default (window) framebuffer, bottom to top

    renderer_window_layer_begin_scene(&(window->window_layer_renderer));

    current_layer = window->layers;
    for (uint8_t i = 0; i < window->num_layers; i++, current_layer++)
    {
        renderer_window_layer_draw_layer(&(window->window_layer_renderer), (*current_layer), i);
    }

    renderer_window_layer_end_scene(&(window->window_layer_renderer));

    window_graphics_run(window);

    window_release_context();

    return (window->metadata.signaled_close);
}

void window_cleanup(struct window_t* window)
{
    free(window->metadata.tag);
    window->metadata.tag = 0;

    renderer_window_layer_cleanup(&(window->window_layer_renderer));

    window->max_layers = 0;
    window->num_layers = 0;
    free(window->layers);

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