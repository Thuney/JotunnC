#include "renderer_2d.h"
#include "log.h"

#include "framebuffer.h"
#include "util.h"
#include "window.h"

#include <memory.h>
#ifdef DEBUG
  #include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Draws everything in the batch, a draw call per kind of primitive in it, in
//  the order triangles, quads, textured quads, circles, lines. The batch is
//  left as it was
//
//  in:     in_renderer
static void renderer_2d_flush(
  struct renderer_2d_t* in_renderer);

// Empties the batch
//
//  in/out: inout_renderer
static void renderer_2d_start_batch(
  struct renderer_2d_t* inout_renderer);

// Draws everything in the batch, then empties it
//
//  in/out: inout_renderer
static void renderer_2d_next_batch(
  struct renderer_2d_t* inout_renderer);

// Loads the shaders and creates the batches, the mesh buffers and the white
//  texture. Shaders that fail to load are logged in DEBUG builds
//
//  out:    out_data
static void renderer_2d_data_init(
  struct renderer_2d_data_t* out_data);

// Releases everything renderer_2d_data_init() made
//
//  in/out: inout_data
static void renderer_2d_data_cleanup(
  struct renderer_2d_data_t* inout_data);

// Finds the size of one index of a type
//
//  in:     in_index_type
//  ret:    The index's size in bytes; that of a 32-bit index for a type that
//            isn't an index type
static uint32_t _renderer_2d_get_index_size_bytes(
  const enum render_api_element_data_type_t in_index_type);

/*******************************************************************************
 *
 *   Function: renderer_2d_flush()
 *
 ******************************************************************************/
static void renderer_2d_flush(
  struct renderer_2d_t *in_renderer)
{
  struct renderer_2d_data_t *data = &in_renderer->render_data;

  struct camera_orthographic_t *camera_ortho =
    (struct camera_orthographic_t *)in_renderer->base.camera;

  // #ifdef DEBUG
  //    fprintf(stdout, "Flushing renderer - \n");
  //    fprintf(stdout, "Triangle Index Count = %d\n", data->triangle_data.triangle_index_count);
  //    fprintf(stdout, "Quad Index Count = %d\n", data->quad_data.quad_index_count);
  //    fprintf(stdout, "Textured Quad Index Count = %d\n", data->textured_quad_data.textured_quad_index_count);
  //    fprintf(stdout, "Circle Index Count = %d\n", data->circle_data.circle_index_count);
  //    fprintf(stdout, "Line Index Count = %d\n\n", data->line_data.line_vertex_count);
  // #endif

  if (data->triangle_data.triangle_index_count)
  {
    const unsigned int triangle_data_size =
      (unsigned int)((uint8_t *)data->triangle_data.vertex_data_ptr -
                     (uint8_t *)data->triangle_data.vertex_data_base);

    vertex_array_bind(&(data->triangle_data.vao));
    vertex_buffer_bind(&(data->triangle_data.vbo));
    element_buffer_bind(&(data->triangle_data.ebo));

    vertex_buffer_set_sub_data(
      &(data->triangle_data.vbo),
      (float *)data->triangle_data.vertex_data_base,
      triangle_data_size);

    shader_program_use(&(data->triangle_shader));

    shader_program_set_uniform_fmat4x4(
      &(data->triangle_shader), "model", &in_renderer->render_data.model_matrix);
    shader_program_set_uniform_fmat4x4(
      &(data->triangle_shader), "view", &in_renderer->base.camera->view_matrix);
    shader_program_set_uniform_fmat4x4(
      &(data->triangle_shader),
      "projection",
      &(in_renderer->base.camera->projection_matrix));

    render_api_draw_elements(
      RENDER_API_DRAW_MODE_TRIANGLES,
      data->triangle_data.triangle_index_count,
      RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_INT,
      0);
  }

  if (data->quad_data.quad_index_count)
  {
    const unsigned int quad_data_size =
      (unsigned int)((uint8_t *)data->quad_data.vertex_data_ptr -
                     (uint8_t *)data->quad_data.vertex_data_base);

    vertex_array_bind(&data->quad_data.vao);
    vertex_buffer_bind(&data->quad_data.vbo);
    element_buffer_bind(&data->quad_data.ebo);

    vertex_buffer_set_sub_data(
      &data->quad_data.vbo,
      (float *)data->quad_data.vertex_data_base,
      quad_data_size);

    shader_program_use(&data->quad_shader);

    shader_program_set_uniform_fmat4x4(
      &data->quad_shader, "model", &in_renderer->render_data.model_matrix);
    shader_program_set_uniform_fmat4x4(
      &data->quad_shader, "view", &in_renderer->base.camera->view_matrix);
    shader_program_set_uniform_fmat4x4(
      &data->quad_shader,
      "projection",
      &in_renderer->base.camera->projection_matrix);

    render_api_draw_elements(
      RENDER_API_DRAW_MODE_TRIANGLES,
      data->quad_data.quad_index_count,
      RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_INT,
      0);
  }

