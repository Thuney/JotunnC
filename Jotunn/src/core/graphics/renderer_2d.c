#include <memory.h>
#include <stdlib.h>
#include <string.h>

#include "renderer_2d.h"

void renderer_2d_init(struct renderer_2d_t* renderer, char* tag)
{
   memset(renderer, 0, sizeof(struct renderer_2d_t));

   int tag_length = strlen(tag);
   renderer->tag = (char*) malloc(tag_length*sizeof(char));    
   strcpy(renderer->tag, tag);
}

void renderer_2d_cleanup(struct renderer_2d_t* renderer)
{
   #ifdef DEBUG
        fprintf(stdout, "Cleaning up renderer 2D\n");
    #endif

    free(renderer->tag);
    renderer->tag = 0;
}

void renderer_2d_begin(struct renderer_2d_t* renderer)
{

}

void renderer_2d_submit(struct renderer_2d_t* renderer)
{

}

void renderer_2d_end(struct renderer_2d_t* renderer)
{

}