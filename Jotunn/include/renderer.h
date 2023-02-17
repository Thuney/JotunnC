#pragma once

struct window_t;

#include "camera.h"
#include "render_api.h"

struct renderer_base_t
{
   struct window_t* parent_window;
   //
   char* tag;
   //
   struct camera_base_t* camera;
   //
   void (*renderer_begin_scene)(void* renderer);
   void (*renderer_end_scene)(void* renderer);
};

void renderer_base_init(struct renderer_base_t* renderer, struct camera_base_t* camera, struct window_t* parent_window, const char* tag);
void renderer_base_cleanup(struct renderer_base_t* renderer);

void renderer_base_begin_scene(struct renderer_base_t* renderer);
void renderer_base_end_scene(struct renderer_base_t* renderer);