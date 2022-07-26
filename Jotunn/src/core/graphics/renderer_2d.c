#include <memory.h>
#ifdef DEBUG
   #include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "fmatrix.h"
#include "font.h"
#include "renderable_2d_primitive.h"
#include "renderer_2d.h"
#include "shader.h"

#define _RENDERER_2D_MAX_TEXTURES 16

struct renderer_2d_data_t
{
   struct shader_program_t triangle_shader;
   struct shader_program_t quad_shader;
   struct shader_program_t textured_quad_shader;
   struct shader_program_t circle_shader;
   struct shader_program_t line_shader;
   //
   const struct texture_2d_t* textures[_RENDERER_2D_MAX_TEXTURES];
   unsigned int textures_index;
   //
   struct renderable_2d_triangle_data_t      triangle_data;
   struct renderable_2d_quad_data_t          quad_data;
   struct renderable_2d_textured_quad_data_t textured_quad_data;
   struct renderable_2d_circle_data_t        circle_data;
   struct renderable_2d_line_data_t          line_data;
   //
   float line_width;
};

static struct renderer_2d_data_t renderer_2d_data;

static fmatrix_4x4 model_matrix;

static void renderer_2d_flush(const struct renderer_2d_t* renderer)
{
   struct renderer_2d_data_t* data = &renderer_2d_data;

   if (data->triangle_data.triangle_index_count)
   {
      const unsigned int triangle_data_size = (unsigned int)((uint8_t*)data->triangle_data.vertex_data_ptr - (uint8_t*)data->triangle_data.vertex_data_base);

      vertex_array_bind(&data->triangle_data.vao);
      vertex_buffer_bind(&data->triangle_data.vbo);
      element_buffer_bind(&data->triangle_data.ebo);

      vertex_buffer_buffer_sub_data(&data->triangle_data.vbo, (float*)data->triangle_data.vertex_data_base, triangle_data_size);

      shader_program_use(&data->triangle_shader);

      shader_program_set_uniform_fmat4x4(&renderer_2d_data.triangle_shader, "model", &model_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.triangle_shader, "view", &renderer->camera.base.view_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.triangle_shader, "projection", &renderer->camera.base.projection_matrix);

      render_api_draw_elements(DRAW_TYPE_TRIANGLES, data->triangle_data.triangle_index_count, ELEMENT_UNSIGNED_INT, 0);
   }

   if (data->quad_data.quad_index_count)
   {
      const unsigned int quad_data_size = (unsigned int)((uint8_t*)data->quad_data.vertex_data_ptr - (uint8_t*)data->quad_data.vertex_data_base);

      vertex_array_bind(&data->quad_data.vao);
      vertex_buffer_bind(&data->quad_data.vbo);
      element_buffer_bind(&data->quad_data.ebo);

      vertex_buffer_buffer_sub_data(&data->quad_data.vbo, (float*)data->quad_data.vertex_data_base, quad_data_size);

      shader_program_use(&data->quad_shader);

      shader_program_set_uniform_fmat4x4(&renderer_2d_data.quad_shader, "model", &model_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.quad_shader, "view", &renderer->camera.base.view_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.quad_shader, "projection", &renderer->camera.base.projection_matrix);

      render_api_draw_elements(DRAW_TYPE_TRIANGLES, data->quad_data.quad_index_count, ELEMENT_UNSIGNED_INT, 0);
   }

   if (data->textured_quad_data.textured_quad_index_count)
   {
      const unsigned int textured_quad_data_size = (unsigned int)((uint8_t*)data->textured_quad_data.vertex_data_ptr - (uint8_t*)data->textured_quad_data.vertex_data_base);

      vertex_array_bind(&data->textured_quad_data.vao);
      vertex_buffer_bind(&data->textured_quad_data.vbo);
      element_buffer_bind(&data->textured_quad_data.ebo);

      vertex_buffer_buffer_sub_data(&data->textured_quad_data.vbo, (float*)data->textured_quad_data.vertex_data_base, textured_quad_data_size);

      for (unsigned int i = 0; i < renderer_2d_data.textures_index; i++)
      {
         const struct texture_2d_t* texture = renderer_2d_data.textures[i];

         // #ifdef DEBUG
         //    fprintf(stdout, "Binding texture with texture_id = %u to slot %u\n", texture->texture_id, i);
         // #endif

         texture_2d_bind(texture, i);
      }

      shader_program_use(&data->textured_quad_shader);

      shader_program_set_uniform_fmat4x4(&renderer_2d_data.textured_quad_shader, "model", &model_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.textured_quad_shader, "view", &renderer->camera.base.view_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.textured_quad_shader, "projection", &renderer->camera.base.projection_matrix);

      char texture_uniform_name_buf[15];

      for (unsigned int i = 0; i < _RENDERER_2D_MAX_TEXTURES; i++)
      {
         sprintf(texture_uniform_name_buf, "u_textures[%u]", i);
         shader_program_set_uniform_int(&renderer_2d_data.textured_quad_shader, texture_uniform_name_buf, i);
      }

      render_api_draw_elements(DRAW_TYPE_TRIANGLES, data->textured_quad_data.textured_quad_index_count, ELEMENT_UNSIGNED_INT, 0);
   }

   if (data->circle_data.circle_index_count)
   {
      const unsigned int circle_data_size = (unsigned int)((uint8_t*)data->circle_data.vertex_data_ptr - (uint8_t*)data->circle_data.vertex_data_base);

      vertex_array_bind(&data->circle_data.vao);
      vertex_buffer_bind(&data->circle_data.vbo);
      element_buffer_bind(&data->circle_data.ebo);

      vertex_buffer_buffer_sub_data(&data->circle_data.vbo, (float*)data->circle_data.vertex_data_base, circle_data_size);

      shader_program_use(&data->circle_shader);

      shader_program_set_uniform_fmat4x4(&renderer_2d_data.circle_shader, "model", &model_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.circle_shader, "view", &renderer->camera.base.view_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.circle_shader, "projection", &renderer->camera.base.projection_matrix);

      render_api_draw_elements(DRAW_TYPE_TRIANGLES, data->circle_data.circle_index_count, ELEMENT_UNSIGNED_INT, 0);
   }

   if (data->line_data.line_vertex_count)
   {
      const unsigned int line_data_size = (unsigned int)((uint8_t*)data->line_data.vertex_data_ptr - (uint8_t*)data->line_data.vertex_data_base);

      vertex_array_bind(&data->line_data.vao);
      vertex_buffer_bind(&data->line_data.vbo);

      vertex_buffer_buffer_sub_data(&data->line_data.vbo, (float*)data->line_data.vertex_data_base, line_data_size);

      shader_program_use(&data->line_shader);

      shader_program_set_uniform_fmat4x4(&renderer_2d_data.line_shader, "model", &model_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.line_shader, "view", &renderer->camera.base.view_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.line_shader, "projection", &renderer->camera.base.projection_matrix);

      render_api_set_line_width(data->line_width);
      render_api_draw_lines(data->line_data.line_vertex_count);
   }
}

