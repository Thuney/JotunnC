#include <memory.h>
#ifdef DEBUG
   #include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "fmatrix.h"
#include "renderable_2d_primitive.h"
#include "renderer_2d.h"
#include "shader.h"

struct renderer_2d_data_t
{
   struct shader_program_t triangle_shader;
   struct shader_program_t quad_shader;
   struct shader_program_t circle_shader;
   struct shader_program_t line_shader;
   //
   struct renderable_2d_triangle_data_t triangle_data;
   struct renderable_2d_quad_data_t         quad_data;
   struct renderable_2d_circle_data_t     circle_data;
   struct renderable_2d_line_data_t         line_data;
   //
   float line_width;
};

static struct renderer_2d_data_t renderer_2d_data;

#define PRIMITIVE_SCALE_FACTOR 50.0f
static const fvector3 primitive_scale_factors      = { {PRIMITIVE_SCALE_FACTOR, PRIMITIVE_SCALE_FACTOR, 0.0f} };

static fmatrix_4x4 model_matrix;

static void renderer_2d_flush(const struct renderer_2d_t* renderer)
{
   #ifdef DEBUG
      fprintf(stdout, "Flushing batch!\n");
   #endif

   struct renderer_2d_data_t* data = &renderer_2d_data;

   if (data->triangle_data.triangle_index_count)
   {
      #ifdef DEBUG
         fprintf(stdout, "Flushing triangles! Index count = %u\n", data->triangle_data.triangle_index_count);
         fprintf(stdout, "Triangle Data Ptr - %u Base Ptr - %u\n", data->triangle_data.vertex_data_ptr, data->triangle_data.vertex_data_base);
      #endif

      const unsigned int triangle_data_size = (unsigned int)((uint8_t*)data->triangle_data.vertex_data_ptr - (uint8_t*)data->triangle_data.vertex_data_base);

      #ifdef DEBUG
        fprintf(stdout, "Triangle data size = %d\n", triangle_data_size);
      #endif

      vertex_array_bind(&data->triangle_data.vao);
      vertex_buffer_bind(&data->triangle_data.vbo);
      element_buffer_bind(&data->triangle_data.ebo);

      shader_program_use(&data->triangle_shader);

      vertex_buffer_buffer_sub_data(&data->triangle_data.vbo, (float*)data->triangle_data.vertex_data_base, triangle_data_size);

      shader_program_set_uniform_fmat4x4(&renderer_2d_data.triangle_shader, "model", &model_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.triangle_shader, "view", &renderer->camera.base.view_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.triangle_shader, "projection", &renderer->camera.base.projection_matrix);

      render_api_draw_elements(DRAW_TYPE_TRIANGLES, data->triangle_data.triangle_index_count, ELEMENT_UNSIGNED_INT, 0);
   }

   if (data->quad_data.quad_index_count)
   {
      #ifdef DEBUG
         fprintf(stdout, "Flushing quads\n");
      #endif

      const unsigned int quad_data_size = (unsigned int)((uint8_t*)data->quad_data.vertex_data_ptr - (uint8_t*)data->quad_data.vertex_data_base);

      vertex_array_bind(&data->quad_data.vao);
      vertex_buffer_bind(&data->quad_data.vbo);
      element_buffer_bind(&data->quad_data.ebo);

      shader_program_use(&data->quad_shader);

      vertex_buffer_buffer_sub_data(&data->quad_data.vbo, (float*)data->quad_data.vertex_data_base, quad_data_size);

      shader_program_set_uniform_fmat4x4(&renderer_2d_data.quad_shader, "model", &model_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.quad_shader, "view", &renderer->camera.base.view_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.quad_shader, "projection", &renderer->camera.base.projection_matrix);

      render_api_draw_elements(DRAW_TYPE_TRIANGLES, data->quad_data.quad_index_count, ELEMENT_UNSIGNED_INT, 0);
   }

   if (data->circle_data.circle_index_count)
   {
      #ifdef DEBUG
         fprintf(stdout, "Flushing circles\n");
      #endif

      const unsigned int circle_data_size = (unsigned int)((uint8_t*)data->circle_data.vertex_data_ptr - (uint8_t*)data->circle_data.vertex_data_base);

      vertex_array_bind(&data->quad_data.vao);
      vertex_buffer_bind(&data->quad_data.vbo);
      element_buffer_bind(&data->quad_data.ebo);

      shader_program_use(&data->circle_shader);

      vertex_buffer_buffer_sub_data(&data->circle_data.vbo, (float*)data->circle_data.vertex_data_base, circle_data_size);

      shader_program_set_uniform_fmat4x4(&renderer_2d_data.circle_shader, "model", &model_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.circle_shader, "view", &renderer->camera.base.view_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.circle_shader, "projection", &renderer->camera.base.projection_matrix);

      render_api_draw_elements(DRAW_TYPE_TRIANGLES, data->circle_data.circle_index_count, ELEMENT_UNSIGNED_INT, 0);
   }

   if (data->line_data.line_vertex_count)
   {
      #ifdef DEBUG
         fprintf(stdout, "Flushing lines\n");
      #endif

      const unsigned int line_data_size = (unsigned int)((uint8_t*)data->line_data.vertex_data_ptr - (uint8_t*)data->line_data.vertex_data_base);

      vertex_array_bind(&data->line_data.vao);
      vertex_buffer_bind(&data->line_data.vbo);

      shader_program_use(&data->line_shader);

      vertex_buffer_buffer_sub_data(&data->line_data.vbo, (float*)data->line_data.vertex_data_base, line_data_size);

      shader_program_set_uniform_fmat4x4(&renderer_2d_data.line_shader, "model", &model_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.line_shader, "view", &renderer->camera.base.view_matrix);
      shader_program_set_uniform_fmat4x4(&renderer_2d_data.line_shader, "projection", &renderer->camera.base.projection_matrix);

      render_api_draw_lines(data->line_data.line_vertex_count);
   }
}

