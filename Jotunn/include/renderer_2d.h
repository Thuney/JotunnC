#pragma once

#include "camera.h"
#include "render_api.h"
#include "shader.h"
#include "texture.h"

struct renderer_2d_t
{
   char* tag;
   struct camera_ortho_t camera;
};

void renderer_2d_init(struct renderer_2d_t* renderer, const char* tag, const float left, const float right, const float top, const float bottom, const float near_plane, const float far_plane);
void renderer_2d_cleanup(struct renderer_2d_t* renderer);

void renderer_2d_begin_scene(struct renderer_2d_t* renderer);
void renderer_2d_end_scene(struct renderer_2d_t* renderer);

void renderer_2d_draw_triangle(const struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const fvector4 color);
void renderer_2d_draw_quad(const struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const fvector4 color);
void renderer_2d_draw_textured_quad(const struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const struct texture_2d_t* texture);
void renderer_2d_draw_subtextured_quad(const struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const struct texture_2d_t* texture, const fvector2 subtexture_coords[4]);
void renderer_2d_draw_circle(const struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const fvector4 color);
void renderer_2d_draw_line(const struct renderer_2d_t* renderer, const fvector3 pos_1, const fvector3 pos_2, const fvector4 color);
void renderer_2d_draw_string(const struct renderer_2d_t* renderer, const struct typeface_t* typeface, const fvector3 start_position, const char* draw_string);

void renderer_2d_set_line_width(const struct renderer_2d_t* renderer, const float line_width);