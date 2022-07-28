#pragma once

#include "camera.h"
#include "render_api.h"
#include "renderable_2d_primitive.h"
#include "shader.h"

struct renderer_2d_t
{
   char* tag;
   struct camera_ortho_t camera;
};

void renderer_2d_init(struct renderer_2d_t* renderer, const char* tag, const float left, const float right, const float top, const float bottom, const float near_plane, const float far_plane);
void renderer_2d_cleanup(struct renderer_2d_t* renderer);

void renderer_2d_begin(struct renderer_2d_t* renderer);
void renderer_2d_submit_2d_prim(struct renderer_2d_t* renderer, struct renderable_2d_t* submission);
void renderer_2d_end(struct renderer_2d_t* renderer);