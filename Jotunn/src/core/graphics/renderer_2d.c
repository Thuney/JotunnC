#include "renderer_2d.h"

#include "framebuffer.h"
#include "window.h"

#include <memory.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>

static void renderer_2d_flush(struct renderer_2d_t *renderer)
{
   struct renderer_2d_data_t *data = &renderer->render_data;

   struct camera_ortho_t *camera_ortho = (struct camera_ortho_t *)renderer->base.camera;

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
      const unsigned int triangle_data_size = (unsigned int)((uint8_t *)data->triangle_data.vertex_data_ptr - (uint8_t *)data->triangle_data.vertex_data_base);

      vertex_array_bind(&data->triangle_data.vao);
      vertex_buffer_bind(&data->triangle_data.vbo);
      element_buffer_bind(&data->triangle_data.ebo);

      vertex_buffer_buffer_sub_data(&data->triangle_data.vbo, (float *)data->triangle_data.vertex_data_base, triangle_data_size);

      shader_program_use(&data->triangle_shader);

      shader_program_set_uniform_fmat4x4(&data->triangle_shader, "model", &renderer->render_data.model_matrix);
      shader_program_set_uniform_fmat4x4(&data->triangle_shader, "view", &renderer->base.camera->view_matrix);
      shader_program_set_uniform_fmat4x4(&data->triangle_shader, "projection", &renderer->base.camera->projection_matrix);

      render_api_draw_elements(DRAW_TYPE_TRIANGLES, data->triangle_data.triangle_index_count, ELEMENT_UNSIGNED_INT, 0);
   }

   if (data->quad_data.quad_index_count)
   {
      const unsigned int quad_data_size = (unsigned int)((uint8_t *)data->quad_data.vertex_data_ptr - (uint8_t *)data->quad_data.vertex_data_base);

      vertex_array_bind(&data->quad_data.vao);
      vertex_buffer_bind(&data->quad_data.vbo);
      element_buffer_bind(&data->quad_data.ebo);

      vertex_buffer_buffer_sub_data(&data->quad_data.vbo, (float *)data->quad_data.vertex_data_base, quad_data_size);

      shader_program_use(&data->quad_shader);

      shader_program_set_uniform_fmat4x4(&data->quad_shader, "model", &renderer->render_data.model_matrix);
      shader_program_set_uniform_fmat4x4(&data->quad_shader, "view", &renderer->base.camera->view_matrix);
      shader_program_set_uniform_fmat4x4(&data->quad_shader, "projection", &renderer->base.camera->projection_matrix);

      render_api_draw_elements(DRAW_TYPE_TRIANGLES, data->quad_data.quad_index_count, ELEMENT_UNSIGNED_INT, 0);
   }

   if (data->textured_quad_data.textured_quad_index_count)
   {
      const unsigned int textured_quad_data_size = (unsigned int)((uint8_t *)data->textured_quad_data.vertex_data_ptr - (uint8_t *)data->textured_quad_data.vertex_data_base);

      vertex_array_bind(&data->textured_quad_data.vao);
      vertex_buffer_bind(&data->textured_quad_data.vbo);
      element_buffer_bind(&data->textured_quad_data.ebo);

      vertex_buffer_buffer_sub_data(&data->textured_quad_data.vbo, (float *)data->textured_quad_data.vertex_data_base, textured_quad_data_size);

      shader_program_use(&data->textured_quad_shader);

      for (unsigned int i = 0; i < data->textures_index; i++)
      {
         const struct texture_2d_t *texture = data->textures[i];

         // #ifdef DEBUG
         //    fprintf(stdout, "Binding texture with texture_id = %u to slot %u\n", texture->texture_id, i);
         // #endif

         texture_2d_bind(texture, i);
      }

      shader_program_set_uniform_fmat4x4(&data->textured_quad_shader, "model", &renderer->render_data.model_matrix);
      shader_program_set_uniform_fmat4x4(&data->textured_quad_shader, "view", &renderer->base.camera->view_matrix);
      shader_program_set_uniform_fmat4x4(&data->textured_quad_shader, "projection", &renderer->base.camera->projection_matrix);

      char texture_uniform_name_buf[16];

      for (unsigned int i = 0; i < _RENDERER_2D_MAX_TEXTURES; i++)
      {
         sprintf(texture_uniform_name_buf, "u_textures[%u]", i);
         shader_program_set_uniform_int(&data->textured_quad_shader, texture_uniform_name_buf, i);
      }

      render_api_draw_elements(DRAW_TYPE_TRIANGLES, data->textured_quad_data.textured_quad_index_count, ELEMENT_UNSIGNED_INT, 0);
   }

   if (data->circle_data.circle_index_count)
   {
      const unsigned int circle_data_size = (unsigned int)((uint8_t *)data->circle_data.vertex_data_ptr - (uint8_t *)data->circle_data.vertex_data_base);

      vertex_array_bind(&data->circle_data.vao);
      vertex_buffer_bind(&data->circle_data.vbo);
      element_buffer_bind(&data->circle_data.ebo);

      vertex_buffer_buffer_sub_data(&data->circle_data.vbo, (float *)data->circle_data.vertex_data_base, circle_data_size);

      shader_program_use(&data->circle_shader);

      shader_program_set_uniform_fmat4x4(&data->circle_shader, "model", &renderer->render_data.model_matrix);
      shader_program_set_uniform_fmat4x4(&data->circle_shader, "view", &renderer->base.camera->view_matrix);
      shader_program_set_uniform_fmat4x4(&data->circle_shader, "projection", &renderer->base.camera->projection_matrix);

      render_api_draw_elements(DRAW_TYPE_TRIANGLES, data->circle_data.circle_index_count, ELEMENT_UNSIGNED_INT, 0);
   }

   if (data->line_data.line_vertex_count)
   {
      const unsigned int line_data_size = (unsigned int)((uint8_t *)data->line_data.vertex_data_ptr - (uint8_t *)data->line_data.vertex_data_base);

      vertex_array_bind(&data->line_data.vao);
      vertex_buffer_bind(&data->line_data.vbo);

      vertex_buffer_buffer_sub_data(&data->line_data.vbo, (float *)data->line_data.vertex_data_base, line_data_size);

      shader_program_use(&data->line_shader);

      shader_program_set_uniform_fmat4x4(&data->line_shader, "model", &renderer->render_data.model_matrix);
      shader_program_set_uniform_fmat4x4(&data->line_shader, "view", &renderer->base.camera->view_matrix);
      shader_program_set_uniform_fmat4x4(&data->line_shader, "projection", &renderer->base.camera->projection_matrix);

      render_api_set_line_width(data->line_width);
      render_api_draw_lines(data->line_data.line_vertex_count);
   }
}

