#include "renderer_window_layer.h"

#include "util.h"

#ifdef DEBUG
    #include <stdio.h>
#endif

#define NUM_FRAMEBUFFER_VERTICES ((3+2)*6)

static float framebuffer_vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions         // texCoords
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f
};

static unsigned int framebuffer_indices[] = {
    0, 1, 2,
    3, 4, 5
};

struct framebuffer_vertex_t
{
   fvector3 position;
   fvector2 texture_coordinate;
};

enum framebuffer_vertex_attribute_t
{
   FRAMEBUFFER_VERTEX_POSITION = 0x00,
   FRAMEBUFFER_VERTEX_TEXTURE_COORDINATES,
   _FRAMEBUFFER_VERTEX_ATTRIBUTE_NUM
};

// Framebuffer vertex attributes
static struct vertex_attribute_t framebuffer_vertex_attributes[4] = {
   [0] = 
   {
      .attribute_name       = "position",
      .index                = (unsigned int)-1, // To be filled by shader program
      .size                 = 3,
      .data_type            = V_FLOAT,
      .should_normalize     = 0,
      .stride               = sizeof(struct framebuffer_vertex_t),
      .ptr_offset_to_attrib = 0
   },
   [1] = 
   {
      .attribute_name       = "texCoord",
      .index                = (unsigned int)-1, // To be filled by shader program
      .size                 = 2,
      .data_type            = V_FLOAT,
      .should_normalize     = 0,
      .stride               = sizeof(struct framebuffer_vertex_t),
      .ptr_offset_to_attrib = (void*)sizeof(fvector3)
   }
};

static void renderer_window_layer_data_init(struct renderer_window_layer_data_t *data, struct vertex_attribute_t* attributes)
{
    unsigned int error = 0;

    vertex_array_init(&(data->vao), 1);
    vertex_buffer_init(&(data->vbo), 1);
    element_buffer_init(&(data->ebo), 1);

    vertex_array_bind(&(data->vao));
    vertex_buffer_bind(&(data->vbo));
    element_buffer_bind(&(data->ebo));

    error = shader_program_init_filepath(&data->framebuffer_shader, PATH_FROM_JOTUNN_ROOT("/res/shaders/framebuffer_shader.vert"), PATH_FROM_JOTUNN_ROOT("/res/shaders/framebuffer_shader.frag"));
#ifdef DEBUG
    if (error)
        fprintf(stdout, "Error during framebuffer shader init\n");
#endif

    unsigned int attribute_index, temp_shader_index;
    char* attribute_name;

    for (attribute_index = 0; attribute_index < _FRAMEBUFFER_VERTEX_ATTRIBUTE_NUM; attribute_index++)
    {
        attribute_name = attributes[attribute_index].attribute_name;
        temp_shader_index = shader_program_get_attribute_location(&(data->framebuffer_shader), attribute_name);

        #ifdef DEBUG
           fprintf(stdout, "Setting shader attribute at index %u with name %s\n", temp_shader_index, attribute_name);
        #endif

        attributes[attribute_index].index = temp_shader_index;

        #ifdef DEBUG
           vertex_attribute_print(&attributes[attribute_index]);
        #endif

        vertex_array_set_attribute(&(data->vao), &attributes[attribute_index]);
    }
    
}

static void renderer_window_layer_data_cleanup(struct renderer_window_layer_data_t *data)
{
    shader_program_destroy(&data->framebuffer_shader);
}

//

void renderer_window_layer_init(struct renderer_window_layer_t *renderer, struct window_t *parent_window, const char *tag)
{
    const uint16_t render_flags = (COLOR_FLAG | BLEND_FLAG);

    camera_init_unprojected(&(renderer->camera_unprojected));

    renderer_base_init(&(renderer->base), render_flags, &(renderer->camera_unprojected), parent_window, tag);

    renderer->base.renderer_begin_scene = &(renderer_window_layer_begin_scene);
    renderer->base.renderer_end_scene = &(renderer_window_layer_end_scene);

#ifdef DEBUG
    fprintf(stdout, "Initializing renderer_window_layer\n");
#endif

    renderer_window_layer_data_init(&(renderer->render_data), framebuffer_vertex_attributes);
}

void renderer_window_layer_cleanup(struct renderer_window_layer_t *renderer)
{
    vertex_array_unbind();

    renderer_window_layer_data_cleanup(&(renderer->render_data));
}

void renderer_window_layer_begin_scene(void *renderer)
{
    struct renderer_window_layer_t *renderer_window_layer = (struct renderer_window_layer_t *)renderer;

    framebuffer_unbind();

    render_api_bind(&(renderer_window_layer->base.render_api));
    render_api_clear(&(renderer_window_layer->base.render_api));
}

void renderer_window_layer_end_scene(void *renderer)
{
    struct renderer_window_layer_t *renderer_window_layer = (struct renderer_window_layer_t *)renderer;

    vertex_array_unbind();

    framebuffer_unbind();
}

void renderer_window_layer_draw_layer(struct renderer_window_layer_t *renderer, struct window_layer_t *window_layer, uint8_t layer_number)
{
    // shader_program_use(&(renderer->render_data.framebuffer_shader));

    struct renderer_window_layer_data_t *data = &(renderer->render_data);

    vertex_array_bind(&data->vao);
    vertex_buffer_bind(&data->vbo);
    element_buffer_bind(&data->ebo);

    // Fill vertex buffer
    element_buffer_buffer_data(&data->ebo, framebuffer_indices, sizeof(framebuffer_indices), STATIC_DRAW);
    vertex_buffer_buffer_data(&data->vbo, framebuffer_vertices, sizeof(framebuffer_vertices), STATIC_READ);

    shader_program_use(&data->framebuffer_shader);

    texture_2d_bind(&(window_layer->framebuffer->color_buffer.texture), 0);

    shader_program_set_uniform_int(&data->framebuffer_shader, "u_textures[0]", 0);

    render_api_draw_elements(DRAW_TYPE_TRIANGLES, 6, ELEMENT_UNSIGNED_INT, 0);
}
