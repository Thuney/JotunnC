#include "renderer_3d.h"
#include "allocator.h"
#include "log.h"

#include "framebuffer.h"
#include "util.h"
#include "window.h"

#include <stdbool.h>
#include <stdio.h>

// The colors renderer_3d_draw_grid() and renderer_3d_draw_axes() draw the
//  axes in
static const union fvector4_t renderer_3d_x_axis_color = {{0.9f, 0.25f, 0.25f, 1.0f}};
static const union fvector4_t renderer_3d_y_axis_color = {{0.25f, 0.9f, 0.25f, 1.0f}};
static const union fvector4_t renderer_3d_z_axis_color = {{0.25f, 0.45f, 0.95f, 1.0f}};

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Draws everything in the batch, a draw call per batch with anything in it,
//  in the order cubes, pyramids, spheres, textured cubes, lines. The batch is
//  left as it was
//
//  in:     in_renderer
static void renderer_3d_flush(
  struct renderer_3d_t* in_renderer);

// Draws one batch of lit meshes, if it holds any, with the shaded shader
//
//  in:     in_renderer, in_shaded_data
static void renderer_3d_flush_shaded(
  struct renderer_3d_t* in_renderer,
  const struct renderable_3d_shaded_data_t* in_shaded_data);

// Empties the batch
//
//  in/out: inout_renderer
static void renderer_3d_start_batch(
  struct renderer_3d_t* inout_renderer);

// Draws everything in the batch, then empties it
//
//  in/out: inout_renderer
static void renderer_3d_next_batch(
  struct renderer_3d_t* inout_renderer);

// Loads the shaders and creates the batches, keeping their vertices in
//  inout_storage, and sets the default light. Shaders that fail to load are
//  logged in DEBUG builds
//
//  in/out: inout_storage
//  out:    out_data
static void renderer_3d_data_init(
  struct renderer_3d_data_t* out_data,
  struct renderer_3d_storage_t* inout_storage);

// Releases everything renderer_3d_data_init() made
//
//  in/out: inout_data
static void renderer_3d_data_cleanup(
  struct renderer_3d_data_t* inout_data);

// Adds a copy of a batch's mesh, moved by in_transform and colored in_color,
//  drawing the whole batch first when it has no room for another copy
//
//  in:     in_transform, in_color
//  in/out: inout_renderer, inout_shaded_data
static void renderer_3d_push_shaded_mesh(
  struct renderer_3d_t* inout_renderer,
  struct renderable_3d_shaded_data_t* inout_shaded_data,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color);

// Finds the slot in the batch's textures that in_texture is sampled from,
//  adding it when it isn't there yet. When there's no room left for it, the
//  batch is drawn first, and it takes the first slot of the next
//
//  in:     in_texture
//  in/out: inout_renderer
//  ret:    The texture's slot, as the float the textured vertices carry
static float renderer_3d_find_texture_index(
  struct renderer_3d_t* inout_renderer,
  const struct texture_2d_t* in_texture);

// Finds the matrix that carries normals through a transform: the inverse
//  transpose of the transform's upper 3x3, which is its cofactor matrix over
//  its determinant. The shaders normalize their normals, so only the
//  determinant's sign is applied
//
//  in:     in_transform
//  ret:    The normal matrix, laid out for row vectors like in_transform
static union fmatrix_3x3_t renderer_3d_calculate_normal_matrix(
  const union fmatrix_4x4_t* in_transform);

// Carries a normal through a normal matrix
//
//  in:     in_normal_matrix, in_normal
//  ret:    The transformed normal, not normalized
static union fvector3_t renderer_3d_transform_normal(
  const union fmatrix_3x3_t* in_normal_matrix,
  union fvector3_t in_normal);

// Sets a shader's model, view and projection uniforms from the renderer and
//  its camera. The shader must be the one in use
//
//  in:     in_renderer, in_shader
static void renderer_3d_set_camera_uniforms(
  const struct renderer_3d_t* in_renderer,
  const struct shader_program_t* in_shader);

// Sets a shader's light uniforms from the renderer's light. The shader must
//  be the one in use
//
//  in:     in_renderer, in_shader
static void renderer_3d_set_light_uniforms(
  const struct renderer_3d_t* in_renderer,
  const struct shader_program_t* in_shader);

