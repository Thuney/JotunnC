#include "renderer_window_layer.h"

#include <memory.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#define NUM_FRAMEBUFFER_VERTICES ((3+2)*6)

// static float framebuffer_vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
//         // positions   // texCoords
//         -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
//         -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
//          1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

//         -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
//          1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
//          1.0f,  1.0f, 0.0f,  1.0f, 1.0f
//     };

static void get_framebuffer_vertices_for_layer(float vertices[NUM_FRAMEBUFFER_VERTICES], uint8_t layer)
{
    float framebuffer_vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions                 // texCoords
        -1.0f,  1.0f, (float)layer,  0.0f, 1.0f,
        -1.0f, -1.0f, (float)layer,  0.0f, 0.0f,
         1.0f, -1.0f, (float)layer,  1.0f, 0.0f,
        -1.0f,  1.0f, (float)layer,  0.0f, 1.0f,
         1.0f, -1.0f, (float)layer,  1.0f, 0.0f,
         1.0f,  1.0f, (float)layer,  1.0f, 1.0f
    };

    memcpy(vertices, framebuffer_vertices, NUM_FRAMEBUFFER_VERTICES);
}

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

    error = shader_program_init_filepath(&data->framebuffer_shader, "/home/loki/Repos/JotunnC/Jotunn/res/shaders/framebuffer_shader.vert", "/home/loki/Repos/JotunnC/Jotunn/res/shaders/framebuffer_shader.frag");
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

        // #ifdef DEBUG
        //    fprintf(stdout, "Setting shader attribute at index %u with name %s\n", temp_shader_index, attribute_name);
        // #endif

        attributes[attribute_index].index = temp_shader_index;

        // #ifdef DEBUG
        //    vertex_attribute_print(&attributes[attribute_index]);
        // #endif

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
}

void renderer_window_layer_end_scene(void *renderer)
{
    struct renderer_window_layer_t *renderer_window_layer = (struct renderer_window_layer_t *)renderer;

    vertex_array_unbind();

    framebuffer_unbind();
}

void renderer_window_layer_draw_layer(struct renderer_window_layer_t *renderer, struct window_layer_t *window_layer, uint8_t layer_number)
{
    shader_program_use(&(renderer->render_data.framebuffer_shader));

    struct renderer_window_layer_data_t *data = &(renderer->render_data);

    vertex_array_bind(&data->vao);
    vertex_buffer_bind(&data->vbo);
    element_buffer_bind(&data->ebo);

    static float layer_framebuffer_vertices[NUM_FRAMEBUFFER_VERTICES];

    get_framebuffer_vertices_for_layer(layer_framebuffer_vertices, layer_number);

    static const unsigned int texture_data_size = (unsigned int)(NUM_FRAMEBUFFER_VERTICES*sizeof(float));

    // Fill vertex buffer
    vertex_buffer_buffer_data(&data->vbo, layer_framebuffer_vertices, texture_data_size, STATIC_READ);

    shader_program_use(&data->framebuffer_shader);

    texture_2d_bind(&(window_layer->framebuffer->color_buffer.texture), 0);
    shader_program_set_uniform_int(&data->framebuffer_shader, "u_textures[0]", 0);

    render_api_draw_elements(DRAW_TYPE_TRIANGLES, 6, ELEMENT_UNSIGNED_INT, 0);
}