  if (data->textured_quad_data.textured_quad_index_count)
  {
    const unsigned int textured_quad_data_size =
      (unsigned int)((uint8_t *)data->textured_quad_data.vertex_data_ptr -
                     (uint8_t *)data->textured_quad_data.vertex_data_base);

    vertex_array_bind(&data->textured_quad_data.vao);
    vertex_buffer_bind(&data->textured_quad_data.vbo);
    element_buffer_bind(&data->textured_quad_data.ebo);

    vertex_buffer_set_sub_data(
      &data->textured_quad_data.vbo,
      (float *)data->textured_quad_data.vertex_data_base,
      textured_quad_data_size);

    shader_program_use(&data->textured_quad_shader);

    for (unsigned int i = 0; i < data->textures_index; i++)
    {
      const struct texture_2d_t *texture = data->textures[i];

      // #ifdef DEBUG
      //    fprintf(stdout, "Binding texture with texture_id = %u to slot %u\n", texture->texture_id, i);
      // #endif

      texture_2d_bind(texture, i);
    }

    shader_program_set_uniform_fmat4x4(
      &data->textured_quad_shader,
      "model",
      &in_renderer->render_data.model_matrix);
    shader_program_set_uniform_fmat4x4(
      &data->textured_quad_shader, "view", &in_renderer->base.camera->view_matrix);
    shader_program_set_uniform_fmat4x4(
      &data->textured_quad_shader,
      "projection",
      &in_renderer->base.camera->projection_matrix);

    char texture_uniform_name_buf[16];

    for (unsigned int i = 0; i < _RENDERER_2D_MAX_TEXTURES; i++)
    {
      sprintf(texture_uniform_name_buf, "u_textures[%u]", i);
      shader_program_set_uniform_int(
        &data->textured_quad_shader, texture_uniform_name_buf, i);
    }

    render_api_draw_elements(
      RENDER_API_DRAW_MODE_TRIANGLES,
      data->textured_quad_data.textured_quad_index_count,
      RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_INT,
      0);
  }

  if (data->circle_data.circle_index_count)
  {
    const unsigned int circle_data_size =
      (unsigned int)((uint8_t *)data->circle_data.vertex_data_ptr -
                     (uint8_t *)data->circle_data.vertex_data_base);

    vertex_array_bind(&data->circle_data.vao);
    vertex_buffer_bind(&data->circle_data.vbo);
    element_buffer_bind(&data->circle_data.ebo);

    vertex_buffer_set_sub_data(
      &data->circle_data.vbo,
      (float *)data->circle_data.vertex_data_base,
      circle_data_size);

    shader_program_use(&data->circle_shader);

    shader_program_set_uniform_fmat4x4(
      &data->circle_shader, "model", &in_renderer->render_data.model_matrix);
    shader_program_set_uniform_fmat4x4(
      &data->circle_shader, "view", &in_renderer->base.camera->view_matrix);
    shader_program_set_uniform_fmat4x4(
      &data->circle_shader,
      "projection",
      &in_renderer->base.camera->projection_matrix);

    render_api_draw_elements(
      RENDER_API_DRAW_MODE_TRIANGLES,
      data->circle_data.circle_index_count,
      RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_INT,
      0);
  }

  if (data->line_data.line_vertex_count)
  {
    const unsigned int line_data_size =
      (unsigned int)((uint8_t *)data->line_data.vertex_data_ptr -
                     (uint8_t *)data->line_data.vertex_data_base);

    vertex_array_bind(&data->line_data.vao);
    vertex_buffer_bind(&data->line_data.vbo);

    vertex_buffer_set_sub_data(
      &data->line_data.vbo,
      (float *)data->line_data.vertex_data_base,
      line_data_size);

    shader_program_use(&data->line_shader);

    shader_program_set_uniform_fmat4x4(
      &data->line_shader, "model", &in_renderer->render_data.model_matrix);
    shader_program_set_uniform_fmat4x4(
      &data->line_shader, "view", &in_renderer->base.camera->view_matrix);
    shader_program_set_uniform_fmat4x4(
      &data->line_shader,
      "projection",
      &in_renderer->base.camera->projection_matrix);

    render_api_set_line_width(data->line_width_pixels);
    render_api_draw_lines(data->line_data.line_vertex_count);
  }
}

