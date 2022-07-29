#include <memory.h>
#ifdef DEBUG
   #include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "renderer_2d.h"

void renderer_2d_init(struct renderer_2d_t* renderer, const char* tag, const float left, const float right, const float top, const float bottom, const float near_plane, const float far_plane)
{
   memset(renderer, 0, sizeof(struct renderer_2d_t));

   int tag_length = strlen(tag);
   renderer->tag = (char*) malloc(tag_length*sizeof(char));    
   strcpy(renderer->tag, tag);

   render_api_init();

   const fvector3 camera_position = (fvector3) { {0.0f, 0.0f,  2.0f} };
   const fvector3 camera_up       = (fvector3) { {0.0f, 1.0f,  0.0f} };
   const fvector3 camera_front    = (fvector3) { {0.0f, 0.0f, -1.0f} };

   camera_init_orthographic(&(renderer->camera), camera_position, camera_up, camera_front);
   camera_set_projection_orthographic(&(renderer->camera), left, right, top, bottom, near_plane, far_plane);
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

   static fmatrix_4x4 *model_ptr, *view_ptr, *projection_ptr;

   model_ptr      = &submission->model_matrix;
   view_ptr       = &renderer->camera.base.view_matrix;
   projection_ptr = &renderer->camera.base.projection_matrix;

   shader_program_set_uniform_fmat4x4(submission->shader_program, "model", model_ptr);
   shader_program_set_uniform_fmat4x4(submission->shader_program, "view", view_ptr);
   shader_program_set_uniform_fmat4x4(submission->shader_program, "projection", projection_ptr);

   render_api_draw_elements(DRAW_TYPE_TRIANGLES, submission->num_indices, ELEMENT_UNSIGNED_INT, 0);
}

void renderer_2d_end(struct renderer_2d_t* renderer)
{
   vertex_array_unbind();
}