static void renderer_2d_start_batch(const struct renderer_2d_t* renderer)
{
   #ifdef DEBUG
      fprintf(stdout, "Starting batch!\n");
   #endif

   struct renderer_2d_data_t* data = &renderer_2d_data;

   data->triangle_data.triangle_index_count = 0;
   data->triangle_data.vertex_data_ptr      = data->triangle_data.vertex_data_base;

   data->quad_data.quad_index_count = 0;
   data->quad_data.vertex_data_ptr  = data->quad_data.vertex_data_base;

   data->circle_data.circle_index_count = 0;
   data->circle_data.vertex_data_ptr    = data->circle_data.vertex_data_base;

   data->line_data.line_vertex_count = 0;
   data->line_data.vertex_data_ptr  = data->line_data.vertex_data_base;
}

static void renderer_2d_next_batch(const struct renderer_2d_t* renderer)
{
   #ifdef DEBUG
         fprintf(stdout, "Next batch!\n");
   #endif

   renderer_2d_flush(renderer);
   renderer_2d_start_batch(renderer);
}

static const unsigned int max_quads     = 100; // Our upper limit on quads to draw. Translates into a limitation on max vertices / indices, technically, to include things like triangles, circles, and lines
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
   error = shader_program_init_filepath(&data->quad_shader,     "../../../Jotunn/res/shaders/quad_2d_shader.vert", "../../../Jotunn/res/shaders/quad_2d_shader.frag");
   #ifdef DEBUG
      if (error) fprintf(stdout, "Error during quad shader init\n");
   #endif
   error = shader_program_init_filepath(&data->circle_shader,   "../../../Jotunn/res/shaders/circle_2d_shader.vert", "../../../Jotunn/res/shaders/circle_2d_shader.frag");
   #ifdef DEBUG
      if (error) fprintf(stdout, "Error during circle shader init\n");
   #endif
   error = shader_program_init_filepath(&data->line_shader,     "../../../Jotunn/res/shaders/line_2d_shader.vert", "../../../Jotunn/res/shaders/line_2d_shader.frag");
   #ifdef DEBUG
      if (error) fprintf(stdout, "Error during line shader init\n");
   #endif

   renderable_2d_triangle_data_init(&data->triangle_data, max_vertices, &data->triangle_shader);
   renderable_2d_quad_data_init(&data->quad_data, max_vertices, &data->quad_shader);
   renderable_2d_circle_data_init(&data->circle_data, max_vertices, &data->circle_shader);
   renderable_2d_line_data_init(&data->line_data, max_vertices, &data->line_shader);

   data->line_width = 10.0f;
}