static void renderer_2d_start_batch(struct renderer_2d_t *renderer)
{
   // #ifdef DEBUG
   //    fprintf(stdout, "Renderer 2D starting batch\n");
   // #endif

   struct renderer_2d_data_t *data = &renderer->render_data;

   data->triangle_data.triangle_index_count = 0;
   data->triangle_data.vertex_data_ptr = data->triangle_data.vertex_data_base;

   data->quad_data.quad_index_count = 0;
   data->quad_data.vertex_data_ptr = data->quad_data.vertex_data_base;

   data->textured_quad_data.textured_quad_index_count = 0;
   data->textured_quad_data.vertex_data_ptr = data->textured_quad_data.vertex_data_base;

   data->circle_data.circle_index_count = 0;
   data->circle_data.vertex_data_ptr = data->circle_data.vertex_data_base;

   data->line_data.line_vertex_count = 0;
   data->line_data.vertex_data_ptr = data->line_data.vertex_data_base;

   data->textures_index = 0;
}

static void renderer_2d_next_batch(struct renderer_2d_t *renderer)
{
   renderer_2d_flush(renderer);
   renderer_2d_start_batch(renderer);
}

static const unsigned int max_quads = 1000; // Our upper limit on quads to draw. Translates into a limitation on max vertices / indices, technically, to include things like triangles, circles, and lines
static const unsigned int max_vertices = max_quads * 4;
static const unsigned int max_indices = max_quads * 6;

//

