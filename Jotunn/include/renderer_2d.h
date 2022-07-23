#pragma once

#include "camera.h"
#include "render_api.h"
#include "renderable_2d_primitive.h"
#include "shader.h"

struct renderer_2d_t
{
   char* tag;
   // struct camera_base_t camera;
};

void renderer_2d_init(struct renderer_2d_t* renderer, char* tag);
void renderer_2d_cleanup(struct renderer_2d_t* renderer);

void renderer_2d_begin(struct renderer_2d_t* renderer);
void renderer_2d_submit_2d_prim(struct renderer_2d_t* renderer, struct renderable_2d_t* submission);
void renderer_2d_end(struct renderer_2d_t* renderer);