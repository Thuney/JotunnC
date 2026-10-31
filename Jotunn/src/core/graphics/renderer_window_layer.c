#include "renderer_window_layer.h"
#include "log.h"

#include "util.h"

#include "../diagnostics/diagnostics_internal.h"
#include "window_layer.h"

#ifdef DEBUG
  #include <stdio.h>
#endif

#define NUM_FRAMEBUFFER_VERTICES ((3 + 2) * 6)

// Vertex attributes for a quad that fills the entire screen
//  (in Normalized Device Coordinates)
static float framebuffer_vertices[] =
{
  -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

  -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
   1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
   1.0f, -1.0f, 0.0f, 1.0f, 0.0f
};

static unsigned int framebuffer_indices[] = {0, 1, 2, 3, 4, 5};

struct renderer_window_layer_vertex_t
{
  union fvector3_t position;
  union fvector2_t texture_coordinate;
};

enum renderer_window_layer_vertex_attribute_t
{
  RENDERER_WINDOW_LAYER_VERTEX_ATTRIBUTE_POSITION = 0x00,
  RENDERER_WINDOW_LAYER_VERTEX_ATTRIBUTE_TEXTURE_COORDINATES,
  RENDERER_WINDOW_LAYER_VERTEX_ATTRIBUTE_COUNT
};

// Framebuffer vertex attributes
static struct vertex_attribute_t framebuffer_vertex_attributes[4] =
{
  [0] =
  {
    .attribute_name = "position",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 3,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderer_window_layer_vertex_t),
    .ptr_offset_to_attrib = 0
  },
  [1] =
  {
    .attribute_name = "texCoord",
    .index = (unsigned int)-1, // To be filled by shader program
    .size = 2,
    .data_type = VERTEX_ATTRIBUTE_DATA_TYPE_FLOAT,
    .should_normalize = 0,
    .stride = sizeof(struct renderer_window_layer_vertex_t),
    .ptr_offset_to_attrib = (void*)sizeof(union fvector3_t)
  }
};

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Creates the quad's vertex array and buffers and loads the framebuffer
//  shader, looking up each attribute's location in it and setting the
//  attribute on the vertex array
//
//  in/out: inout_attributes
//  out:    out_data
static void renderer_window_layer_data_init(
  struct renderer_window_layer_data_t* out_data,
  struct vertex_attribute_t* inout_attributes);

// Destroys the shader, vertex array and buffers
//
//  in/out: inout_data
static void renderer_window_layer_data_cleanup(
  struct renderer_window_layer_data_t* inout_data);

//

/*******************************************************************************
 *
 *   Function: renderer_window_layer_init()
 *
 ******************************************************************************/
void renderer_window_layer_init(
  struct renderer_window_layer_t* out_renderer,
  struct window_t* in_parent_window,
  const char* in_tag)
{
  const uint16_t render_flags = (RENDER_API_RENDER_FLAGS_COLOR | RENDER_API_RENDER_FLAGS_BLEND);

  camera_unprojected_init(
    &(out_renderer->camera_unprojected));

  renderer_base_init(
    &(out_renderer->base),
    render_flags,
    &(out_renderer->camera_unprojected),
    in_parent_window,
    in_tag);

  // Layers are drawn into framebuffers cleared to transparent black, which
  //  leaves their contents premultiplied by alpha
  render_api_set_blend_mode(
    &(out_renderer->base.render_api),
    RENDER_API_BLEND_MODE_PREMULTIPLIED_ALPHA);

  out_renderer->base.renderer_begin_scene = &(renderer_window_layer_begin_scene);
  out_renderer->base.renderer_end_scene   = &(renderer_window_layer_end_scene);

#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Initializing renderer_window_layer");
#endif

  renderer_window_layer_data_init(
    &(out_renderer->render_data),
    framebuffer_vertex_attributes);
}

/*******************************************************************************
 *
 *   Function: renderer_window_layer_cleanup()
 *
 ******************************************************************************/
void renderer_window_layer_cleanup(
  struct renderer_window_layer_t* inout_renderer)
{
  vertex_array_unbind();

  renderer_window_layer_data_cleanup(&(inout_renderer->render_data));

  renderer_base_cleanup(&(inout_renderer->base));
}

/*******************************************************************************
 *
 *   Function: renderer_window_layer_begin_scene()
 *
 ******************************************************************************/
