#include "renderer_3d.h"

#include "framebuffer.h"
#include "util.h"
#include "window.h"

#include <stdio.h>

// Upper limits on each primitive type to draw per batch
static const unsigned int max_cubes = 1000;
static const unsigned int max_pyramids = 1000;
static const unsigned int max_spheres = 250;
static const unsigned int max_textured_cubes = 500;
static const unsigned int max_lines = 10000;

/******************************************************************************
 *
 * Function: renderer_3d_calculate_normal_matrix()
 *
 ******************************************************************************/
static fmatrix_3x3 renderer_3d_calculate_normal_matrix(
  const fmatrix_4x4* transform)
{
  // Normals transform by the inverse transpose of the transform's upper 3x3,
  //  which is its cofactor matrix divided by its determinant. Normals are
  //  re-normalized in the shader, so only the sign of the determinant matters
  const float a = transform->mat[0][0];
  const float b = transform->mat[0][1];
  const float c = transform->mat[0][2];
  const float d = transform->mat[1][0];
  const float e = transform->mat[1][1];
  const float f = transform->mat[1][2];
  const float g = transform->mat[2][0];
  const float h = transform->mat[2][1];
  const float i = transform->mat[2][2];

  const float cofactors[3][3] = {
    {(e * i) - (f * h), (f * g) - (d * i), (d * h) - (e * g)},
    {(c * h) - (b * i), (a * i) - (c * g), (b * g) - (a * h)},
    {(b * f) - (c * e), (c * d) - (a * f), (a * e) - (b * d)}};

  const float determinant =
    (a * cofactors[0][0]) + (b * cofactors[0][1]) + (c * cofactors[0][2]);

  fmatrix_3x3 normal_matrix;

  if (determinant < 0.0f) {
    const float negated_cofactors[3][3] = {
      {-cofactors[0][0], -cofactors[0][1], -cofactors[0][2]},
      {-cofactors[1][0], -cofactors[1][1], -cofactors[1][2]},
      {-cofactors[2][0], -cofactors[2][1], -cofactors[2][2]}};

    fmatrix_3x3_set(&normal_matrix, negated_cofactors);
  }
  else {
    fmatrix_3x3_set(&normal_matrix, cofactors);
  }

  return normal_matrix;
}

/******************************************************************************
 *
 * Function: renderer_3d_transform_normal()
 *
 ******************************************************************************/
static fvector3 renderer_3d_transform_normal(
  const fmatrix_3x3* normal_matrix,
  const fvector3 normal)
{
  fvector3 resultant = {{0.0f, 0.0f, 0.0f}};

  // Row vector convention, matching fmatrix_4x4_transform_point
  for (int col = 0; col < 3; col++) {
    for (int row = 0; row < 3; row++) {
      resultant.vec[col] += (normal.vec[row] * normal_matrix->mat[row][col]);
    }
  }

  return resultant;
}

/******************************************************************************
 *
 * Function: renderer_3d_set_camera_uniforms()
 *
 ******************************************************************************/
static void renderer_3d_set_camera_uniforms(
  struct renderer_3d_t* renderer,
  struct shader_program_t* shader)
{
  shader_program_set_uniform_fmat4x4(
    shader,
    "model",
    &(renderer->render_data.model_matrix));
  shader_program_set_uniform_fmat4x4(
    shader,
    "view",
    &(renderer->base.camera->view_matrix));
  shader_program_set_uniform_fmat4x4(
    shader,
    "projection",
    &(renderer->base.camera->projection_matrix));
}

/******************************************************************************
 *
 * Function: renderer_3d_set_light_uniforms()
 *
 ******************************************************************************/
static void renderer_3d_set_light_uniforms(
  struct renderer_3d_t* renderer,
  struct shader_program_t* shader)
{
  const struct renderer_3d_light_t* light = &(renderer->render_data.light);

  shader_program_set_uniform_fvec3(
    shader,
    "u_lightDirection",
    &(light->direction));
  shader_program_set_uniform_fvec3(
    shader,
    "u_lightColor",
    &(light->color));
  shader_program_set_uniform_float(
    shader,
    "u_ambientStrength",
    light->ambient_strength);
}

/******************************************************************************
 *
 * Function: renderer_3d_flush_shaded()
 *
 ******************************************************************************/
