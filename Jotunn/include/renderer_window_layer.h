#pragma once

#include "fmatrix.h"
#include "framebuffer.h"
#include "renderable_2d_primitive.h"
#include "renderer.h"
#include "shader.h"

#define _RENDERER_WINDOW_LAYER_MAX_TEXTURES 16

struct renderer_window_layer_data_t
{
   struct shader_program_t framebuffer_shader;
};

struct renderer_window_layer_t
{
   struct renderer_base_t base;
   //
   struct renderer_window_layer_data_t render_data;
};

void renderer_window_layer_init(struct renderer_window_layer_t* renderer, struct camera_base_t* camera, struct window_t* parent_window, const char* tag);
void renderer_window_layer_cleanup(struct renderer_window_layer_t* renderer);

void renderer_window_layer_begin_scene(void* renderer);
void renderer_window_layer_end_scene(void* renderer);

void renderer_window_layer_draw_framebuffer(struct renderer_window_layer_t* renderer, struct framebuffer_t* framebuffer);