void renderer_window_layer_begin_scene(
  struct renderer_base_t* in_renderer)
{
  struct renderer_window_layer_t* renderer_window_layer =
    container_of(
      in_renderer,
      struct renderer_window_layer_t,
      base);

  // Called directly rather than through renderer_base_begin_scene(), so the
  //  scene is marked here
  diagnostics_internal_scene_begin(&(renderer_window_layer->base));

  framebuffer_unbind();

  render_api_bind(&(renderer_window_layer->base.render_api));
  render_api_clear(&(renderer_window_layer->base.render_api));
}

/*******************************************************************************
 *
 *   Function: renderer_window_layer_end_scene()
 *
 ******************************************************************************/
void renderer_window_layer_end_scene(
  struct renderer_base_t* in_renderer)
{
  (void)in_renderer;

  vertex_array_unbind();

  framebuffer_unbind();

  diagnostics_internal_scene_end();
}

/*******************************************************************************
 *
 *   Function: renderer_window_layer_draw()
 *
 ******************************************************************************/
void renderer_window_layer_draw(
  struct renderer_window_layer_t* inout_renderer,
  const struct window_layer_t* in_window_layer,
  uint8_t in_layer_number)
{
  // shader_program_use(&(renderer->render_data.framebuffer_shader));

  struct renderer_window_layer_data_t* data = &(inout_renderer->render_data);

  vertex_array_bind(&(data->vao));
  vertex_buffer_bind(&(data->vbo));
  element_buffer_bind(&(data->ebo));

  // Fill vertex buffer
  element_buffer_buffer_data(
    &(data->ebo),
    framebuffer_indices,
    sizeof(framebuffer_indices),
    VERTEX_BUFFER_DATA_USAGE_STATIC_DRAW);
  vertex_buffer_set_data(
    &(data->vbo),
    framebuffer_vertices,
    sizeof(framebuffer_vertices),
    VERTEX_BUFFER_DATA_USAGE_STATIC_READ);

  shader_program_use(&(data->framebuffer_shader));

  texture_2d_bind(
    &(in_window_layer->framebuffer->color_buffer.texture),
    0);

  shader_program_set_uniform_int(
    &(data->framebuffer_shader),
    "u_textures[0]",
    0);

  render_api_draw_elements(
    RENDER_API_DRAW_MODE_TRIANGLES,
    6,
    RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_INT,
    0);
}

/*******************************************************************************
 *
 *   Function: renderer_window_layer_data_init()
 *
 ******************************************************************************/
static void renderer_window_layer_data_init(
  struct renderer_window_layer_data_t* out_data,
  struct vertex_attribute_t* inout_attributes)
{
  vertex_array_init(
    &(out_data->vao),
    1);
  vertex_buffer_init(
    &(out_data->vbo),
    1);
  element_buffer_init(
    &(out_data->ebo),
    1);

  vertex_array_bind(&(out_data->vao));
  vertex_buffer_bind(&(out_data->vbo));
  element_buffer_bind(&(out_data->ebo));

  shader_program_init_filepath(
    &(out_data->framebuffer_shader),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/framebuffer_shader.vert"),
    PATH_FROM_JOTUNN_ROOT("/res/shaders/framebuffer_shader.frag"));

  unsigned int attribute_index, temp_shader_index;
  const char* attribute_name;

  for (attribute_index = 0; attribute_index < RENDERER_WINDOW_LAYER_VERTEX_ATTRIBUTE_COUNT;
       attribute_index++)
  {
    attribute_name = inout_attributes[attribute_index].attribute_name;

    inout_attributes[attribute_index].index =
      shader_program_get_attribute_location(
        &(out_data->framebuffer_shader),
        attribute_name);

    vertex_array_set_attribute(
      &(out_data->vao),
      &(inout_attributes[attribute_index]));
  }
}

/*******************************************************************************
 *
 *   Function: renderer_window_layer_data_cleanup()
 *
 ******************************************************************************/
static void renderer_window_layer_data_cleanup(
  struct renderer_window_layer_data_t* inout_data)
{
  shader_program_destroy(&(inout_data->framebuffer_shader));

  vertex_array_destroy(
    &(inout_data->vao),
    1);
  vertex_buffer_destroy(
    &(inout_data->vbo),
    1);
  element_buffer_destroy(
    &(inout_data->ebo),
    1);
}