static void renderer_2d_data_cleanup(struct renderer_2d_data_t* data)
{
   renderable_2d_triangle_data_cleanup(&data->triangle_data);
   renderable_2d_quad_data_cleanup(&data->quad_data);
   renderable_2d_circle_data_cleanup(&data->circle_data);
   renderable_2d_line_data_cleanup(&data->line_data);

   shader_program_destroy(&data->triangle_shader);
   shader_program_destroy(&data->quad_shader);
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

   #ifdef DEBUG
      fprintf(stdout, "Initializing renderer_2d data\n");
   #endif
   // Batch stuff
   renderer_2d_data_init(&renderer_2d_data);

   #ifdef DEBUG
      fprintf(stdout, "Setting renderer_2d line width\n");
   #endif
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

void renderer_2d_draw_triangle(const struct renderer_2d_t* renderer, const fvector3 position, const fvector4* color)
{
   #ifdef DEBUG
      fprintf(stdout, "Triangle position\n");
      fvector3_print(&position);
      fprintf(stdout, "Triangle color\n");
      fvector4_print(color);
   #endif

   const unsigned int triangle_vertex_count = 3;

   if (renderer_2d_data.triangle_data.triangle_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   unsigned int i;
   for (i = 0; i < triangle_vertex_count; i++)
   {
      renderer_2d_data.triangle_data.vertex_data_ptr->position = fvector3_add(&triangle_2d_position_data[i], &position);;
      renderer_2d_data.triangle_data.vertex_data_ptr->color    = *color;

      renderer_2d_data.triangle_data.vertex_data_ptr++;
   }

   renderer_2d_data.triangle_data.triangle_index_count += 3;

}

void renderer_2d_draw_quad(const struct renderer_2d_t* renderer, const fvector3 position, const fvector4* color)
{
   #ifdef DEBUG
      fprintf(stdout, "Adding quad\n");
   #endif

   const unsigned int quad_vertex_count = 4;

   if (renderer_2d_data.quad_data.quad_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   unsigned int i;
   for (i = 0; i < quad_vertex_count; i++)
   {
      renderer_2d_data.quad_data.vertex_data_ptr->position = fvector3_add(&quad_2d_position_data[i], &position);
      renderer_2d_data.quad_data.vertex_data_ptr->color    = *color;
      
      renderer_2d_data.quad_data.vertex_data_ptr++;
   }

   renderer_2d_data.quad_data.quad_index_count += 6;
}

void renderer_2d_draw_circle(const struct renderer_2d_t* renderer, const fvector3 position, const fvector4* color)
{
   const unsigned int circle_vertex_count = 4;

   if (renderer_2d_data.circle_data.circle_index_count >= max_indices)
      renderer_2d_next_batch(renderer);

   unsigned int i;
   for (i = 0; i < circle_vertex_count; i++)
   {
      renderer_2d_data.circle_data.vertex_data_ptr->position = fvector3_add(&quad_2d_position_data[i], &position);
      renderer_2d_data.circle_data.vertex_data_ptr->color    = *color;
      
      renderer_2d_data.circle_data.vertex_data_ptr++;
   }

   renderer_2d_data.circle_data.circle_index_count += 6;
}

void renderer_2d_draw_line(const struct renderer_2d_t* renderer, const fvector3 pos_1, const fvector3 pos_2, const fvector4* color)
{
   renderer_2d_data.line_data.vertex_data_ptr->position = pos_1;
   renderer_2d_data.line_data.vertex_data_ptr->color    = *color;

   renderer_2d_data.line_data.vertex_data_ptr++;

   renderer_2d_data.line_data.vertex_data_ptr->position = pos_2;
   renderer_2d_data.line_data.vertex_data_ptr->color    = *color;

   renderer_2d_data.line_data.vertex_data_ptr++;

   renderer_2d_data.line_data.line_vertex_count += 2;
}

void renderer_2d_set_line_width(const struct renderer_2d_t* renderer, const float line_width)
{
   render_api_set_line_width(line_width);
}