/*******************************************************************************
 *
 *   Function: renderer_2d_start_batch()
 *
 ******************************************************************************/
static void renderer_2d_start_batch(
  struct renderer_2d_t *inout_renderer)
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Renderer 2D starting batch\n");
  // #endif

  struct renderer_2d_data_t *data = &inout_renderer->render_data;

  data->triangle_data.triangle_index_count = 0;
  data->triangle_data.vertex_data_ptr = data->triangle_data.vertex_data_base;

  data->quad_data.quad_index_count = 0;
  data->quad_data.vertex_data_ptr = data->quad_data.vertex_data_base;

  data->textured_quad_data.textured_quad_index_count = 0;
  data->textured_quad_data.vertex_data_ptr =
    data->textured_quad_data.vertex_data_base;

  data->circle_data.circle_index_count = 0;
  data->circle_data.vertex_data_ptr = data->circle_data.vertex_data_base;

  data->line_data.line_vertex_count = 0;
  data->line_data.vertex_data_ptr = data->line_data.vertex_data_base;

  data->textures_index = 0;
}

/*******************************************************************************
 *
 *   Function: renderer_2d_next_batch()
 *
 ******************************************************************************/
static void renderer_2d_next_batch(
  struct renderer_2d_t *inout_renderer)
{
  renderer_2d_flush(inout_renderer);
  renderer_2d_start_batch(inout_renderer);
}

// Our upper limit on quads to draw per batch. Translates into a limitation on
//  max vertices / indices, technically, to include things like triangles,
//  circles, and lines
static const unsigned int max_quads = 1000;
static const unsigned int max_vertices = max_quads * 4;
static const unsigned int max_indices = max_quads * 6;

/*******************************************************************************
 *
 *   Function: renderer_2d_data_init()
 *
 ******************************************************************************/
static void renderer_2d_data_init(
  struct renderer_2d_data_t *out_data)
{
  unsigned int error = 0;

  error = shader_program_init_filepath(
    &out_data->triangle_shader,
    PATH_FROM_JOTUNN_ROOT("/res/shaders/triangle_2d_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/triangle_2d_shader.frag"));
#ifdef DEBUG
  if (error)
    log_write(
      LOG_LEVEL_ERROR,
      "Error during triangle shader init");
#endif
  error = shader_program_init_filepath(
    &out_data->quad_shader,
    PATH_FROM_JOTUNN_ROOT("/res/shaders/quad_2d_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/quad_2d_shader.frag"));
#ifdef DEBUG
  if (error)
    log_write(
      LOG_LEVEL_ERROR,
      "Error during quad shader init");
#endif
  error = shader_program_init_filepath(
    &out_data->textured_quad_shader,
    PATH_FROM_JOTUNN_ROOT("/res/shaders/textured_quad_2d_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/textured_quad_2d_shader.frag"));
#ifdef DEBUG
  if (error)
    log_write(
      LOG_LEVEL_ERROR,
      "Error during quad shader init");
#endif
  error = shader_program_init_filepath(
    &out_data->circle_shader,
    PATH_FROM_JOTUNN_ROOT("/res/shaders/circle_2d_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/circle_2d_shader.frag"));
#ifdef DEBUG
  if (error)
    log_write(
      LOG_LEVEL_ERROR,
      "Error during circle shader init");
#endif
  error = shader_program_init_filepath(
    &out_data->line_shader,
    PATH_FROM_JOTUNN_ROOT("/res/shaders/line_2d_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/line_2d_shader.frag"));
#ifdef DEBUG
  if (error)
    log_write(
      LOG_LEVEL_ERROR,
      "Error during line shader init");
#endif
  error = shader_program_init_filepath(
    &(out_data->mesh_shader),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/mesh_2d_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/mesh_2d_shader.frag"));
#ifdef DEBUG
  if (error)
  {
    log_write(
      LOG_LEVEL_ERROR,
      "Error during mesh shader init");
  }
#endif

  fmatrix_4x4_init(&out_data->model_matrix);

  renderable_2d_triangle_data_init(
    &out_data->triangle_data, max_vertices, &out_data->triangle_shader);
  renderable_2d_quad_data_init(
    &out_data->quad_data, max_vertices, &out_data->quad_shader);
  renderable_2d_textured_quad_data_init(
    &out_data->textured_quad_data, max_vertices, &out_data->textured_quad_shader);
  renderable_2d_circle_data_init(
    &out_data->circle_data, max_vertices, &out_data->circle_shader);
  renderable_2d_line_data_init(
    &out_data->line_data, max_vertices, &out_data->line_shader);
  renderable_2d_mesh_data_init(
    &(out_data->mesh_data),
    &(out_data->mesh_shader));

  static unsigned char white_texture_pixels[4] = {0xFF, 0xFF, 0xFF, 0xFF};
  texture_2d_init(
    &(out_data->white_texture),
    1,
    1,
    TEXTURE_2D_INTERNAL_FORMAT_RGBA,
    TEXTURE_2D_DATA_FORMAT_RGBA,
    false);
  texture_2d_set_data(
    &(out_data->white_texture),
    white_texture_pixels,
    sizeof(white_texture_pixels));
  resource_set_name(
    &(out_data->white_texture.resource),
    "renderer_2d white");

  out_data->line_width_pixels = 5.0f;

  out_data->textures_index = 0;
}

/*******************************************************************************
 *
 *   Function: renderer_2d_data_cleanup()
 *
 ******************************************************************************/
static void renderer_2d_data_cleanup(
  struct renderer_2d_data_t *inout_data)
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Renderer 2D data cleanup\n");
  // #endif