static void renderer_2d_data_init(struct renderer_2d_data_t *data)
{
   unsigned int error = 0;

   error = shader_program_init_filepath(&data->triangle_shader, "/home/loki/Projects/JotunnC/Jotunn/res/shaders/triangle_2d_shader.vert", "/home/loki/Projects/JotunnC/Jotunn/res/shaders/triangle_2d_shader.frag");
#ifdef DEBUG
   if (error)
      fprintf(stdout, "Error during triangle shader init\n");
#endif
   error = shader_program_init_filepath(&data->quad_shader, "/home/loki/Projects/JotunnC/Jotunn/res/shaders/quad_2d_shader.vert", "/home/loki/Projects/JotunnC/Jotunn/res/shaders/quad_2d_shader.frag");
#ifdef DEBUG
   if (error)
      fprintf(stdout, "Error during quad shader init\n");
#endif
   error = shader_program_init_filepath(&data->textured_quad_shader, "/home/loki/Projects/JotunnC/Jotunn/res/shaders/textured_quad_2d_shader.vert", "/home/loki/Projects/JotunnC/Jotunn/res/shaders/textured_quad_2d_shader.frag");
#ifdef DEBUG
   if (error)
      fprintf(stdout, "Error during quad shader init\n");
#endif
   error = shader_program_init_filepath(&data->circle_shader, "/home/loki/Projects/JotunnC/Jotunn/res/shaders/circle_2d_shader.vert", "/home/loki/Projects/JotunnC/Jotunn/res/shaders/circle_2d_shader.frag");
#ifdef DEBUG
   if (error)
      fprintf(stdout, "Error during circle shader init\n");
#endif
   error = shader_program_init_filepath(&data->line_shader, "/home/loki/Projects/JotunnC/Jotunn/res/shaders/line_2d_shader.vert", "/home/loki/Projects/JotunnC/Jotunn/res/shaders/line_2d_shader.frag");
#ifdef DEBUG
   if (error)
      fprintf(stdout, "Error during line shader init\n");
#endif

   fmatrix_4x4_init(&data->model_matrix);

   renderable_2d_triangle_data_init(&data->triangle_data, max_vertices, &data->triangle_shader);
   renderable_2d_quad_data_init(&data->quad_data, max_vertices, &data->quad_shader);
   renderable_2d_textured_quad_data_init(&data->textured_quad_data, max_vertices, &data->textured_quad_shader);
   renderable_2d_circle_data_init(&data->circle_data, max_vertices, &data->circle_shader);
   renderable_2d_line_data_init(&data->line_data, max_vertices, &data->line_shader);

   data->line_width = 5.0f;

   data->textures_index = 0;
}

static void renderer_2d_data_cleanup(struct renderer_2d_data_t *data)
{
   // #ifdef DEBUG
   //    fprintf(stdout, "Renderer 2D data cleanup\n");
   // #endif

   renderable_2d_triangle_data_cleanup(&data->triangle_data);
   renderable_2d_quad_data_cleanup(&data->quad_data);
   renderable_2d_textured_quad_data_cleanup(&data->textured_quad_data);
   renderable_2d_circle_data_cleanup(&data->circle_data);
   renderable_2d_line_data_cleanup(&data->line_data);

   shader_program_destroy(&data->triangle_shader);
   shader_program_destroy(&data->quad_shader);
   shader_program_destroy(&data->textured_quad_shader);
   shader_program_destroy(&data->circle_shader);
   shader_program_destroy(&data->line_shader);

   data->line_width = -1.0f;
}

void renderer_2d_init(struct renderer_2d_t *renderer, struct camera_base_t *camera, struct window_t *parent_window, const char *tag)
{
   const uint16_t render_flags = (COLOR_FLAG | DEPTH_FLAG | BLEND_FLAG);

   renderer_base_init(&renderer->base, render_flags, camera, parent_window, tag);

   renderer->base.renderer_begin_scene = &renderer_2d_begin_scene;
   renderer->base.renderer_end_scene   = &renderer_2d_end_scene;

#ifdef DEBUG
   fprintf(stdout, "Initializing renderer_2d\n");
#endif

   // Batch stuff
   renderer_2d_data_init(&renderer->render_data);
   renderer_2d_set_line_width(renderer, renderer->render_data.line_width);
}

