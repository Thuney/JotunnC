#include "ui_layer.h"

#include <memory.h>

static void ui_container_event_handle(struct ui_container_t* ui_container, struct event_base_t* event);

static void ui_layer_event_handle(struct window_t* parent_window, struct window_layer_t* window_layer, struct event_base_t* event);
static void ui_layer_run(struct window_layer_t* ui_layer);

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

#define UI_CONTAINER_MIN_WIDTH 20
#define UI_CONTAINER_MIN_HEIGHT 20

//
void ui_container_init(struct ui_container_t* ui_container,
                       enum ui_container_layout_t layout,
                       uint16_t origin_x,
                       uint16_t origin_y)
{
    memset(ui_container, 0, sizeof(struct ui_container_t));

    ui_container->layout = layout;

    ui_container->origin_x = origin_x;
    ui_container->origin_y = origin_y;

    ui_container->width  = UI_CONTAINER_MIN_WIDTH;
    ui_container->height = UI_CONTAINER_MIN_HEIGHT;

    ui_container->padding = 5;

    ui_container->num_elements = 0;

    ui_container->function_ui_container_event_react = ui_container_event_handle;
}

void ui_container_render(struct renderer_2d_t* renderer_2d,
                         struct ui_container_t* ui_container,
                         struct ui_theme_t* theme)
{
    struct ui_element_t* element = ui_container->contained_elements[0].element;

    fmatrix_4x4 transform_matrix;
    {
        fmatrix_4x4 scale_matrix, translation_matrix;

        const fvector3 scale_factors = (fvector3) 
            { 
                {   ui_container->width  + 2*ui_container->padding, 
                    ui_container->height + 2*ui_container->padding + 2*element->padding_y,
                    1.0 
                } 
            };

        const fvector3 translation_vector = (fvector3) 
            { 
                {   ui_container->origin_x, 
                    ui_container->origin_y, 
                    -0.1f
                } 
            };

        fmatrix_4x4_init(&scale_matrix);
        fmatrix_4x4_init(&translation_matrix);
        fmatrix_4x4_init(&transform_matrix);

        scale_matrix = fmatrix_4x4_transform_scale(&scale_matrix, scale_factors);
        translation_matrix = fmatrix_4x4_transform_translate(&translation_matrix, translation_vector);
        transform_matrix = fmatrix_4x4_multiply(&scale_matrix, &translation_matrix);
    }

    renderer_2d_draw_quad(renderer_2d, &transform_matrix, theme->background_color);

    switch (ui_container->layout)
    {
        case UI_LAYOUT_VERTICAL:
        {
            fvector2 pen_position = { (ui_container->origin_x + ui_container->padding), 
                                      (ui_container->origin_y + ui_container->height + ui_container->padding + element->padding_y) };

            for (int i = 0; i < ui_container->num_elements; i++)
            {
                element = ui_container->contained_elements[i].element;

                if(element->function_ui_element_render)
                {
                    element->function_ui_element_render(renderer_2d,
                                                        element, 
                                                        pen_position.comp.x, 
                                                        pen_position.comp.y, 
                                                        theme);
                }

                pen_position.comp.y -= (element->height + 2*element->padding_y);
            }
        }
        break;
        case UI_LAYOUT_HORIZONTAL:
        {
            fvector2 pen_position = { (ui_container->origin_x + ui_container->padding), 
                                      (ui_container->origin_y + ui_container->height + ui_container->padding + element->padding_y) };

            for (int i = 0; i < ui_container->num_elements; i++)
            {
                element = ui_container->contained_elements[i].element;

                if(element->function_ui_element_render)
                {
                    element->function_ui_element_render(renderer_2d,
                                                        element, 
                                                        pen_position.comp.x, 
                                                        pen_position.comp.y, 
                                                        theme);
                }

                pen_position.comp.x += (element->width + 2*element->padding_x);
            }
        }
        break;
        case UI_LAYOUT_GRID:
        {

        }
        break;
    }
}