  renderable_2d_triangle_data_cleanup(&inout_data->triangle_data);
  renderable_2d_quad_data_cleanup(&inout_data->quad_data);
  renderable_2d_textured_quad_data_cleanup(&inout_data->textured_quad_data);
  renderable_2d_circle_data_cleanup(&inout_data->circle_data);
  renderable_2d_line_data_cleanup(&inout_data->line_data);
  renderable_2d_mesh_data_cleanup(&(inout_data->mesh_data));
  texture_2d_cleanup(&(inout_data->white_texture));

  shader_program_destroy(&inout_data->triangle_shader);
  shader_program_destroy(&inout_data->quad_shader);
  shader_program_destroy(&inout_data->textured_quad_shader);
  shader_program_destroy(&inout_data->circle_shader);
  shader_program_destroy(&inout_data->line_shader);
  shader_program_destroy(&(inout_data->mesh_shader));

  inout_data->line_width_pixels = -1.0f;
}

/*******************************************************************************
 *
 *   Function: renderer_2d_init()
 *
 ******************************************************************************/
void renderer_2d_init(
  struct renderer_2d_t *out_renderer,
  struct camera_base_t *in_camera,
  struct window_t *in_parent_window,
  const char *in_tag)
{
  const uint16_t render_flags = (RENDER_API_RENDER_FLAGS_COLOR | RENDER_API_RENDER_FLAGS_DEPTH | RENDER_API_RENDER_FLAGS_BLEND);

  renderer_base_init(&out_renderer->base, render_flags, in_camera, in_parent_window, in_tag);

  out_renderer->base.renderer_begin_scene = &renderer_2d_begin_scene;
  out_renderer->base.renderer_end_scene = &renderer_2d_end_scene;

#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Initializing renderer_2d");
#endif

  // Batch stuff
  renderer_2d_data_init(&out_renderer->render_data);
  renderer_2d_set_line_width(out_renderer, out_renderer->render_data.line_width_pixels);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_cleanup()
 *
 ******************************************************************************/
void renderer_2d_cleanup(
  struct renderer_2d_t *inout_renderer)
{
  // #ifdef DEBUG
  //      fprintf(stdout, "Cleaning up renderer 2D\n");
  // #endif

  vertex_array_unbind();

  renderer_2d_data_cleanup(&inout_renderer->render_data);

  renderer_base_cleanup(&(inout_renderer->base));
}

/*******************************************************************************
 *
 *   Function: renderer_2d_begin_scene()
 *
 ******************************************************************************/
void renderer_2d_begin_scene(
  void *inout_renderer)
{
  struct renderer_2d_t *renderer_2d = (struct renderer_2d_t *)inout_renderer;
  // #ifdef DEBUG
  //    fprintf(stdout, "Renderer 2D beginning scene\n");
  // #endif

  renderer_2d_start_batch(renderer_2d);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_end_scene()
 *
 ******************************************************************************/
void renderer_2d_end_scene(
  void *in_renderer)
{
  struct renderer_2d_t *renderer_2d = (struct renderer_2d_t *)in_renderer;
  // #ifdef DEBUG
  //    fprintf(stdout, "Renderer 2D ending scene\n");
  // #endif

  renderer_2d_flush(renderer_2d);

  vertex_array_unbind();

  framebuffer_unbind();
}

/*******************************************************************************
 *
 *   Function: renderer_2d_draw_triangle()
 *
 ******************************************************************************/
void renderer_2d_draw_triangle(
  struct renderer_2d_t *inout_renderer,
  const union fmatrix_4x4_t *in_transform,
  const union fvector4_t in_color)
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Renderer 2D drawing triangle\n");
  // #endif

  struct renderer_2d_data_t *data = &inout_renderer->render_data;

  const unsigned int triangle_vertex_count = 3;

  if (data->triangle_data.triangle_index_count >= max_indices)
    renderer_2d_next_batch(inout_renderer);

  unsigned int i;
  for (i = 0; i < triangle_vertex_count; i++)
  {
    data->triangle_data.vertex_data_ptr->position =
      fmatrix_4x4_transform_point(in_transform, triangle_2d_position_data[i]);
    data->triangle_data.vertex_data_ptr->color = in_color;

    data->triangle_data.vertex_data_ptr++;
  }

  data->triangle_data.triangle_index_count += 3;
}

/*******************************************************************************
 *
 *   Function: renderer_2d_draw_quad()
 *
 ******************************************************************************/
void renderer_2d_draw_quad(
  struct renderer_2d_t *inout_renderer,
  const union fmatrix_4x4_t *in_transform,
  const union fvector4_t in_color)
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Renderer 2D drawing quad\n");
  // #endif

  struct renderer_2d_data_t *data = &inout_renderer->render_data;

  const unsigned int quad_vertex_count = 4;

  if (data->quad_data.quad_index_count >= max_indices)
    renderer_2d_next_batch(inout_renderer);

  unsigned int i;
  for (i = 0; i < quad_vertex_count; i++)
  {
    data->quad_data.vertex_data_ptr->position =
      fmatrix_4x4_transform_point(in_transform, quad_2d_position_data[i]);
    data->quad_data.vertex_data_ptr->color = in_color;

    data->quad_data.vertex_data_ptr++;
  }

  data->quad_data.quad_index_count += 6;
}

