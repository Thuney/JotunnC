#include <memory.h>
#ifdef DEBUG
   #include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "renderer_2d.h"

void renderer_2d_init(struct renderer_2d_t* renderer, char* tag)
{
   memset(renderer, 0, sizeof(struct renderer_2d_t));

   int tag_length = strlen(tag);
   renderer->tag = (char*) malloc(tag_length*sizeof(char));    
   strcpy(renderer->tag, tag);

   render_api_init();
}

void renderer_2d_cleanup(struct renderer_2d_t* renderer)
{
   #ifdef DEBUG
        fprintf(stdout, "Cleaning up renderer 2D\n");
   #endif

   free(renderer->tag);
   renderer->tag = 0;

   vertex_array_unbind();
}

void renderer_2d_begin(struct renderer_2d_t* renderer)
{
   render_api_clear();
}

void renderer_2d_submit_2d_prim(struct renderer_2d_t* renderer, struct renderable_2d_t* submission)
{
   vertex_array_bind(&submission->vao);
   shader_program_use(submission->shader_program);
   render_api_draw_elements(DRAW_TYPE_TRIANGLES, submission->num_indices, ELEMENT_UNSIGNED_INT, 0);

   vertex_array_unbind();
}

void renderer_2d_end(struct renderer_2d_t* renderer)
{
   vertex_array_unbind();
}