static void renderer_2d_start_batch(const struct renderer_2d_t* renderer)
{
   struct renderer_2d_data_t* data = &renderer_2d_data;

   data->triangle_data.triangle_index_count = 0;
   data->triangle_data.vertex_data_ptr      = data->triangle_data.vertex_data_base;

   data->quad_data.quad_index_count = 0;
   data->quad_data.vertex_data_ptr  = data->quad_data.vertex_data_base;

   data->textured_quad_data.textured_quad_index_count = 0;
   data->textured_quad_data.vertex_data_ptr  = data->textured_quad_data.vertex_data_base;

   data->circle_data.circle_index_count = 0;
   data->circle_data.vertex_data_ptr    = data->circle_data.vertex_data_base;

   data->line_data.line_vertex_count = 0;
   data->line_data.vertex_data_ptr  = data->line_data.vertex_data_base;

   data->textures_index = 0;
}

static void renderer_2d_next_batch(const struct renderer_2d_t* renderer)
{
   renderer_2d_flush(renderer);
   renderer_2d_start_batch(renderer);
}

static const unsigned int max_quads     = 1000; // Our upper limit on quads to draw. Translates into a limitation on max vertices / indices, technically, to include things like triangles, circles, and lines
static const unsigned int max_vertices  = max_quads * 4;
static const unsigned int max_indices   = max_quads * 6;

//