/*******************************************************************************
 *
 *   Function: renderer_2d_draw_unfilled_quad()
 *
 ******************************************************************************/
void renderer_2d_draw_unfilled_quad(
  struct renderer_2d_t *inout_renderer,
  const union fmatrix_4x4_t *in_transform,
  const union fvector4_t in_color)
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Renderer 2D drawing unfilled quad\n");
  // #endif

  renderer_2d_draw_line(
    inout_renderer,
    fmatrix_4x4_transform_point(in_transform, quad_2d_position_data[0]),
    fmatrix_4x4_transform_point(in_transform, quad_2d_position_data[1]),
    in_color);

  renderer_2d_draw_line(
    inout_renderer,
    fmatrix_4x4_transform_point(in_transform, quad_2d_position_data[1]),
    fmatrix_4x4_transform_point(in_transform, quad_2d_position_data[2]),
    in_color);

  renderer_2d_draw_line(
    inout_renderer,
    fmatrix_4x4_transform_point(in_transform, quad_2d_position_data[2]),
    fmatrix_4x4_transform_point(in_transform, quad_2d_position_data[3]),
    in_color);

  renderer_2d_draw_line(
    inout_renderer,
    fmatrix_4x4_transform_point(in_transform, quad_2d_position_data[3]),
    fmatrix_4x4_transform_point(in_transform, quad_2d_position_data[0]),
    in_color);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_draw_textured_quad()
 *
 ******************************************************************************/