static void renderer_3d_flush_shaded(
  struct renderer_3d_t* renderer,
  struct renderable_3d_shaded_data_t* shaded_data)
{
  if (shaded_data->index_count) {
    const unsigned int shaded_data_size =
      (unsigned int)((uint8_t*)shaded_data->vertex_data_ptr -
                     (uint8_t*)shaded_data->vertex_data_base);

    vertex_array_bind(&(shaded_data->vao));
    vertex_buffer_bind(&(shaded_data->vbo));
    element_buffer_bind(&(shaded_data->ebo));

    vertex_buffer_buffer_sub_data(
      &(shaded_data->vbo),
      (float*)shaded_data->vertex_data_base,
      shaded_data_size);

    shader_program_use(&(renderer->render_data.shaded_shader));

    renderer_3d_set_camera_uniforms(
      renderer,
      &(renderer->render_data.shaded_shader));
    renderer_3d_set_light_uniforms(
      renderer,
      &(renderer->render_data.shaded_shader));

    render_api_draw_elements(
      DRAW_TYPE_TRIANGLES,
      shaded_data->index_count,
      ELEMENT_UNSIGNED_INT,
      0);
  }
}

/******************************************************************************
 *
 * Function: renderer_3d_flush()
 *
 ******************************************************************************/
static void renderer_3d_flush(
  struct renderer_3d_t* renderer)
{
  struct renderer_3d_data_t* data = &(renderer->render_data);

  renderer_3d_flush_shaded(renderer, &(data->cube_data));
  renderer_3d_flush_shaded(renderer, &(data->pyramid_data));
  renderer_3d_flush_shaded(renderer, &(data->sphere_data));

  if (data->textured_cube_data.index_count) {
    const unsigned int textured_data_size =
      (unsigned int)((uint8_t*)data->textured_cube_data.vertex_data_ptr -
                     (uint8_t*)data->textured_cube_data.vertex_data_base);

    vertex_array_bind(&(data->textured_cube_data.vao));
    vertex_buffer_bind(&(data->textured_cube_data.vbo));
    element_buffer_bind(&(data->textured_cube_data.ebo));

    vertex_buffer_buffer_sub_data(
      &(data->textured_cube_data.vbo),
      (float*)data->textured_cube_data.vertex_data_base,
      textured_data_size);

    shader_program_use(&(data->textured_shader));

    for (unsigned int i = 0; i < data->textures_index; i++) {
      texture_2d_bind(data->textures[i], i);
    }

    renderer_3d_set_camera_uniforms(renderer, &(data->textured_shader));
    renderer_3d_set_light_uniforms(renderer, &(data->textured_shader));

    char texture_uniform_name_buf[16];

    for (unsigned int i = 0; i < _RENDERER_3D_MAX_TEXTURES; i++) {
      sprintf(texture_uniform_name_buf, "u_textures[%u]", i);
      shader_program_set_uniform_int(
        &(data->textured_shader),
        texture_uniform_name_buf,
        i);
    }

    render_api_draw_elements(
      DRAW_TYPE_TRIANGLES,
      data->textured_cube_data.index_count,
      ELEMENT_UNSIGNED_INT,
      0);
  }

  if (data->line_data.line_vertex_count) {
    const unsigned int line_data_size =
      (unsigned int)((uint8_t*)data->line_data.vertex_data_ptr -
                     (uint8_t*)data->line_data.vertex_data_base);

    vertex_array_bind(&(data->line_data.vao));
    vertex_buffer_bind(&(data->line_data.vbo));

    vertex_buffer_buffer_sub_data(
      &(data->line_data.vbo),
      (float*)data->line_data.vertex_data_base,
      line_data_size);

    shader_program_use(&(data->line_shader));

    renderer_3d_set_camera_uniforms(renderer, &(data->line_shader));

    render_api_set_line_width(data->line_width);
    render_api_draw_lines(data->line_data.line_vertex_count);
  }
}

/******************************************************************************
 *
 * Function: renderer_3d_start_batch()
 *
 ******************************************************************************/
static void renderer_3d_start_batch(
  struct renderer_3d_t* renderer)
{
  struct renderer_3d_data_t* data = &(renderer->render_data);

