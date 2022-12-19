#pragma once

struct window_t;

#include "camera.h"
#include "fmatrix.h"
#include "font.h"
#include "render_api.h"
#include "renderable_2d_primitive.h"
#include "shader.h"
#include "texture.h"

#define _RENDERER_2D_MAX_TEXTURES 16

struct renderer_2d_data_t
{
   fmatrix_4x4 model_matrix;

   struct shader_program_t triangle_shader;
   struct shader_program_t quad_shader;
   struct shader_program_t textured_quad_shader;
   struct shader_program_t circle_shader;
   struct shader_program_t line_shader;
   //
   struct texture_2d_t* textures[_RENDERER_2D_MAX_TEXTURES];
   unsigned int textures_index;
   //
   struct renderable_2d_triangle_data_t      triangle_data;
   struct renderable_2d_quad_data_t          quad_data;
   struct renderable_2d_textured_quad_data_t textured_quad_data;
   struct renderable_2d_circle_data_t        circle_data;
   struct renderable_2d_line_data_t          line_data;
   //
   float line_width;
};

struct renderer_2d_t
{
   struct window_t* parent_window;
   //
   char* tag;
   //
   struct camera_ortho_t camera;
   //
   struct renderer_2d_data_t render_data;
};

void renderer_2d_init(struct renderer_2d_t* renderer, struct window_t* parent_window, const char* tag, const float left, const float right, const float top, const float bottom, const float near_plane, const float far_plane);
void renderer_2d_cleanup(struct renderer_2d_t* renderer);

void renderer_2d_begin_scene(struct renderer_2d_t* renderer);
void renderer_2d_end_scene(struct renderer_2d_t* renderer);

void renderer_2d_flush(struct renderer_2d_t* renderer);

void renderer_2d_draw_triangle(struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const fvector4 color);
void renderer_2d_draw_quad(struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const fvector4 color);
void renderer_2d_draw_textured_quad(struct renderer_2d_t* renderer, const fmatrix_4x4* transform, struct texture_2d_t* texture);
void renderer_2d_draw_subtextured_quad(struct renderer_2d_t* renderer, const fmatrix_4x4* transform, struct texture_2d_t* texture, const fvector2 subtexture_coords[4]);
void renderer_2d_draw_circle(struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const fvector4 color);
void renderer_2d_draw_line(struct renderer_2d_t* renderer, const fvector3 pos_1, const fvector3 pos_2, const fvector4 color);
void renderer_2d_draw_string(struct renderer_2d_t* renderer, const struct typeface_t* typeface, const fvector3 start_position, const char* draw_string);

void renderer_2d_set_line_width(struct renderer_2d_t* renderer, const float line_width);