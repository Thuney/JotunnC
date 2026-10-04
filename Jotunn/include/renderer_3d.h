#pragma once

struct window_t;

#include "fmatrix.h"
#include "renderable_3d_primitive.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"

#define _RENDERER_3D_MAX_TEXTURES 16

// A single directional light, applied to all shaded and textured geometry
struct renderer_3d_light_t
{
  // Direction the light travels, in world space
  fvector3 direction;
  fvector3 color;
  // Fraction of the light color applied regardless of surface orientation
  float ambient_strength;
};

struct renderer_3d_data_t
{
  fmatrix_4x4 model_matrix;

  struct shader_program_t shaded_shader;
  struct shader_program_t textured_shader;
  struct shader_program_t line_shader;
  //
  struct texture_2d_t* textures[_RENDERER_3D_MAX_TEXTURES];
  unsigned int textures_index;
  //
  struct renderable_3d_shaded_data_t cube_data;
  struct renderable_3d_shaded_data_t pyramid_data;
  struct renderable_3d_shaded_data_t sphere_data;
  struct renderable_3d_textured_data_t textured_cube_data;
  struct renderable_3d_line_data_t line_data;
  //
  struct renderer_3d_light_t light;
  //
  float line_width;
};

struct renderer_3d_t
{
  struct renderer_base_t base;
  //
  struct renderer_3d_data_t render_data;
};

void renderer_3d_init(
  struct renderer_3d_t* renderer,
  struct camera_base_t* camera,
  struct window_t* parent_window,
  const char* tag);
void renderer_3d_cleanup(
  struct renderer_3d_t* renderer);

void renderer_3d_begin_scene(
  void* renderer);
void renderer_3d_end_scene(
  void* renderer);

// Primitives are unit sized and centered on the origin before the transform
//  is applied (the sphere has a radius of 1)
void renderer_3d_draw_cube(
  struct renderer_3d_t* renderer,
  const fmatrix_4x4* transform,
  const fvector4 color);
void renderer_3d_draw_unfilled_cube(
  struct renderer_3d_t* renderer,
  const fmatrix_4x4* transform,
  const fvector4 color);
void renderer_3d_draw_textured_cube(
  struct renderer_3d_t* renderer,
  const fmatrix_4x4* transform,
  struct texture_2d_t* texture);

void renderer_3d_draw_pyramid(
  struct renderer_3d_t* renderer,
  const fmatrix_4x4* transform,
  const fvector4 color);
void renderer_3d_draw_sphere(
  struct renderer_3d_t* renderer,
  const fmatrix_4x4* transform,
  const fvector4 color);
void renderer_3d_draw_line(
  struct renderer_3d_t* renderer,
  const fvector3 pos_1,
  const fvector3 pos_2,
  const fvector4 color);

void renderer_3d_set_line_width(
  struct renderer_3d_t* renderer,
  const float line_width);
void renderer_3d_set_light(
  struct renderer_3d_t* renderer,
  const fvector3 direction,
  const fvector3 color,
  const float ambient_strength);