  data->cube_data.index_count = 0;
  data->cube_data.vertex_data_ptr = data->cube_data.vertex_data_base;

  data->pyramid_data.index_count = 0;
  data->pyramid_data.vertex_data_ptr = data->pyramid_data.vertex_data_base;

  data->sphere_data.index_count = 0;
  data->sphere_data.vertex_data_ptr = data->sphere_data.vertex_data_base;

  data->textured_cube_data.index_count = 0;
  data->textured_cube_data.vertex_data_ptr =
    data->textured_cube_data.vertex_data_base;

  data->line_data.line_vertex_count = 0;
  data->line_data.vertex_data_ptr = data->line_data.vertex_data_base;

  data->textures_index = 0;
}

/******************************************************************************
 *
 * Function: renderer_3d_next_batch()
 *
 ******************************************************************************/
static void renderer_3d_next_batch(
  struct renderer_3d_t* renderer)
{
  renderer_3d_flush(renderer);
  renderer_3d_start_batch(renderer);
}

/******************************************************************************
 *
 * Function: renderer_3d_data_init()
 *
 ******************************************************************************/
static void renderer_3d_data_init(
  struct renderer_3d_data_t* data)
{
  unsigned int error = 0;

  error = shader_program_init_filepath(
    &(data->shaded_shader),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/shaded_3d_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/shaded_3d_shader.frag"));
#ifdef DEBUG
  if (error) {
    fprintf(stdout, "Error during shaded 3D shader init\n");
  }
#endif
  error = shader_program_init_filepath(
    &(data->textured_shader),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/textured_3d_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/textured_3d_shader.frag"));
#ifdef DEBUG
  if (error) {
    fprintf(stdout, "Error during textured 3D shader init\n");
  }
#endif
  error = shader_program_init_filepath(
    &(data->line_shader),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/line_3d_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/line_3d_shader.frag"));
#ifdef DEBUG
  if (error) {
    fprintf(stdout, "Error during line 3D shader init\n");
  }
#endif

  (void)error;

  fmatrix_4x4_init(&(data->model_matrix));

  renderable_3d_generate_mesh_data();

  renderable_3d_shaded_data_init(
    &(data->cube_data),
    &cube_3d_mesh,
    max_cubes,
    &(data->shaded_shader));
  renderable_3d_shaded_data_init(
    &(data->pyramid_data),
    &pyramid_3d_mesh,
    max_pyramids,
    &(data->shaded_shader));
  renderable_3d_shaded_data_init(
    &(data->sphere_data),
    &sphere_3d_mesh,
    max_spheres,
    &(data->shaded_shader));
  renderable_3d_textured_data_init(
    &(data->textured_cube_data),
    &cube_3d_mesh,
    max_textured_cubes,
    &(data->textured_shader));
  renderable_3d_line_data_init(
    &(data->line_data),
    max_lines,
    &(data->line_shader));

  // Default light - white, shining down and away from the default camera
  fvector3_set(&(data->light.direction), -0.4f, -1.0f, -0.6f);
  fvector3_set(&(data->light.color), 1.0f, 1.0f, 1.0f);
  data->light.ambient_strength = 0.25f;

  data->line_width = 1.0f;

  data->textures_index = 0;
}

/******************************************************************************
 *
 * Function: renderer_3d_data_cleanup()
 *
 ******************************************************************************/
static void renderer_3d_data_cleanup(
  struct renderer_3d_data_t* data)
{
  renderable_3d_shaded_data_cleanup(&(data->cube_data));
  renderable_3d_shaded_data_cleanup(&(data->pyramid_data));
  renderable_3d_shaded_data_cleanup(&(data->sphere_data));
  renderable_3d_textured_data_cleanup(&(data->textured_cube_data));
  renderable_3d_line_data_cleanup(&(data->line_data));

  shader_program_destroy(&(data->shaded_shader));
  shader_program_destroy(&(data->textured_shader));
  shader_program_destroy(&(data->line_shader));

  data->line_width = -1.0f;
}

/******************************************************************************
 *
 * Function: renderer_3d_push_shaded_mesh()
 *
 ******************************************************************************/
static void renderer_3d_push_shaded_mesh(
  struct renderer_3d_t* renderer,
  struct renderable_3d_shaded_data_t* shaded_data,
  const fmatrix_4x4* transform,
  const fvector4 color)
{
  const struct renderable_3d_mesh_t* mesh = shaded_data->mesh;

  const unsigned int new_index_count =
    shaded_data->index_count + mesh->num_indices;

  if (new_index_count > shaded_data->max_index_count) {
    renderer_3d_next_batch(renderer);
  }

  const fmatrix_3x3 normal_matrix =
    renderer_3d_calculate_normal_matrix(transform);

  for (unsigned int v = 0; v < mesh->num_vertices; v++) {
    shaded_data->vertex_data_ptr->position =
      fmatrix_4x4_transform_point(transform, mesh->positions[v]);
    shaded_data->vertex_data_ptr->normal =
      renderer_3d_transform_normal(&normal_matrix, mesh->normals[v]);
    shaded_data->vertex_data_ptr->color = color;

    shaded_data->vertex_data_ptr++;
  }

  shaded_data->index_count += mesh->num_indices;
}

/******************************************************************************
 *
 * Function: renderer_3d_init()
 *
 ******************************************************************************/
void renderer_3d_init(
  struct renderer_3d_t* renderer,
  struct camera_base_t* camera,
  struct window_t* parent_window,
  const char* tag)
{
  const uint16_t render_flags = (COLOR_FLAG | DEPTH_FLAG | BLEND_FLAG);

  renderer_base_init(
    &(renderer->base),
    render_flags,
    camera,
    parent_window,
    tag);

  renderer->base.renderer_begin_scene = &renderer_3d_begin_scene;
  renderer->base.renderer_end_scene = &renderer_3d_end_scene;

#ifdef DEBUG
  fprintf(stdout, "Initializing renderer_3d\n");
#endif

  renderer_3d_data_init(&(renderer->render_data));
}

/******************************************************************************
 *
 * Function: renderer_3d_cleanup()
 *
 ******************************************************************************/
void renderer_3d_cleanup(
  struct renderer_3d_t* renderer)
{
  vertex_array_unbind();

  renderer_3d_data_cleanup(&(renderer->render_data));

  renderer_base_cleanup(&(renderer->base));
}

/******************************************************************************
 *
 * Function: renderer_3d_begin_scene()
 *
 ******************************************************************************/
void renderer_3d_begin_scene(
  void* renderer)
{
  struct renderer_3d_t* renderer_3d = (struct renderer_3d_t*)renderer;

  renderer_3d_start_batch(renderer_3d);
}

/******************************************************************************
 *
 * Function: renderer_3d_end_scene()
 *
 ******************************************************************************/
void renderer_3d_end_scene(
  void* renderer)
{
  struct renderer_3d_t* renderer_3d = (struct renderer_3d_t*)renderer;

  renderer_3d_flush(renderer_3d);

  vertex_array_unbind();

  framebuffer_unbind();
}

/******************************************************************************
 *
 * Function: renderer_3d_draw_cube()
 *
 ******************************************************************************/
void renderer_3d_draw_cube(
  struct renderer_3d_t* renderer,
  const fmatrix_4x4* transform,
  const fvector4 color)
{
  renderer_3d_push_shaded_mesh(
    renderer,
    &(renderer->render_data.cube_data),
    transform,
    color);
}

/******************************************************************************
 *
 * Function: renderer_3d_draw_unfilled_cube()
 *
 ******************************************************************************/
void renderer_3d_draw_unfilled_cube(
  struct renderer_3d_t* renderer,
  const fmatrix_4x4* transform,
  const fvector4 color)
{
  const unsigned int num_edge_points =
    sizeof(cube_3d_edge_data) / sizeof(cube_3d_edge_data[0]);

  for (unsigned int i = 0; i < num_edge_points; i += 2) {
    renderer_3d_draw_line(
      renderer,
      fmatrix_4x4_transform_point(transform, cube_3d_edge_data[i]),
      fmatrix_4x4_transform_point(transform, cube_3d_edge_data[i + 1]),
      color);
  }
}

/******************************************************************************
 *
 * Function: renderer_3d_draw_textured_cube()
 *
 ******************************************************************************/
void renderer_3d_draw_textured_cube(
  struct renderer_3d_t* renderer,
  const fmatrix_4x4* transform,
  struct texture_2d_t* texture)
{
  struct renderer_3d_data_t* data = &(renderer->render_data);
  struct renderable_3d_textured_data_t* textured_data =
    &(data->textured_cube_data);

  const struct renderable_3d_mesh_t* mesh = textured_data->mesh;

  const float tiling_factor = 1.0f;

  const unsigned int new_index_count =
    textured_data->index_count + mesh->num_indices;

  if (new_index_count > textured_data->max_index_count) {
    renderer_3d_next_batch(renderer);
  }

  float texture_index = -1.0f;
  for (unsigned int i = 0; i < data->textures_index; i++) {
    if (data->textures[i]->texture_id == texture->texture_id) {
      texture_index = (float)i;
      break;
    }
  }

  if (texture_index < 0.0f) {
    if (data->textures_index >= _RENDERER_3D_MAX_TEXTURES) {
      renderer_3d_next_batch(renderer);
    }

    texture_index = (float)data->textures_index;
    data->textures[data->textures_index] = texture;
    data->textures_index++;
  }

  const fmatrix_3x3 normal_matrix =
    renderer_3d_calculate_normal_matrix(transform);

  for (unsigned int v = 0; v < mesh->num_vertices; v++) {
    textured_data->vertex_data_ptr->position =
      fmatrix_4x4_transform_point(transform, mesh->positions[v]);
    textured_data->vertex_data_ptr->normal =
      renderer_3d_transform_normal(&normal_matrix, mesh->normals[v]);
    textured_data->vertex_data_ptr->texture_coordinate =
      mesh->texture_coordinates[v];
    textured_data->vertex_data_ptr->texture_index = texture_index;
    textured_data->vertex_data_ptr->tiling_factor = tiling_factor;

    textured_data->vertex_data_ptr++;
  }

  textured_data->index_count += mesh->num_indices;
}

/******************************************************************************
 *
 * Function: renderer_3d_draw_pyramid()
 *
 ******************************************************************************/
void renderer_3d_draw_pyramid(
  struct renderer_3d_t* renderer,
  const fmatrix_4x4* transform,
  const fvector4 color)
{
  renderer_3d_push_shaded_mesh(
    renderer,
    &(renderer->render_data.pyramid_data),
    transform,
    color);
}

/******************************************************************************
 *
 * Function: renderer_3d_draw_sphere()
 *
 ******************************************************************************/
void renderer_3d_draw_sphere(
  struct renderer_3d_t* renderer,
  const fmatrix_4x4* transform,
  const fvector4 color)
{
  renderer_3d_push_shaded_mesh(
    renderer,
    &(renderer->render_data.sphere_data),
    transform,
    color);
}

/******************************************************************************
 *
 * Function: renderer_3d_draw_line()
 *
 ******************************************************************************/
void renderer_3d_draw_line(
  struct renderer_3d_t* renderer,
  const fvector3 pos_1,
  const fvector3 pos_2,
  const fvector4 color)
{
  struct renderable_3d_line_data_t* line_data =
    &(renderer->render_data.line_data);

  if ((line_data->line_vertex_count + 2) > line_data->max_line_vertex_count) {
    renderer_3d_next_batch(renderer);
  }

  line_data->vertex_data_ptr->position = pos_1;
  line_data->vertex_data_ptr->color = color;

  line_data->vertex_data_ptr++;

  line_data->vertex_data_ptr->position = pos_2;
  line_data->vertex_data_ptr->color = color;

  line_data->vertex_data_ptr++;

  line_data->line_vertex_count += 2;
}

/******************************************************************************
 *
 * Function: renderer_3d_set_line_width()
 *
 ******************************************************************************/
void renderer_3d_set_line_width(
  struct renderer_3d_t* renderer,
  const float line_width)
{
  renderer->render_data.line_width = line_width;
}

/******************************************************************************
 *
 * Function: renderer_3d_set_light()
 *
 ******************************************************************************/
void renderer_3d_set_light(
  struct renderer_3d_t* renderer,
  const fvector3 direction,
  const fvector3 color,
  const float ambient_strength)
{
  struct renderer_3d_light_t* light = &(renderer->render_data.light);

  fvector3_setv(&(light->direction), direction);
  fvector3_setv(&(light->color), color);
  light->ambient_strength = ambient_strength;
}
