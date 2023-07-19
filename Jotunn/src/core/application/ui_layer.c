#include "ui_layer.h"

static void ui_layer_run(struct window_layer_t* ui_layer);

//
void ui_element_init(struct ui_element_t* ui_element,
                     enum ui_element_type_t element_type,
                     uint16_t width, 
                     uint16_t height,
                     void (*function_ui_element_render)(struct ui_element_t* ui_element,
                                                        uint16_t origin_x,
                                                        uint16_t origin_y) )
{
    ui_element->element_type = element_type;

    ui_element->width        = width;
    ui_element->height       = height;

    ui_element->function_ui_element_render = function_ui_element_render;
}

//
void ui_theme_init(struct ui_theme_t* ui_theme,
                   fvector4 accent_color,
                   fvector4 foreground_color,
                   fvector4 background_color)
{
    ui_theme->accent_color     = accent_color;
    ui_theme->foreground_color = foreground_color;
    ui_theme->background_color = background_color;
}

//
void ui_container_init(struct ui_container_t* ui_container,
                       enum ui_container_layout_t layout,
                       uint16_t origin_x, 
                       uint16_t origin_y,
                       uint16_t width,
                       uint16_t height)
{
    ui_container->layout = layout;

    ui_container->origin_x = origin_x;
    ui_container->origin_y = origin_y;

    ui_container->width  = width;
    ui_container->height = height;

    ui_container->num_elements = 0;
}

void ui_container_render(struct ui_container_t* ui_container)
{
    for (int i = 0; i < ui_container->num_elements; i++)
    {
        struct ui_element_t* element = (ui_container->contained_elements + i);

        if(element->function_ui_element_render)
        {
            element->function_ui_element_render(element, 0, 0);
        }
    }
}

void ui_container_add_element(struct ui_container_t* ui_container,
                              struct ui_element_t* ui_element)
{
    if ( (ui_container->num_elements + 1) < MAX_UI_CONTAINER_ELEMENTS )
    {
        ui_container->contained_elements[ui_container->num_elements].element = ui_element;

        switch (ui_container->layout)
        {
            case UI_LAYOUT_VERTICAL:
            {
                ui_container->contained_elements[ui_container->num_elements].row = ui_container->num_elements;
            }
            break;

            case UI_LAYOUT_HORIZONTAL:
            {
                ui_container->contained_elements[ui_container->num_elements].column = ui_container->num_elements;
            }
            break;

            case UI_LAYOUT_GRID:
            {
                ui_container->contained_elements[ui_container->num_elements].row    = (ui_container->num_elements / 4);
                ui_container->contained_elements[ui_container->num_elements].column = (ui_container->num_elements % 4);
            }
            break;
        }

        ui_container->num_elements++;
    }
}

//
void ui_layer_init(struct window_t* parent_window, struct ui_layer_t* ui_layer, uint32_t width, uint32_t height)
{
    // Camera stuff
    const fvector3 camera_position = (fvector3) { {0.0f, 0.0f,  2.0f} };
    const fvector3 camera_up       = (fvector3) { {0.0f, 1.0f,  0.0f} };
    const fvector3 camera_front    = (fvector3) { {0.0f, 0.0f, -1.0f} };

    const float ortho_left   = 0.0f; 
    const float ortho_right  = (float)width;
    const float ortho_top    = (float)height;
    const float ortho_bottom = 0.0f;
    const float ortho_near_plane = -3.0f;
    const float ortho_far_plane  = 100.0f;

    camera_init_orthographic(&(ui_layer->ui_camera_ortho), camera_position, camera_up, camera_front);
    camera_set_projection_orthographic(&(ui_layer->ui_camera_ortho), ortho_left, ortho_right, ortho_top, ortho_bottom, ortho_near_plane, ortho_far_plane);

    renderer_2d_init(&(ui_layer->ui_renderer_2d), &(ui_layer->ui_camera_ortho).base, parent_window, "ball_renderer_2d");

    framebuffer_init(&ui_layer->ui_framebuffer, width, height);
    window_layer_init(parent_window, &ui_layer->ui_window_layer, &ui_layer->ui_framebuffer, &ui_layer->ui_camera_ortho.base, &ui_layer->ui_renderer_2d.base);

    window_layer_set_custom_layer_run(&(ui_layer->ui_window_layer), ui_layer_run);
}

void ui_layer_cleanup(struct ui_layer_t* ui_layer)
{
    framebuffer_cleanup(&(ui_layer->ui_framebuffer));
}

void ui_layer_add_container(struct ui_layer_t* ui_layer,
                            struct ui_container_t* ui_container)
{
    
}

static void ui_layer_run(struct window_layer_t* window_layer)
{
    struct ui_layer_t* ui_layer = (struct ui_layer_t*)window_layer;

    for (int i = 0; i < ui_layer->num_containers; i++)
    {
        struct ui_container_t* container = (ui_layer->ui_containers + i);

        ui_container_render(container);
    }
}