static void renderer_2d_data_init(struct renderer_2d_data_t* data)
{
   unsigned int error = 0;

   error = shader_program_init_filepath(&data->triangle_shader, "../../../Jotunn/res/shaders/triangle_2d_shader.vert", "../../../Jotunn/res/shaders/triangle_2d_shader.frag");
   #ifdef DEBUG
      if (error) fprintf(stdout, "Error during triangle shader init\n");
   #endif
   error = shader_program_init_filepath(&data->quad_shader, "../../../Jotunn/res/shaders/quad_2d_shader.vert", "../../../Jotunn/res/shaders/quad_2d_shader.frag");
   #ifdef DEBUG
      if (error) fprintf(stdout, "Error during quad shader init\n");
   #endif
   error = shader_program_init_filepath(&data->textured_quad_shader, "../../../Jotunn/res/shaders/textured_quad_2d_shader.vert", "../../../Jotunn/res/shaders/textured_quad_2d_shader.frag");
   #ifdef DEBUG
      if (error) fprintf(stdout, "Error during quad shader init\n");
   #endif
   error = shader_program_init_filepath(&data->circle_shader, "../../../Jotunn/res/shaders/circle_2d_shader.vert", "../../../Jotunn/res/shaders/circle_2d_shader.frag");
   #ifdef DEBUG
      if (error) fprintf(stdout, "Error during circle shader init\n");
   #endif
   error = shader_program_init_filepath(&data->line_shader, "../../../Jotunn/res/shaders/line_2d_shader.vert", "../../../Jotunn/res/shaders/line_2d_shader.frag");
   #ifdef DEBUG
      if (error) fprintf(stdout, "Error during line shader init\n");
   #endif

   renderable_2d_triangle_data_init(&data->triangle_data, max_vertices, &data->triangle_shader);
   renderable_2d_quad_data_init(&data->quad_data, max_vertices, &data->quad_shader);
   renderable_2d_textured_quad_data_init(&data->textured_quad_data, max_vertices, &data->textured_quad_shader);
   renderable_2d_circle_data_init(&data->circle_data, max_vertices, &data->circle_shader);
   renderable_2d_line_data_init(&data->line_data, max_vertices, &data->line_shader);

   data->line_width = 3.0f;
}

