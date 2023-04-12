#include "renderer_window_layer.h"

#ifdef DEBUG
    #include <stdio.h>
#endif

static void renderer_window_layer_data_init(struct renderer_window_layer_data_t *data)
{
   unsigned int error = 0;

   error = shader_program_init_filepath(&data->framebuffer_shader, "/home/loki/Repos/JotunnC/Jotunn/res/shaders/framebuffer_shader.vert", "/home/loki/Repos/JotunnC/Jotunn/res/shaders/framebuffer_shader.frag");
#ifdef DEBUG
   if (error)
      fprintf(stdout, "Error during framebuffer shader init\n");
#endif
}

static void renderer_window_layer_data_cleanup(struct renderer_window_layer_data_t *data)
{
   shader_program_destroy(&data->framebuffer_shader);
}

//

void renderer_window_layer_init(struct renderer_window_layer_t* renderer, struct camera_base_t* camera, struct window_t* parent_window, const char* tag)
{
    const uint16_t render_flags = (COLOR_FLAG | BLEND_FLAG);

    renderer_base_init(&renderer->base, render_flags, camera, parent_window, tag);

    renderer->base.renderer_begin_scene = &renderer_window_layer_begin_scene;
    renderer->base.renderer_end_scene = &renderer_window_layer_end_scene;

#ifdef DEBUG
    fprintf(stdout, "Initializing renderer_window_layer\n");
#endif

    // Batch stuff
    renderer_window_layer_data_init(&renderer->render_data);
}

void renderer_window_layer_cleanup(struct renderer_window_layer_t *renderer)
{
    vertex_array_unbind();

    renderer_window_layer_data_cleanup(&renderer->render_data);
}

void renderer_window_layer_begin_scene(void *renderer)
{
    struct renderer_window_layer_t *renderer_window_layer = (struct renderer_window_layer_t *)renderer;


}

void renderer_window_layer_end_scene(void *renderer)
{
    struct renderer_window_layer_t *renderer_window_layer = (struct renderer_window_layer_t *)renderer;

    
   vertex_array_unbind();

   framebuffer_unbind();
}

void renderer_window_layer_draw_framebuffer(struct renderer_window_layer_t* renderer, struct framebuffer_t* framebuffer)
{

    
}