void renderer_2d_draw_textured_quad(
  struct renderer_2d_t *inout_renderer,
  const union fmatrix_4x4_t *in_transform,
  const struct texture_2d_t *in_texture)
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Renderer 2D drawing textured quad\n");
  // #endif

  struct renderer_2d_data_t *data = &inout_renderer->render_data;

  const unsigned int quad_vertex_count = 4;

  const float tiling_factor = 1.0f;

  if (data->quad_data.quad_index_count >= max_indices)
    renderer_2d_next_batch(inout_renderer);

  float texture_index = -1.0f;
  for (unsigned int i = 0; i < data->textures_index; i++)
  {
    const unsigned int this_texture_id = (data->textures[i])->texture_id;

    if ((this_texture_id) == (in_texture->texture_id))
    {
      texture_index = (float)i;
      break;
    }
  }

  if (texture_index < 0)
  {
    if (data->textures_index >= _RENDERER_2D_MAX_TEXTURES)
      renderer_2d_next_batch(inout_renderer);

    texture_index = (float)data->textures_index;
    data->textures[data->textures_index] = in_texture;
    data->textures_index++;
  }

  unsigned int i;
  for (i = 0; i < quad_vertex_count; i++)
  {
    const union fvector3_t new_position =
      fmatrix_4x4_transform_point(in_transform, textured_quad_2d_position_data[i]);
    const union fvector2_t new_texture_coordinates =
      textured_quad_texture_coordinates[i];
    const float new_texture_index = texture_index;
    const float new_tiling_factor = tiling_factor;

    data->textured_quad_data.vertex_data_ptr->position = new_position;
    data->textured_quad_data.vertex_data_ptr->texture_coordinate =
      new_texture_coordinates;
    data->textured_quad_data.vertex_data_ptr->texture_index = new_texture_index;
    data->textured_quad_data.vertex_data_ptr->tiling_factor = new_tiling_factor;

    data->textured_quad_data.vertex_data_ptr++;
  }

  data->textured_quad_data.textured_quad_index_count += 6;
}

/*******************************************************************************
 *
 *   Function: renderer_2d_draw_subtextured_quad()
 *
 ******************************************************************************/
void renderer_2d_draw_subtextured_quad(
  struct renderer_2d_t *inout_renderer,
  const union fmatrix_4x4_t *in_transform,
  const struct texture_2d_t *in_texture,
  const union fvector2_t in_subtexture_coords[4])
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Renderer 2D drawing subtextured quad\n");
  // #endif

  struct renderer_2d_data_t *data = &inout_renderer->render_data;

  const unsigned int quad_vertex_count = 4;

  const float tiling_factor = 1.0f;

  if (data->quad_data.quad_index_count >= max_indices)
    renderer_2d_next_batch(inout_renderer);

  float texture_index = -1.0f;
  for (unsigned int i = 0; i < data->textures_index; i++)
  {
    const unsigned int this_texture_id = (data->textures[i])->texture_id;

    if ((this_texture_id) == (in_texture->texture_id))
    {
      texture_index = (float)i;
      break;
    }
  }

  if (texture_index < 0)
  {
    if (data->textures_index >= _RENDERER_2D_MAX_TEXTURES)
      renderer_2d_next_batch(inout_renderer);

    texture_index = (float)data->textures_index;
    data->textures[data->textures_index] = in_texture;
    data->textures_index++;
  }

  unsigned int i;
  for (i = 0; i < quad_vertex_count; i++)
  {
    const union fvector3_t new_position =
      fmatrix_4x4_transform_point(in_transform, textured_quad_2d_position_data[i]);
    const union fvector2_t new_texture_coordinates = in_subtexture_coords[i];
    const float new_texture_index = texture_index;
    const float new_tiling_factor = tiling_factor;

    data->textured_quad_data.vertex_data_ptr->position = new_position;
    data->textured_quad_data.vertex_data_ptr->texture_coordinate =
      new_texture_coordinates;
    data->textured_quad_data.vertex_data_ptr->texture_index = new_texture_index;
    data->textured_quad_data.vertex_data_ptr->tiling_factor = new_tiling_factor;

    data->textured_quad_data.vertex_data_ptr++;
  }

  data->textured_quad_data.textured_quad_index_count += 6;
}

/*******************************************************************************
 *
 *   Function: renderer_2d_draw_circle()
 *
 ******************************************************************************/
