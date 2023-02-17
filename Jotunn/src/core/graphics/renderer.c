#include "renderer.h"

#include <memory.h>
#include <stdlib.h> 

void renderer_base_init(struct renderer_base_t* renderer, struct camera_base_t* camera, struct window_t* parent_window, const char* tag)
{
    renderer->parent_window = parent_window;
    
    int tag_length = strlen(tag);
    renderer->tag = (char*) malloc(tag_length*sizeof(char));    
    strcpy(renderer->tag, tag);

    renderer->camera = camera;
}

void renderer_base_cleanup(struct renderer_base_t* renderer)
{
    free(renderer->tag);
    renderer->tag = 0;
}

void renderer_base_begin_scene(struct renderer_base_t* renderer)
{
    renderer->renderer_begin_scene((void*)renderer);
}

void renderer_base_end_scene(struct renderer_base_t* renderer)
{
    renderer->renderer_end_scene((void*)renderer);
}