/*******************************************************************************
 *
 *   Function: renderer_3d_init()
 *
 ******************************************************************************/
uint8_t renderer_3d_init(
  struct renderer_3d_t* out_renderer,
  struct renderer_3d_storage_t* inout_storage,
  struct camera_base_t* in_camera,
  struct window_t* in_parent_window,
  const char* in_tag)
{
  // Allocated before anything else is made, so that failing leaves nothing
  //  to clean up
  struct renderer_3d_storage_t* storage = inout_storage;
  const bool is_storage_owned = (inout_storage == NULL);

  if (is_storage_owned) {
    storage =
      (struct renderer_3d_storage_t*)allocator_allocate(
        sizeof(struct renderer_3d_storage_t),
        ALLOCATOR_TAG_RENDERER);

    if (storage == NULL) {
    #ifdef DEBUG
      log_write(
        LOG_LEVEL_ERROR,
        "Couldn't allocate %zu bytes of renderer_3d storage",
        sizeof(struct renderer_3d_storage_t));
    #endif

      return 1;
    }
  }

  const uint16_t render_flags =
    (RENDER_API_RENDER_FLAGS_COLOR | RENDER_API_RENDER_FLAGS_DEPTH | RENDER_API_RENDER_FLAGS_BLEND);

  renderer_base_init(
    &(out_renderer->base),
    render_flags,
    in_camera,
    in_parent_window,
    in_tag);

  out_renderer->base.renderer_begin_scene = &(renderer_3d_begin_scene);
  out_renderer->base.renderer_end_scene = &(renderer_3d_end_scene);

#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Initializing renderer_3d");
#endif

  renderer_3d_data_init(
    &(out_renderer->render_data),
    storage);
  out_renderer->render_data.is_storage_owned = is_storage_owned;

  return 0;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_cleanup()
 *
 ******************************************************************************/
void renderer_3d_cleanup(
  struct renderer_3d_t* inout_renderer)
{
  vertex_array_unbind();

  renderer_3d_data_cleanup(&(inout_renderer->render_data));

  if (inout_renderer->render_data.is_storage_owned) {
    allocator_free(inout_renderer->render_data.storage);
  }

  inout_renderer->render_data.storage = NULL;
  inout_renderer->render_data.is_storage_owned = false;

  renderer_base_cleanup(&(inout_renderer->base));
}

/*******************************************************************************
 *
 *   Function: renderer_3d_begin_scene()
 *
 ******************************************************************************/
void renderer_3d_begin_scene(
  struct renderer_base_t* inout_renderer)
{
  struct renderer_3d_t* renderer_3d =
    container_of(
      inout_renderer,
      struct renderer_3d_t,
      base);

  renderer_3d_start_batch(renderer_3d);
}

/*******************************************************************************
 *
 *   Function: renderer_3d_end_scene()
 *
 ******************************************************************************/
void renderer_3d_end_scene(
  struct renderer_base_t* in_renderer)
{
  struct renderer_3d_t* renderer_3d =
    container_of(
      in_renderer,
      struct renderer_3d_t,
      base);

  renderer_3d_flush(renderer_3d);

  vertex_array_unbind();

  framebuffer_unbind();
}

/*******************************************************************************
 *
 *   Function: renderer_3d_draw_cube()
 *
 ******************************************************************************/
void renderer_3d_draw_cube(
  struct renderer_3d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color)
{
  renderer_3d_push_shaded_mesh(
    inout_renderer,
    &(inout_renderer->render_data.cube_data),
    in_transform,
    in_color);
}

/*******************************************************************************
 *
 *   Function: renderer_3d_draw_unfilled_cube()
 *
 ******************************************************************************/
void renderer_3d_draw_unfilled_cube(
  struct renderer_3d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color)
{
  for (unsigned int i = 0; i < _NUM_CUBE_3D_EDGE_VERTICES; i += 2) {
    renderer_3d_draw_line(
      inout_renderer,
      fmatrix_4x4_transform_point(
        in_transform,
        cube_3d_edge_position_data[i]),
      fmatrix_4x4_transform_point(
        in_transform,
        cube_3d_edge_position_data[i + 1]),
      in_color);
  }
}

/*******************************************************************************
 *
 *   Function: renderer_3d_draw_textured_cube()
 *
 ******************************************************************************/
void renderer_3d_draw_textured_cube(
  struct renderer_3d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  const struct texture_2d_t* in_texture)
{
  struct renderable_3d_textured_data_t* textured_data =
    &(inout_renderer->render_data.textured_cube_data);
  const struct renderable_3d_mesh_t* mesh = textured_data->mesh;

  const float tiling_factor = 1.0f;

  if ((textured_data->index_count + mesh->index_count) > textured_data->max_index_count) {
    renderer_3d_next_batch(inout_renderer);
  }

  // After making room for the cube, as starting a new batch empties the
  //  batch's textures
  const float texture_index =
    renderer_3d_find_texture_index(
      inout_renderer,
      in_texture);

  const union fmatrix_3x3_t normal_matrix = renderer_3d_calculate_normal_matrix(in_transform);

  for (unsigned int v = 0; v < mesh->vertex_count; v++) {
    struct renderable_3d_textured_vertex_t* vertex = textured_data->vertex_data_ptr;

    vertex->position =
      fmatrix_4x4_transform_point(
        in_transform,
        mesh->positions[v]);
    vertex->normal =
      renderer_3d_transform_normal(
        &(normal_matrix),
        mesh->normals[v]);
    vertex->texture_coordinate = mesh->texture_coordinates[v];
    vertex->texture_index = texture_index;
    vertex->tiling_factor = tiling_factor;

    textured_data->vertex_data_ptr++;
  }

  textured_data->index_count += mesh->index_count;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_draw_pyramid()
 *
 ******************************************************************************/
void renderer_3d_draw_pyramid(
  struct renderer_3d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color)
{
  renderer_3d_push_shaded_mesh(
    inout_renderer,
    &(inout_renderer->render_data.pyramid_data),
    in_transform,
    in_color);
}

/*******************************************************************************
 *
 *   Function: renderer_3d_draw_sphere()
 *
 ******************************************************************************/
void renderer_3d_draw_sphere(
  struct renderer_3d_t* inout_renderer,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color)
{
  renderer_3d_push_shaded_mesh(
    inout_renderer,
    &(inout_renderer->render_data.sphere_data),
    in_transform,
    in_color);
}

/*******************************************************************************
 *
 *   Function: renderer_3d_draw_line()
 *
 ******************************************************************************/
void renderer_3d_draw_line(
  struct renderer_3d_t* inout_renderer,
  union fvector3_t in_pos_1,
  union fvector3_t in_pos_2,
  union fvector4_t in_color)
{
  struct renderable_3d_line_data_t* line_data = &(inout_renderer->render_data.line_data);

  if ((line_data->line_vertex_count + 2) > line_data->max_line_vertex_count) {
    renderer_3d_next_batch(inout_renderer);
  }

  line_data->vertex_data_ptr->position = in_pos_1;
  line_data->vertex_data_ptr->color = in_color;

  line_data->vertex_data_ptr++;

  line_data->vertex_data_ptr->position = in_pos_2;
  line_data->vertex_data_ptr->color = in_color;

  line_data->vertex_data_ptr++;

  line_data->line_vertex_count += 2;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_draw_grid()
 *
 ******************************************************************************/
void renderer_3d_draw_grid(
  struct renderer_3d_t* inout_renderer,
  uint32_t in_half_extent_units,
  union fvector4_t in_color)
{
  const int half_extent = (int)in_half_extent_units;
  const float extent = (float)in_half_extent_units;

  // A line across each way at every unit, colored as an axis through the
  //  origin
  for (int i = -half_extent; i <= half_extent; i++) {
    const float offset = (float)i;
    const bool is_axis = (i == 0);

    renderer_3d_draw_line(
      inout_renderer,
      (union fvector3_t){{-extent, 0.0f, offset}},
      (union fvector3_t){{extent, 0.0f, offset}},
      is_axis ? renderer_3d_x_axis_color : in_color);
    renderer_3d_draw_line(
      inout_renderer,
      (union fvector3_t){{offset, 0.0f, -extent}},
      (union fvector3_t){{offset, 0.0f, extent}},
      is_axis ? renderer_3d_z_axis_color : in_color);
  }
}

/*******************************************************************************
 *
 *   Function: renderer_3d_draw_axes()
 *
 ******************************************************************************/
void renderer_3d_draw_axes(
  struct renderer_3d_t* inout_renderer,
  float in_length_units)
{
  const union fvector3_t origin = {{0.0f, 0.0f, 0.0f}};

  renderer_3d_draw_line(
    inout_renderer,
    origin,
    (union fvector3_t){{in_length_units, 0.0f, 0.0f}},
    renderer_3d_x_axis_color);
  renderer_3d_draw_line(
    inout_renderer,
    origin,
    (union fvector3_t){{0.0f, in_length_units, 0.0f}},
    renderer_3d_y_axis_color);
  renderer_3d_draw_line(
    inout_renderer,
    origin,
    (union fvector3_t){{0.0f, 0.0f, in_length_units}},
    renderer_3d_z_axis_color);
}

/*******************************************************************************
 *
 *   Function: renderer_3d_set_line_width()
 *
 ******************************************************************************/
void renderer_3d_set_line_width(
  struct renderer_3d_t* inout_renderer,
  float in_line_width_pixels)
{
  inout_renderer->render_data.line_width_pixels = in_line_width_pixels;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_set_light()
 *
 ******************************************************************************/
void renderer_3d_set_light(
  struct renderer_3d_t* inout_renderer,
  union fvector3_t in_direction,
  union fvector3_t in_color,
  float in_ambient_strength)
{
  struct renderer_3d_light_t* light = &(inout_renderer->render_data.light);

  light->direction = in_direction;
  light->color = in_color;
  light->ambient_strength = in_ambient_strength;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_flush()
 *
 ******************************************************************************/
static void renderer_3d_flush(
  struct renderer_3d_t* in_renderer)
{
  struct renderer_3d_data_t* data = &(in_renderer->render_data);

  renderer_3d_flush_shaded(
    in_renderer,
    &(data->cube_data));
  renderer_3d_flush_shaded(
    in_renderer,
    &(data->pyramid_data));
  renderer_3d_flush_shaded(
    in_renderer,
    &(data->sphere_data));

  if (data->textured_cube_data.index_count) {
    const unsigned int textured_data_size =
      (unsigned int)((uint8_t*)data->textured_cube_data.vertex_data_ptr -
                     (uint8_t*)data->textured_cube_data.vertex_data_base);

    vertex_array_bind(&(data->textured_cube_data.vao));
    vertex_buffer_bind(&(data->textured_cube_data.vbo));
    element_buffer_bind(&(data->textured_cube_data.ebo));

    vertex_buffer_set_sub_data(
      &(data->textured_cube_data.vbo),
      (float*)data->textured_cube_data.vertex_data_base,
      textured_data_size);

    shader_program_use(&(data->textured_shader));

    for (unsigned int i = 0; i < data->textures_index; i++) {
      texture_2d_bind(
        data->textures[i],
        i);
    }

    renderer_3d_set_camera_uniforms(
      in_renderer,
      &(data->textured_shader));
    renderer_3d_set_light_uniforms(
      in_renderer,
      &(data->textured_shader));

    char texture_uniform_name_buf[16];

    for (unsigned int i = 0; i < _RENDERER_3D_MAX_TEXTURES; i++) {
      sprintf(
        texture_uniform_name_buf,
        "u_textures[%u]",
        i);
      shader_program_set_uniform_int(
        &(data->textured_shader),
        texture_uniform_name_buf,
        i);
    }

    render_api_draw_elements(
      RENDER_API_DRAW_MODE_TRIANGLES,
      data->textured_cube_data.index_count,
      RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_INT,
      0);
  }

  if (data->line_data.line_vertex_count) {
    const unsigned int line_data_size =
      (unsigned int)((uint8_t*)data->line_data.vertex_data_ptr -
                     (uint8_t*)data->line_data.vertex_data_base);

    vertex_array_bind(&(data->line_data.vao));
    vertex_buffer_bind(&(data->line_data.vbo));

    vertex_buffer_set_sub_data(
      &(data->line_data.vbo),
      (float*)data->line_data.vertex_data_base,
      line_data_size);

    shader_program_use(&(data->line_shader));

    renderer_3d_set_camera_uniforms(
      in_renderer,
      &(data->line_shader));

    render_api_set_line_width(data->line_width_pixels);
    render_api_draw_lines(data->line_data.line_vertex_count);
  }
}

/*******************************************************************************
 *
 *   Function: renderer_3d_flush_shaded()
 *
 ******************************************************************************/
static void renderer_3d_flush_shaded(
  struct renderer_3d_t* in_renderer,
  const struct renderable_3d_shaded_data_t* in_shaded_data)
{
  if (in_shaded_data->index_count == 0) {
    return;
  }

  const struct shader_program_t* shaded_shader = &(in_renderer->render_data.shaded_shader);

  const unsigned int shaded_data_size =
    (unsigned int)((uint8_t*)in_shaded_data->vertex_data_ptr -
                   (uint8_t*)in_shaded_data->vertex_data_base);

  vertex_array_bind(&(in_shaded_data->vao));
  vertex_buffer_bind(&(in_shaded_data->vbo));
  element_buffer_bind(&(in_shaded_data->ebo));

  vertex_buffer_set_sub_data(
    &(in_shaded_data->vbo),
    (float*)in_shaded_data->vertex_data_base,
    shaded_data_size);

  shader_program_use(shaded_shader);

  renderer_3d_set_camera_uniforms(
    in_renderer,
    shaded_shader);
  renderer_3d_set_light_uniforms(
    in_renderer,
    shaded_shader);

  render_api_draw_elements(
    RENDER_API_DRAW_MODE_TRIANGLES,
    in_shaded_data->index_count,
    RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_INT,
    0);
}

/*******************************************************************************
 *
 *   Function: renderer_3d_start_batch()
 *
 ******************************************************************************/
static void renderer_3d_start_batch(
  struct renderer_3d_t* inout_renderer)
{
  struct renderer_3d_data_t* data = &(inout_renderer->render_data);

  data->cube_data.index_count = 0;
  data->cube_data.vertex_data_ptr = data->cube_data.vertex_data_base;

  data->pyramid_data.index_count = 0;
  data->pyramid_data.vertex_data_ptr = data->pyramid_data.vertex_data_base;

  data->sphere_data.index_count = 0;
  data->sphere_data.vertex_data_ptr = data->sphere_data.vertex_data_base;

  data->textured_cube_data.index_count = 0;
  data->textured_cube_data.vertex_data_ptr = data->textured_cube_data.vertex_data_base;

  data->line_data.line_vertex_count = 0;
  data->line_data.vertex_data_ptr = data->line_data.vertex_data_base;

  data->textures_index = 0;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_next_batch()
 *
 ******************************************************************************/
static void renderer_3d_next_batch(
  struct renderer_3d_t* inout_renderer)
{
  renderer_3d_flush(inout_renderer);
  renderer_3d_start_batch(inout_renderer);
}

/*******************************************************************************
 *
 *   Function: renderer_3d_data_init()
 *
 ******************************************************************************/
static void renderer_3d_data_init(
  struct renderer_3d_data_t* out_data,
  struct renderer_3d_storage_t* inout_storage)
{
  unsigned int error = 0;

  error = shader_program_init_filepath(
    &(out_data->shaded_shader),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/shaded_3d_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/shaded_3d_shader.frag"));
#ifdef DEBUG
  if (error) {
    log_write(
      LOG_LEVEL_ERROR,
      "Error during shaded 3D shader init");
  }
#endif
  error = shader_program_init_filepath(
    &(out_data->textured_shader),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/textured_3d_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/textured_3d_shader.frag"));
#ifdef DEBUG
  if (error) {
    log_write(
      LOG_LEVEL_ERROR,
      "Error during textured 3D shader init");
  }
#endif
  error = shader_program_init_filepath(
    &(out_data->line_shader),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/line_3d_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/line_3d_shader.frag"));
#ifdef DEBUG
  if (error) {
    log_write(
      LOG_LEVEL_ERROR,
      "Error during line 3D shader init");
  }
#endif

  (void)error;

  fmatrix_4x4_init(&(out_data->model_matrix));

  out_data->storage = inout_storage;

  renderable_3d_shaded_data_init(
    &(out_data->cube_data),
    &(cube_3d_mesh),
    RENDERER_3D_MAX_PRIMITIVES_PER_BATCH,
    inout_storage->cube_vertices,
    &(out_data->shaded_shader));
  renderable_3d_shaded_data_init(
    &(out_data->pyramid_data),
    &(pyramid_3d_mesh),
    RENDERER_3D_MAX_PRIMITIVES_PER_BATCH,
    inout_storage->pyramid_vertices,
    &(out_data->shaded_shader));
  renderable_3d_shaded_data_init(
    &(out_data->sphere_data),
    &(sphere_3d_mesh),
    RENDERER_3D_MAX_PRIMITIVES_PER_BATCH,
    inout_storage->sphere_vertices,
    &(out_data->shaded_shader));
  renderable_3d_textured_data_init(
    &(out_data->textured_cube_data),
    &(cube_3d_mesh),
    RENDERER_3D_MAX_PRIMITIVES_PER_BATCH,
    inout_storage->textured_cube_vertices,
    &(out_data->textured_shader));
  renderable_3d_line_data_init(
    &(out_data->line_data),
    RENDERER_3D_MAX_PRIMITIVES_PER_BATCH,
    inout_storage->line_vertices,
    &(out_data->line_shader));

  // White, falling down and away from a camera looking along -Z
  fvector3_set(
    &(out_data->light.direction),
    -0.4f,
    -1.0f,
    -0.6f);
  fvector3_set(
    &(out_data->light.color),
    1.0f,
    1.0f,
    1.0f);
  out_data->light.ambient_strength = 0.25f;

  out_data->line_width_pixels = 1.0f;

  out_data->textures_index = 0;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_data_cleanup()
 *
 ******************************************************************************/
static void renderer_3d_data_cleanup(
  struct renderer_3d_data_t* inout_data)
{
  renderable_3d_shaded_data_cleanup(&(inout_data->cube_data));
  renderable_3d_shaded_data_cleanup(&(inout_data->pyramid_data));
  renderable_3d_shaded_data_cleanup(&(inout_data->sphere_data));
  renderable_3d_textured_data_cleanup(&(inout_data->textured_cube_data));
  renderable_3d_line_data_cleanup(&(inout_data->line_data));

  shader_program_destroy(&(inout_data->shaded_shader));
  shader_program_destroy(&(inout_data->textured_shader));
  shader_program_destroy(&(inout_data->line_shader));

  inout_data->line_width_pixels = -1.0f;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_push_shaded_mesh()
 *
 ******************************************************************************/
static void renderer_3d_push_shaded_mesh(
  struct renderer_3d_t* inout_renderer,
  struct renderable_3d_shaded_data_t* inout_shaded_data,
  const union fmatrix_4x4_t* in_transform,
  union fvector4_t in_color)
{
  const struct renderable_3d_mesh_t* mesh = inout_shaded_data->mesh;

  if ((inout_shaded_data->index_count + mesh->index_count) > inout_shaded_data->max_index_count) {
    renderer_3d_next_batch(inout_renderer);
  }

  const union fmatrix_3x3_t normal_matrix = renderer_3d_calculate_normal_matrix(in_transform);

  for (unsigned int v = 0; v < mesh->vertex_count; v++) {
    struct renderable_3d_shaded_vertex_t* vertex = inout_shaded_data->vertex_data_ptr;

    vertex->position =
      fmatrix_4x4_transform_point(
        in_transform,
        mesh->positions[v]);
    vertex->normal =
      renderer_3d_transform_normal(
        &(normal_matrix),
        mesh->normals[v]);
    vertex->color = in_color;

    inout_shaded_data->vertex_data_ptr++;
  }

  inout_shaded_data->index_count += mesh->index_count;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_find_texture_index()
 *
 ******************************************************************************/
static float renderer_3d_find_texture_index(
  struct renderer_3d_t* inout_renderer,
  const struct texture_2d_t* in_texture)
{
  struct renderer_3d_data_t* data = &(inout_renderer->render_data);

  for (unsigned int i = 0; i < data->textures_index; i++) {
    if (data->textures[i]->texture_id == in_texture->texture_id) {
      return (float)i;
    }
  }

  if (data->textures_index >= _RENDERER_3D_MAX_TEXTURES) {
    renderer_3d_next_batch(inout_renderer);
  }

  const float texture_index = (float)data->textures_index;

  data->textures[data->textures_index] = in_texture;
  data->textures_index++;

  return texture_index;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_calculate_normal_matrix()
 *
 ******************************************************************************/
static union fmatrix_3x3_t renderer_3d_calculate_normal_matrix(
  const union fmatrix_4x4_t* in_transform)
{
  const float a = in_transform->mat[0][0];
  const float b = in_transform->mat[0][1];
  const float c = in_transform->mat[0][2];
  const float d = in_transform->mat[1][0];
  const float e = in_transform->mat[1][1];
  const float f = in_transform->mat[1][2];
  const float g = in_transform->mat[2][0];
  const float h = in_transform->mat[2][1];
  const float i = in_transform->mat[2][2];

  const float cofactors[3][3] = {
    {((e * i) - (f * h)), ((f * g) - (d * i)), ((d * h) - (e * g))},
    {((c * h) - (b * i)), ((a * i) - (c * g)), ((b * g) - (a * h))},
    {((b * f) - (c * e)), ((c * d) - (a * f)), ((a * e) - (b * d))}};

  const float determinant =
    ((a * cofactors[0][0]) + (b * cofactors[0][1]) + (c * cofactors[0][2]));

  // A mirroring transform turns the cofactors' normals inside out
  const float sign = (determinant < 0.0f) ? -1.0f : 1.0f;

  float normal_matrix_data[3][3];

  for (unsigned int row = 0; row < 3; row++) {
    for (unsigned int col = 0; col < 3; col++) {
      normal_matrix_data[row][col] = (sign * cofactors[row][col]);
    }
  }

  union fmatrix_3x3_t normal_matrix;

  fmatrix_3x3_set(
    &(normal_matrix),
    (const float (*)[3])normal_matrix_data);

  return normal_matrix;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_transform_normal()
 *
 ******************************************************************************/
static union fvector3_t renderer_3d_transform_normal(
  const union fmatrix_3x3_t* in_normal_matrix,
  union fvector3_t in_normal)
{
  union fvector3_t transformed_normal = {{0.0f, 0.0f, 0.0f}};

  // The normal as a row vector, as fmatrix_4x4_transform_point() takes points
  for (unsigned int col = 0; col < 3; col++) {
    for (unsigned int row = 0; row < 3; row++) {
      transformed_normal.vec[col] += (in_normal.vec[row] * in_normal_matrix->mat[row][col]);
    }
  }

  return transformed_normal;
}

/*******************************************************************************
 *
 *   Function: renderer_3d_set_camera_uniforms()
 *
 ******************************************************************************/
static void renderer_3d_set_camera_uniforms(
  const struct renderer_3d_t* in_renderer,
  const struct shader_program_t* in_shader)
{
  shader_program_set_uniform_fmat4x4(
    in_shader,
    "model",
    &(in_renderer->render_data.model_matrix));
  shader_program_set_uniform_fmat4x4(
    in_shader,
    "view",
    &(in_renderer->base.camera->view_matrix));
  shader_program_set_uniform_fmat4x4(
    in_shader,
    "projection",
    &(in_renderer->base.camera->projection_matrix));
}

/*******************************************************************************
 *
 *   Function: renderer_3d_set_light_uniforms()
 *
 ******************************************************************************/
static void renderer_3d_set_light_uniforms(
  const struct renderer_3d_t* in_renderer,
  const struct shader_program_t* in_shader)
{
  const struct renderer_3d_light_t* light = &(in_renderer->render_data.light);

  shader_program_set_uniform_fvec3(
    in_shader,
    "u_lightDirection",
    &(light->direction));
  shader_program_set_uniform_fvec3(
    in_shader,
    "u_lightColor",
    &(light->color));
  shader_program_set_uniform_float(
    in_shader,
    "u_ambientStrength",
    light->ambient_strength);
}
