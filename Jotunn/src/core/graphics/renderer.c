#include "renderer.h"

#include "allocator.h"

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
  const uint16_t in_render_flags,
  struct camera_base_t* in_camera,
  struct window_t* in_parent_window,
  const char* in_tag)
{
  out_renderer->parent_window = in_parent_window;

  out_renderer->tag =
    allocator_duplicate_string(
      in_tag,
      ALLOCATOR_TAG_RENDERER);

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
  allocator_free(inout_renderer->tag);
  inout_renderer->tag = 0;
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
  inout_renderer->renderer_begin_scene((void*)inout_renderer);
}

/*******************************************************************************
 *
 *   Function: renderer_base_end_scene()
 *
 ******************************************************************************/
void renderer_base_end_scene(
  struct renderer_base_t* inout_renderer)
{
  inout_renderer->renderer_end_scene((void*)inout_renderer);

  // Closed after the renderer's own end, which may still draw what it batched
  diagnostics_internal_scene_end();
}