void renderer_2d_cleanup(struct renderer_2d_t *renderer)
{
   // #ifdef DEBUG
   //      fprintf(stdout, "Cleaning up renderer 2D\n");
   // #endif

   vertex_array_unbind();

   renderer_2d_data_cleanup(&renderer->render_data);
}

void renderer_2d_begin_scene(void *renderer)
{
   struct renderer_2d_t *renderer_2d = (struct renderer_2d_t *)renderer;
   // #ifdef DEBUG
   //    fprintf(stdout, "Renderer 2D beginning scene\n");
   // #endif

   renderer_2d_start_batch(renderer_2d);
}

void renderer_2d_end_scene(void *renderer)
{
   struct renderer_2d_t *renderer_2d = (struct renderer_2d_t *)renderer;
   // #ifdef DEBUG
   //    fprintf(stdout, "Renderer 2D ending scene\n");
   // #endif

   renderer_2d_flush(renderer_2d);

   vertex_array_unbind();

   framebuffer_unbind();
}

//

void renderer_2d_draw_triangle(struct renderer_2d_t *renderer, const fmatrix_4x4 *transform, const fvector4 color)
{
   // #ifdef DEBUG
   //    fprintf(stdout, "Renderer 2D drawing triangle\n");
   // #endif

   struct renderer_2d_data_t *data = &renderer->render_data;

   const unsigned int triangle_vertex_count = 3;

   if (data->triangle_data.triangle_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   unsigned int i;
   for (i = 0; i < triangle_vertex_count; i++)
   {
      data->triangle_data.vertex_data_ptr->position = fmatrix_4x4_transform_point(transform, triangle_2d_position_data[i]);
      data->triangle_data.vertex_data_ptr->color = color;

      data->triangle_data.vertex_data_ptr++;
   }

   data->triangle_data.triangle_index_count += 3;
}

void renderer_2d_draw_quad(struct renderer_2d_t *renderer, const fmatrix_4x4 *transform, const fvector4 color)
{
   // #ifdef DEBUG
   //    fprintf(stdout, "Renderer 2D drawing quad\n");
   // #endif

   struct renderer_2d_data_t *data = &renderer->render_data;

   const unsigned int quad_vertex_count = 4;

   if (data->quad_data.quad_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   unsigned int i;
   for (i = 0; i < quad_vertex_count; i++)
   {
      data->quad_data.vertex_data_ptr->position = fmatrix_4x4_transform_point(transform, quad_2d_position_data[i]);
      data->quad_data.vertex_data_ptr->color = color;

      data->quad_data.vertex_data_ptr++;
   }

   data->quad_data.quad_index_count += 6;
}

void renderer_2d_draw_unfilled_quad(struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const fvector4 color)
{
   // #ifdef DEBUG
   //    fprintf(stdout, "Renderer 2D drawing unfilled quad\n");
   // #endif

   renderer_2d_draw_line(
      renderer, 
      fmatrix_4x4_transform_point(transform, quad_2d_position_data[0]), 
      fmatrix_4x4_transform_point(transform, quad_2d_position_data[1]), 
      color);

   renderer_2d_draw_line(
      renderer, 
      fmatrix_4x4_transform_point(transform, quad_2d_position_data[1]), 
      fmatrix_4x4_transform_point(transform, quad_2d_position_data[2]), 
      color);

   renderer_2d_draw_line(
      renderer, 
      fmatrix_4x4_transform_point(transform, quad_2d_position_data[2]), 
      fmatrix_4x4_transform_point(transform, quad_2d_position_data[3]), 
      color);

   renderer_2d_draw_line(
      renderer, 
      fmatrix_4x4_transform_point(transform, quad_2d_position_data[3]), 
      fmatrix_4x4_transform_point(transform, quad_2d_position_data[0]), 
      color);
}

void renderer_2d_draw_textured_quad(struct renderer_2d_t *renderer, const fmatrix_4x4 *transform, struct texture_2d_t *texture)
{
   // #ifdef DEBUG
   //    fprintf(stdout, "Renderer 2D drawing textured quad\n");
   // #endif

   struct renderer_2d_data_t *data = &renderer->render_data;

   const unsigned int quad_vertex_count = 4;

   const float tiling_factor = 1.0f;

   if (data->quad_data.quad_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   float texture_index = -1.0f;
   for (unsigned int i = 0; i < data->textures_index; i++)
   {
      const unsigned int this_texture_id = (data->textures[i])->texture_id;

      if ((this_texture_id) == (texture->texture_id))
      {
         texture_index = (float)i;
         break;
      }
   }

   if (texture_index < 0)
   {
      if (data->textures_index >= _RENDERER_2D_MAX_TEXTURES)
         renderer_2d_next_batch(renderer);

      texture_index = (float)data->textures_index;
      data->textures[data->textures_index] = texture;
      data->textures_index++;
   }

   unsigned int i;
   for (i = 0; i < quad_vertex_count; i++)
   {
      const fvector3 new_position = fmatrix_4x4_transform_point(transform, textured_quad_2d_position_data[i]);
      const fvector2 new_texture_coordinates = textured_quad_texture_coordinates[i];
      const float new_texture_index = texture_index;
      const float new_tiling_factor = tiling_factor;

      data->textured_quad_data.vertex_data_ptr->position = new_position;
      data->textured_quad_data.vertex_data_ptr->texture_coordinate = new_texture_coordinates;
      data->textured_quad_data.vertex_data_ptr->texture_index = new_texture_index;
      data->textured_quad_data.vertex_data_ptr->tiling_factor = new_tiling_factor;

      data->textured_quad_data.vertex_data_ptr++;
   }

   data->textured_quad_data.textured_quad_index_count += 6;
}

void renderer_2d_draw_subtextured_quad(struct renderer_2d_t *renderer, const fmatrix_4x4 *transform, struct texture_2d_t *texture, const fvector2 subtexture_coords[4])
{
   // #ifdef DEBUG
   //    fprintf(stdout, "Renderer 2D drawing subtextured quad\n");
   // #endif

   struct renderer_2d_data_t *data = &renderer->render_data;

   const unsigned int quad_vertex_count = 4;

   const float tiling_factor = 1.0f;

   if (data->quad_data.quad_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   float texture_index = -1.0f;
   for (unsigned int i = 0; i < data->textures_index; i++)
   {
      const unsigned int this_texture_id = (data->textures[i])->texture_id;

      if ((this_texture_id) == (texture->texture_id))
      {
         texture_index = (float)i;
         break;
      }
   }

   if (texture_index < 0)
   {
      if (data->textures_index >= _RENDERER_2D_MAX_TEXTURES)
         renderer_2d_next_batch(renderer);

      texture_index = (float)data->textures_index;
      data->textures[data->textures_index] = texture;
      data->textures_index++;
   }

   unsigned int i;
   for (i = 0; i < quad_vertex_count; i++)
   {
      const fvector3 new_position = fmatrix_4x4_transform_point(transform, textured_quad_2d_position_data[i]);
      const fvector2 new_texture_coordinates = subtexture_coords[i];
      const float new_texture_index = texture_index;
      const float new_tiling_factor = tiling_factor;

      data->textured_quad_data.vertex_data_ptr->position = new_position;
      data->textured_quad_data.vertex_data_ptr->texture_coordinate = new_texture_coordinates;
      data->textured_quad_data.vertex_data_ptr->texture_index = new_texture_index;
      data->textured_quad_data.vertex_data_ptr->tiling_factor = new_tiling_factor;

      data->textured_quad_data.vertex_data_ptr++;
   }

   data->textured_quad_data.textured_quad_index_count += 6;
}

void renderer_2d_draw_circle(struct renderer_2d_t *renderer, const fmatrix_4x4 *transform, const fvector4 color)
{
   // #ifdef DEBUG
   //    fprintf(stdout, "Renderer 2D drawing circle\n");
   // #endif

   struct renderer_2d_data_t *data = &renderer->render_data;

   if (data->circle_data.circle_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   unsigned int i;
   for (i = 0; i < _NUM_CIRCLE_2D_VERTICES; i++)
   {
      data->circle_data.vertex_data_ptr->position = fmatrix_4x4_transform_point(transform, circle_2d_position_data[i]);
      data->circle_data.vertex_data_ptr->color = color;

      data->circle_data.vertex_data_ptr++;
   }

   data->circle_data.circle_index_count += (3 * (_RENDERABLE_2D_CIRCLE_RESOLUTION));
}

void renderer_2d_draw_line(struct renderer_2d_t *renderer, const fvector3 pos_1, const fvector3 pos_2, const fvector4 color)
{
   // #ifdef DEBUG
   //    fprintf(stdout, "Renderer 2D drawing line\n");
   // #endif

   struct renderer_2d_data_t *data = &renderer->render_data;

   data->line_data.vertex_data_ptr->position = pos_1;
   data->line_data.vertex_data_ptr->color = color;

   data->line_data.vertex_data_ptr++;

   data->line_data.vertex_data_ptr->position = pos_2;
   data->line_data.vertex_data_ptr->color = color;

   data->line_data.vertex_data_ptr++;

   data->line_data.line_vertex_count += 2;
}

void renderer_2d_draw_string(struct renderer_2d_t *renderer, const struct typeface_t *typeface, const fvector3 start_position, const char *draw_string)
{
   // #ifdef DEBUG
   //    fprintf(stdout, "Renderer 2D drawing string\n");
   // #endif

   unsigned int num_chars = strlen(draw_string);
   fvector2 text_dimensions = typeface_calculate_string_dimensions(typeface, draw_string, num_chars);

   int error;

   fmatrix_4x4 scale_matrix, translation_matrix, transform;
   fmatrix_4x4_init(&scale_matrix);
   fmatrix_4x4_init(&translation_matrix);

   fvector3 string_current_render_position = {start_position.comp.x, (start_position.comp.y - text_dimensions.comp.y), 0.0f};

   fvector3 texture_dimensions_scale_factors, character_x_translation, character_local_translation;

   const struct glyph_t *character_glyph;

   unsigned int i;
   for (i = 0; i < num_chars; i++)
   {
      char c = draw_string[i];

      character_glyph = typeface_get_glyph_from_char(typeface, c);

      fvector3_set(&texture_dimensions_scale_factors, 
                  (float)(character_glyph->width), 
                  (float)(character_glyph->height), 
                  1.0f);

      fvector3_set(&character_local_translation, 
                  (float)character_glyph->offset_x, 
                  -1.0f * (float)character_glyph->offset_y,
                  0.0f);

      translation_matrix = fmatrix_4x4_transform_translate(&translation_matrix, character_local_translation);
      translation_matrix = fmatrix_4x4_transform_translate(&translation_matrix, string_current_render_position);

      scale_matrix = fmatrix_4x4_transform_scale(&scale_matrix, texture_dimensions_scale_factors);

      transform = fmatrix_4x4_multiply(&scale_matrix, &translation_matrix);

      const fvector2 *p0, *p1;
      p0 = &(character_glyph->glyph_texture.subtexture_coordinates[0]);
      p1 = &(character_glyph->glyph_texture.subtexture_coordinates[1]);

      const fvector2 subtexture_coords[4] =
         {
            {p0->comp.x, p1->comp.y},
            {p1->comp.x, p1->comp.y},
            {p1->comp.x, p0->comp.y},
            {p0->comp.x, p0->comp.y}
         };

      renderer_2d_draw_subtextured_quad(renderer, &transform, character_glyph->glyph_texture.parent_texture, subtexture_coords);

      fvector3_set(&character_x_translation, ((float)(character_glyph->advance_x)), 0.0f, 0.0f);
      string_current_render_position = fvector3_add(&string_current_render_position, &character_x_translation);

      fmatrix_4x4_identity(&scale_matrix);
      fmatrix_4x4_identity(&translation_matrix);
   }
}

void renderer_2d_set_line_width(struct renderer_2d_t *renderer, const float line_width)
{
   render_api_set_line_width(line_width);
}