static void renderer_2d_data_cleanup(struct renderer_2d_data_t* data)
{
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

void renderer_2d_init(struct renderer_2d_t* renderer, const char* tag, const float left, const float right, const float top, const float bottom, const float near_plane, const float far_plane)
{
   fmatrix_4x4_init(&model_matrix);
   // model_matrix = fmatrix_4x4_transform_scale(&model_matrix, primitive_scale_factors);

   #ifdef DEBUG
      fprintf(stdout, "Initializing renderer_2d\n");
   #endif

   memset(renderer, 0, sizeof(struct renderer_2d_t));

   int tag_length = strlen(tag);
   renderer->tag = (char*) malloc(tag_length*sizeof(char));    
   strcpy(renderer->tag, tag);

   render_api_init();

   // Camera stuff
   const fvector3 camera_position = (fvector3) { {0.0f, 0.0f,  2.0f} };
   const fvector3 camera_up       = (fvector3) { {0.0f, 1.0f,  0.0f} };
   const fvector3 camera_front    = (fvector3) { {0.0f, 0.0f, -1.0f} };

   camera_init_orthographic(&(renderer->camera), camera_position, camera_up, camera_front);
   camera_set_projection_orthographic(&(renderer->camera), left, right, top, bottom, near_plane, far_plane);

   // Batch stuff
   renderer_2d_data_init(&renderer_2d_data);
   renderer_2d_set_line_width(renderer, renderer_2d_data.line_width);
}

void renderer_2d_cleanup(struct renderer_2d_t* renderer)
{
   #ifdef DEBUG
        fprintf(stdout, "Cleaning up renderer 2D\n");
   #endif

   free(renderer->tag);
   renderer->tag = 0;

   vertex_array_unbind();

   renderer_2d_data_cleanup(&renderer_2d_data);
}

void renderer_2d_begin_scene(struct renderer_2d_t* renderer)
{
   render_api_clear();

   renderer_2d_start_batch(renderer);
}

void renderer_2d_end_scene(struct renderer_2d_t* renderer)
{
   renderer_2d_flush(renderer);

   vertex_array_unbind();
}

//

void renderer_2d_draw_triangle(const struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const fvector4 color)
{
   const unsigned int triangle_vertex_count = 3;

   if (renderer_2d_data.triangle_data.triangle_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   unsigned int i;
   for (i = 0; i < triangle_vertex_count; i++)
   {
      renderer_2d_data.triangle_data.vertex_data_ptr->position = fmatrix_4x4_transform_point(transform, triangle_2d_position_data[i]);
      renderer_2d_data.triangle_data.vertex_data_ptr->color    = color;

      renderer_2d_data.triangle_data.vertex_data_ptr++;
   }

   renderer_2d_data.triangle_data.triangle_index_count += 3;

}

void renderer_2d_draw_quad(const struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const fvector4 color)
{
   const unsigned int quad_vertex_count = 4;

   if (renderer_2d_data.quad_data.quad_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   unsigned int i;
   for (i = 0; i < quad_vertex_count; i++)
   {
      renderer_2d_data.quad_data.vertex_data_ptr->position = fmatrix_4x4_transform_point(transform, quad_2d_position_data[i]);
      renderer_2d_data.quad_data.vertex_data_ptr->color    = color;
      
      renderer_2d_data.quad_data.vertex_data_ptr++;
   }

   renderer_2d_data.quad_data.quad_index_count += 6;
}

void renderer_2d_draw_textured_quad(const struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const struct texture_2d_t* texture)
{
   const unsigned int quad_vertex_count = 4;

   const float tiling_factor = 1.0f;

   if (renderer_2d_data.quad_data.quad_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   float texture_index = 0.0f;
   for (unsigned int i = 0; i < renderer_2d_data.textures_index; i++)
   {
      if ( ((renderer_2d_data.textures[i])->texture_id) == (texture->texture_id) )
      {
         texture_index = (float)i;
         break;
      }
   }

   if (texture_index == 0.0f)
   {
      if (renderer_2d_data.textures_index >= _RENDERER_2D_MAX_TEXTURES)
         renderer_2d_next_batch(renderer);

      texture_index = (float)renderer_2d_data.textures_index;
      renderer_2d_data.textures[renderer_2d_data.textures_index] = texture;
      renderer_2d_data.textures_index++;
   }

   unsigned int i;
   for (i = 0; i < quad_vertex_count; i++)
   {
      const fvector3 new_position            = fmatrix_4x4_transform_point(transform, textured_quad_2d_position_data[i]);
      const fvector2 new_texture_coordinates = textured_quad_texture_coordinates[i];
      const float new_texture_index          = texture_index;
      const float new_tiling_factor          = tiling_factor;
      
      renderer_2d_data.textured_quad_data.vertex_data_ptr->position            = new_position;
      renderer_2d_data.textured_quad_data.vertex_data_ptr->texture_coordinate  = new_texture_coordinates;
      renderer_2d_data.textured_quad_data.vertex_data_ptr->texture_index       = new_texture_index;
      renderer_2d_data.textured_quad_data.vertex_data_ptr->tiling_factor       = new_tiling_factor;
      
      renderer_2d_data.textured_quad_data.vertex_data_ptr++;
   }

   renderer_2d_data.textured_quad_data.textured_quad_index_count += 6;
}

void renderer_2d_draw_subtextured_quad(const struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const struct texture_2d_t* texture, const fvector2 subtexture_coords[4])
{
   const unsigned int quad_vertex_count = 4;

   const float tiling_factor = 1.0f;

   if (renderer_2d_data.quad_data.quad_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   float texture_index = 0.0f;
   for (unsigned int i = 0; i < renderer_2d_data.textures_index; i++)
   {
      if ( ((renderer_2d_data.textures[i])->texture_id) == (texture->texture_id) )
      {
         texture_index = (float)i;
         break;
      }
   }

   if (texture_index == 0.0f)
   {
      if (renderer_2d_data.textures_index >= _RENDERER_2D_MAX_TEXTURES)
         renderer_2d_next_batch(renderer);

      texture_index = (float)renderer_2d_data.textures_index;
      renderer_2d_data.textures[renderer_2d_data.textures_index] = texture;
      renderer_2d_data.textures_index++;
   }

   unsigned int i;
   for (i = 0; i < quad_vertex_count; i++)
   {
      const fvector3 new_position            = fmatrix_4x4_transform_point(transform, textured_quad_2d_position_data[i]);
      const fvector2 new_texture_coordinates = subtexture_coords[i];
      const float new_texture_index          = texture_index;
      const float new_tiling_factor          = tiling_factor;

      renderer_2d_data.textured_quad_data.vertex_data_ptr->position            = new_position;
      renderer_2d_data.textured_quad_data.vertex_data_ptr->texture_coordinate  = new_texture_coordinates;
      renderer_2d_data.textured_quad_data.vertex_data_ptr->texture_index       = new_texture_index;
      renderer_2d_data.textured_quad_data.vertex_data_ptr->tiling_factor       = new_tiling_factor;
      
      renderer_2d_data.textured_quad_data.vertex_data_ptr++;
   }

   renderer_2d_data.textured_quad_data.textured_quad_index_count += 6;
}

void renderer_2d_draw_circle(const struct renderer_2d_t* renderer, const fmatrix_4x4* transform, const fvector4 color)
{
   if (renderer_2d_data.circle_data.circle_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   unsigned int i;
   for (i = 0; i < _NUM_CIRCLE_2D_VERTICES; i++)
   {
      renderer_2d_data.circle_data.vertex_data_ptr->position = fmatrix_4x4_transform_point(transform, circle_2d_position_data[i]);
      renderer_2d_data.circle_data.vertex_data_ptr->color    = color;
      
      renderer_2d_data.circle_data.vertex_data_ptr++;
   }

   renderer_2d_data.circle_data.circle_index_count += (3*(_RENDERABLE_2D_CIRCLE_RESOLUTION));
}

void renderer_2d_draw_line(const struct renderer_2d_t* renderer, const fvector3 pos_1, const fvector3 pos_2, const fvector4 color)
{
   renderer_2d_data.line_data.vertex_data_ptr->position = pos_1;
   renderer_2d_data.line_data.vertex_data_ptr->color    = color;

   renderer_2d_data.line_data.vertex_data_ptr++;

   renderer_2d_data.line_data.vertex_data_ptr->position = pos_2;
   renderer_2d_data.line_data.vertex_data_ptr->color    = color;

   renderer_2d_data.line_data.vertex_data_ptr++;

   renderer_2d_data.line_data.line_vertex_count += 2;
}

void renderer_2d_draw_string(const struct renderer_2d_t* renderer, const struct typeface_t* typeface, const fvector3 start_position, const char* draw_string)
{
   int error;

   fmatrix_4x4 scale_matrix, translation_matrix, transform;
   fmatrix_4x4_init(&scale_matrix);
   fmatrix_4x4_init(&translation_matrix);

   fvector3 string_current_render_position = { start_position.comp.x, start_position.comp.y, 0.0f };

   fvector3 texture_dimensions_scale_factors, character_x_translation, character_local_translation;

   unsigned int num_chars = strlen(draw_string);

   const struct glyph_t* character_glyph;

   unsigned int i;
   for (i = 0; i < num_chars; i++)
   {
      char c = draw_string[i];

      character_glyph = typeface_get_glyph_from_char(typeface, c);

      fvector3_set(&texture_dimensions_scale_factors, ((float)(character_glyph->width)), ((float)(character_glyph->height)), 1.0f);
      fvector3_set(&character_local_translation, (float)character_glyph->offset_x, -1.0f*(float)character_glyph->offset_y, 0.0f);

      translation_matrix = fmatrix_4x4_transform_translate(&translation_matrix, character_local_translation);
      translation_matrix = fmatrix_4x4_transform_translate(&translation_matrix, string_current_render_position);

      scale_matrix = fmatrix_4x4_transform_scale(&scale_matrix, texture_dimensions_scale_factors);

      transform = fmatrix_4x4_multiply(&scale_matrix, &translation_matrix);

      const fvector2 *p0, *p1;
      p0 = &(character_glyph->glyph_texture.subtexture_coordinates[0]);
      p1 = &(character_glyph->glyph_texture.subtexture_coordinates[1]);

      const fvector2 subtexture_coords[4] = 
      {
         { p0->comp.x, p1->comp.y }, 
         { p1->comp.x, p1->comp.y }, 
         { p1->comp.x, p0->comp.y }, 
         { p0->comp.x, p0->comp.y }
      };

      renderer_2d_draw_subtextured_quad(renderer, &transform, character_glyph->glyph_texture.parent_texture, subtexture_coords);

      fvector3_set(&character_x_translation, ( (float)(character_glyph->advance_x) ), 0.0f, 0.0f);
      string_current_render_position = fvector3_add(&string_current_render_position, &character_x_translation);

      fmatrix_4x4_identity(&scale_matrix);
      fmatrix_4x4_identity(&translation_matrix);
   }
}

void renderer_2d_set_line_width(const struct renderer_2d_t* renderer, const float line_width)
{
   render_api_set_line_width(line_width);
}