void ui_container_add_element(struct ui_container_t* ui_container,
                              struct ui_element_t* ui_element)
{
    if ( (ui_container->num_elements + 1) < MAX_UI_CONTAINER_ELEMENTS )
    {
        uint16_t index = ui_container->num_elements++;

        ui_container->contained_elements[index].element = ui_element;

        switch (ui_container->layout)
        {
            case UI_LAYOUT_VERTICAL:
            {
                uint16_t total_element_width = (ui_element->width + 2*ui_element->padding_x);

                if (total_element_width > ui_container->width) 
                    ui_container->width  = total_element_width;

                if (ui_container->num_elements == 1)
                {                    
                    ui_container->height = (ui_element->height + 2*ui_element->padding_y);
                }
                else
                {    
                    ui_container->height += (ui_element->height + 2*ui_element->padding_y);
                }

                ui_container->contained_elements[index].row = index;
            }
            break;

            case UI_LAYOUT_HORIZONTAL:
            {
                uint16_t total_element_height = (ui_element->height + 2*ui_element->padding_y);

                if (total_element_height > ui_container->height) 
                    ui_container->height  = total_element_height;

                if (ui_container->num_elements == 1)
                {                    
                    ui_container->width = (ui_element->width + 2*ui_element->padding_x);
                }
                else
                {    
                    ui_container->width += (ui_element->width + 2*ui_element->padding_x);
                }

                ui_container->contained_elements[index].column = index;
            }
            break;

            case UI_LAYOUT_GRID:
            {
                ui_container->contained_elements[index].row    = (index / 4);
                ui_container->contained_elements[index].column = (index % 4);
            }
            break;
        }
    }
}

static bool point_intersects_ui_container(struct ui_container_t* ui_container, const fvector2 position)
{
    bool does_intersect = false;

    if (position.comp.x > ui_container->origin_x &&
        position.comp.x < (ui_container->origin_x + ui_container->width + ui_container->padding) &&

        position.comp.y < ui_container->origin_y &&
        position.comp.y > (ui_container->origin_y - ui_container->height - ui_container->padding))
    {
        does_intersect = true;       
    }   
    
    return does_intersect;
}

//

static void ui_container_event_handle(struct ui_container_t* ui_container, struct event_base_t* event)
{
    static fvector2 cur_mouse = { 0.0f, 0.0f };

    switch (event->event_type)
    {
        case EVENT_APP_TICK:
        {
            struct event_app_tick_t* app_tick_event = (struct event_app_tick_t*)event;

        }
        break;

        case EVENT_MOUSE_MOVED:
        {
            struct event_mouse_moved_t* event_mouse_moved = (struct event_mouse_moved_t*)event;

            float mouse_x = event_mouse_moved->x;
            float mouse_y = event_mouse_moved->y;

            cur_mouse.comp.x = mouse_x;
            cur_mouse.comp.y = mouse_y;
        }
        break;

        case EVENT_MOUSE_BUTTON:
        {
            struct event_mouse_button_t* event_mouse_button = (struct event_mouse_button_t*)event;

            if (event_mouse_button->button == 0)
            {
                switch(event_mouse_button->action)
                {
                    // Pressed
                    case 1:
                    {
                        if (point_intersects_ui_container(ui_container, cur_mouse ))
                        {
                            fprintf(stdout, "Mouse clicked UI container\n");
                        }
                    }
                    break;
                    // Released
                    case 0:
                    {

                    }
                    break;
                }
            }
        }
        break;

        default:
        {

        }
        break;
    }

    if (!event->handled)
    {
        for (int j = 0; j < ui_container->num_elements; j++)
        {
            struct ui_element_t* element = (ui_container->contained_elements[j].element);
            
            if (element->function_ui_element_event_react)
            {
                element->function_ui_element_event_react(element, event);
            }
        }
    }
}

//
void ui_layer_init(struct window_t* parent_window, 
                   struct ui_layer_t* ui_layer, 
                   uint32_t width, 
                   uint32_t height, 
                   struct ui_theme_t theme)
{
    memset(ui_layer, 0, sizeof(struct ui_layer_t));

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
    window_layer_set_event_react(&(ui_layer->ui_window_layer), ui_layer_event_handle);

    ui_layer->ui_theme = theme;
}

void ui_layer_cleanup(struct ui_layer_t* ui_layer)
{
    framebuffer_cleanup(&(ui_layer->ui_framebuffer));
}

void ui_layer_add_container(struct ui_layer_t* ui_layer,
                            struct ui_container_t* ui_container)
{
    if ((ui_layer->num_containers + 1) < MAX_UI_CONTAINERS)
    {
        ui_layer->ui_containers[ui_layer->num_containers++] = ui_container;
    }
}

//

static void ui_layer_event_handle(struct window_t* parent_window, struct window_layer_t* window_layer, struct event_base_t* event)
{
    struct ui_layer_t* ui_layer = (struct ui_layer_t*)window_layer;

    for (int i = 0; i < ui_layer->num_containers; i++)
    {
        struct ui_container_t* container = ui_layer->ui_containers[i];

        container->function_ui_container_event_react(container, event);
    }
}

static void ui_layer_run(struct window_layer_t* window_layer)
{
    struct ui_layer_t* ui_layer = (struct ui_layer_t*)window_layer;

    for (int i = 0; i < ui_layer->num_containers; i++)
    {
        struct ui_container_t* container = ui_layer->ui_containers[i];

        ui_container_render(&(ui_layer->ui_renderer_2d), container, &(ui_layer->ui_theme));
    }
}