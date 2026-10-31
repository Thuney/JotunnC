#include "renderer.h"

#include "../diagnostics/diagnostics_internal.h"

#include <memory.h>
#include <stdlib.h>

/*******************************************************************************
 *
 *   Function: renderer_base_init()
 *
 ******************************************************************************/
void renderer_base_init(
  struct renderer_base_t* out_renderer,
  uint16_t in_render_flags,
  struct camera_base_t* in_camera,
  struct window_t* in_parent_window,
  const char* in_tag)
{
  out_renderer->parent_window = in_parent_window;

  out_renderer->tag = in_tag;

  render_api_init(
    &(out_renderer->render_api),
    in_render_flags);

  out_renderer->camera = in_camera;
}

/*******************************************************************************
 *
 *   Function: renderer_base_cleanup()
 *
 ******************************************************************************/
void renderer_base_cleanup(
  struct renderer_base_t* inout_renderer)
{
  inout_renderer->tag = NULL;
}

/*******************************************************************************
 *
 *   Function: renderer_base_begin_scene()
 *
 ******************************************************************************/
void renderer_base_begin_scene(
  struct renderer_base_t* inout_renderer)
{
  diagnostics_internal_scene_begin(inout_renderer);

  render_api_bind(&(inout_renderer->render_api));
  render_api_clear(&(inout_renderer->render_api));
  inout_renderer->renderer_begin_scene(inout_renderer);
}

/*******************************************************************************
 *
 *   Function: renderer_base_end_scene()
 *
 ******************************************************************************/
void renderer_base_end_scene(
  struct renderer_base_t* inout_renderer)
{
  inout_renderer->renderer_end_scene(inout_renderer);

  // Closed after the renderer's own end, which may still draw what it batched
  diagnostics_internal_scene_end();
}
