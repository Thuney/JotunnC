#include "window.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

extern int window_graphics_init(struct window_t* window);
extern void window_graphics_run(struct window_t* window);
extern void window_graphics_cleanup(struct window_t* window);
extern void window_graphics_set_background_color(struct window_t* window, int red, int green, int blue);

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
    
    return window_graphics_init(window);
}

int window_run(struct window_t* window)
{
    window_graphics_run(window);

    return window->metadata.signaled_close;
}

void window_cleanup(struct window_t* window)
{
    free(window->metadata.tag);
    window->metadata.tag = 0;

    window_graphics_cleanup(window);
}

void window_set_background_color(int red, int green, int blue)
{

}