void renderer_2d_draw_circle(
  struct renderer_2d_t *inout_renderer,
  const union fmatrix_4x4_t *in_transform,
  const union fvector4_t in_color)
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Renderer 2D drawing circle\n");
  // #endif

  struct renderer_2d_data_t *data = &inout_renderer->render_data;

  if (data->circle_data.circle_index_count >= max_indices)
    renderer_2d_next_batch(inout_renderer);

  unsigned int i;
  for (i = 0; i < _NUM_CIRCLE_2D_VERTICES; i++)
  {
    data->circle_data.vertex_data_ptr->position =
      fmatrix_4x4_transform_point(in_transform, circle_2d_position_data[i]);
    data->circle_data.vertex_data_ptr->color = in_color;

    data->circle_data.vertex_data_ptr++;
  }

  data->circle_data.circle_index_count +=
    (3 * (_RENDERABLE_2D_CIRCLE_RESOLUTION));
}

/*******************************************************************************
 *
 *   Function: renderer_2d_draw_line()
 *
 ******************************************************************************/
void renderer_2d_draw_line(
  struct renderer_2d_t *inout_renderer,
  const union fvector3_t in_pos_1,
  const union fvector3_t in_pos_2,
  const union fvector4_t in_color)
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Renderer 2D drawing line\n");
  // #endif

  struct renderer_2d_data_t *data = &inout_renderer->render_data;

  data->line_data.vertex_data_ptr->position = in_pos_1;
  data->line_data.vertex_data_ptr->color = in_color;

  data->line_data.vertex_data_ptr++;

  data->line_data.vertex_data_ptr->position = in_pos_2;
  data->line_data.vertex_data_ptr->color = in_color;

  data->line_data.vertex_data_ptr++;

  data->line_data.line_vertex_count += 2;
}

/*******************************************************************************
 *
 *   Function: renderer_2d_draw_mesh()
 *
 ******************************************************************************/
void renderer_2d_draw_mesh(
  struct renderer_2d_t* inout_renderer,
  const struct renderer_2d_mesh_t* in_mesh)
{
  struct renderer_2d_data_t* data = &(inout_renderer->render_data);

  if ((in_mesh->vertex_count == 0) ||
      (in_mesh->index_count == 0))
  {
    return;
  }

  // Everything submitted before the mesh has to land underneath it
  renderer_2d_next_batch(inout_renderer);

  const uint32_t index_size_bytes =
    _renderer_2d_get_index_size_bytes(in_mesh->index_type);

  vertex_array_bind(&(data->mesh_data.vao));
  vertex_buffer_bind(&(data->mesh_data.vbo));
  element_buffer_bind(&(data->mesh_data.ebo));

  vertex_buffer_set_data(
    &(data->mesh_data.vbo),
    in_mesh->vertices,
    (in_mesh->vertex_count * sizeof(struct renderable_2d_mesh_vertex_t)),
    VERTEX_BUFFER_DATA_USAGE_STREAM_DRAW);
  element_buffer_buffer_data(
    &(data->mesh_data.ebo),
    in_mesh->indices,
    (in_mesh->index_count * index_size_bytes),
    VERTEX_BUFFER_DATA_USAGE_STREAM_DRAW);

  shader_program_use(&(data->mesh_shader));

  shader_program_set_uniform_fmat4x4(
    &(data->mesh_shader),
    "model",
    &(data->model_matrix));
  shader_program_set_uniform_fmat4x4(
    &(data->mesh_shader),
    "view",
    &(inout_renderer->base.camera->view_matrix));
  shader_program_set_uniform_fmat4x4(
    &(data->mesh_shader),
    "projection",
    &(inout_renderer->base.camera->projection_matrix));
  shader_program_set_uniform_int(
    &(data->mesh_shader),
    "u_texture",
    0);

  for (uint32_t i = 0; i < in_mesh->submesh_count; i++)
  {
    const struct renderer_2d_submesh_t* submesh = &(in_mesh->submeshes[i]);
    const struct renderer_2d_clip_rect_t* clip_rect = &(submesh->clip_rect);

    if (submesh->index_count == 0)
    {
      continue;
    }

    if (submesh->is_clipped)
    {
      if ((clip_rect->width_pixels <= 0) ||
          (clip_rect->height_pixels <= 0))
      {
        continue;
      }

      render_api_set_scissor(
        &(inout_renderer->base.render_api),
        clip_rect->x_pixels,
        clip_rect->y_pixels,
        clip_rect->width_pixels,
        clip_rect->height_pixels);
    }
    else
    {
      render_api_disable_scissor(&(inout_renderer->base.render_api));
    }

    texture_2d_bind(
      (submesh->texture != NULL) ?
        submesh->texture :
        &(data->white_texture),
      0);

    render_api_draw_elements_base_vertex(
      RENDER_API_DRAW_MODE_TRIANGLES,
      submesh->index_count,
      in_mesh->index_type,
      ((uintptr_t)submesh->index_offset * index_size_bytes),
      (int32_t)submesh->vertex_offset);
  }

  render_api_disable_scissor(&(inout_renderer->base.render_api));

  vertex_array_unbind();
}

