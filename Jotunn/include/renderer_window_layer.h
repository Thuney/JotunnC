#pragma once

#include "fmatrix.h"
#include "framebuffer.h"
#include "renderable_2d_primitive.h"
#include "renderer.h"
#include "shader.h"

struct window_layer_t;

#define _RENDERER_WINDOW_LAYER_MAX_TEXTURES 16

struct renderer_window_layer_data_t
{
   struct shader_program_t framebuffer_shader;

   struct vertex_array_t vao;
   struct vertex_buffer_t vbo;
   struct element_buffer_t ebo;
};

struct renderer_window_layer_t
{
   struct renderer_base_t base;
   //
   struct camera_base_t camera_unprojected;
   struct renderer_window_layer_data_t render_data;
};

void renderer_window_layer_init(struct renderer_window_layer_t* renderer, struct window_t* parent_window, const char* tag);
void renderer_window_layer_cleanup(struct renderer_window_layer_t* renderer);

void renderer_window_layer_begin_scene(void* renderer);
void renderer_window_layer_end_scene(void* renderer);

void renderer_window_layer_draw_layer(struct renderer_window_layer_t* renderer, struct window_layer_t *window_layer, uint8_t layer_number);