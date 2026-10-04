#pragma once

#include "application.h"
#include "camera.h"
#include "event.h"
#include "framebuffer.h"
#include "renderer_3d.h"
#include "texture.h"
#include "window_layer.h"

struct demo_3d_orbit_t
{
  // Angles in radians, distance in world units, all about the orbit target
  float yaw;
  float pitch;
  float distance;
  fvector3 target;
  //
  uint8_t dragging;
  fvector2 last_mouse;
};

struct demo_3d_window_t
{
  // Base window structure - must be first so a window_t* can be cast back
  struct window_t window;
  //
  struct window_layer_t window_layer;
  struct framebuffer_t framebuffer;
  //
  struct camera_perspective_t camera_perspective;
  struct renderer_3d_t renderer_3d;
  //
  struct texture_2d_t cube_texture;
  //
  struct demo_3d_orbit_t orbit;
  float elapsed_seconds;
};

uint8_t demo_3d_window_init(
  struct application_t* app_parent,
  struct demo_3d_window_t* demo_window);
void demo_3d_window_on_event(
  struct window_t* window,
  struct event_base_t* event);

void demo_3d_window_run(
  struct window_layer_t* window_layer);

void demo_3d_window_cleanup(
  struct demo_3d_window_t* demo_window);