/*******************************************************************************
 *
 *   Function: renderer_2d_draw_string()
 *
 ******************************************************************************/
void renderer_2d_draw_string(
  struct renderer_2d_t *inout_renderer,
  const struct font_typeface_t *in_typeface,
  const union fvector3_t in_start_position,
  const char *in_draw_string)
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Renderer 2D drawing string\n");
  // #endif

  unsigned int num_chars = strlen(in_draw_string);
  union fvector2_t text_dimensions =
    font_typeface_calculate_string_dimensions(in_typeface, in_draw_string, num_chars);

  int error;

  union fmatrix_4x4_t scale_matrix, translation_matrix, transform;
  fmatrix_4x4_init(&scale_matrix);
  fmatrix_4x4_init(&translation_matrix);

  union fvector3_t string_current_render_position = {in_start_position.comp.x,
                                             in_start_position.comp.y -
                                               text_dimensions.comp.y,
                                             0.0f};

  union fvector3_t texture_dimensions_scale_factors, character_x_translation,
    character_local_translation;

  const struct font_glyph_t *character_glyph;

  unsigned int i;
  for (i = 0; i < num_chars; i++)
  {
    char c = in_draw_string[i];

    character_glyph = typeface_get_glyph_from_char(in_typeface, c);

    fvector3_set(
      &texture_dimensions_scale_factors,
      (float)(character_glyph->width_pixels),
      (float)(character_glyph->height_pixels),
      1.0f);

    fvector3_set(
      &character_local_translation,
      (float)character_glyph->offset_x_pixels,
      -1.0f * (float)character_glyph->offset_y_pixels,
      0.0f);

    translation_matrix = fmatrix_4x4_transform_translate(
      &translation_matrix, character_local_translation);
    translation_matrix = fmatrix_4x4_transform_translate(
      &translation_matrix, string_current_render_position);

    scale_matrix = fmatrix_4x4_transform_scale(
      &scale_matrix, texture_dimensions_scale_factors);

    transform = fmatrix_4x4_multiply(&scale_matrix, &translation_matrix);

    const union fvector2_t *p0, *p1;
    p0 = &(character_glyph->glyph_texture.subtexture_coordinates[0]);
    p1 = &(character_glyph->glyph_texture.subtexture_coordinates[1]);

    const union fvector2_t subtexture_coords[4] = {{p0->comp.x, p1->comp.y},
                                           {p1->comp.x, p1->comp.y},
                                           {p1->comp.x, p0->comp.y},
                                           {p0->comp.x, p0->comp.y}};

    renderer_2d_draw_subtextured_quad(
      inout_renderer,
      &transform,
      character_glyph->glyph_texture.parent_texture,
      subtexture_coords);

    fvector3_set(
      &character_x_translation,
      (float)(character_glyph->advance_x_pixels),
      0.0f,
      0.0f);
    string_current_render_position =
      fvector3_add(&string_current_render_position, &character_x_translation);

    fmatrix_4x4_identity(&scale_matrix);
    fmatrix_4x4_identity(&translation_matrix);
  }
}

/*******************************************************************************
 *
 *   Function: renderer_2d_set_line_width()
 *
 ******************************************************************************/
void renderer_2d_set_line_width(
  const struct renderer_2d_t *in_renderer,
  const float in_line_width_pixels)
{
  render_api_set_line_width(in_line_width_pixels);
}

/*******************************************************************************
 *
 *   Function: _renderer_2d_get_index_size_bytes()
 *
 ******************************************************************************/
static uint32_t _renderer_2d_get_index_size_bytes(
  const enum render_api_element_data_type_t in_index_type)
{
  uint32_t index_size_bytes;

  switch (in_index_type)
  {
    case RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_BYTE:
    {
      index_size_bytes = sizeof(uint8_t);
    }
    break;
    case RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_SHORT:
    {
      index_size_bytes = sizeof(uint16_t);
    }
    break;
    case RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_INT:
    default:
    {
      index_size_bytes = sizeof(uint32_t);
    }
    break;
  }

  return index_size_bytes;
}
