#include "ui_layer.h"

#include <memory.h>

static void ui_container_event_handle(struct ui_container_t* ui_container, struct event_base_t* event);

static void ui_layer_event_handle(struct window_t* parent_window, struct window_layer_t* window_layer, struct event_base_t* event);
static void ui_layer_run(struct window_layer_t* ui_layer);

fvector4 ui_color_from_hex(uint32_t hex_code)
{
    uint8_t r_hex, g_hex, b_hex, a_hex;

    r_hex = (hex_code & 0xFF000000) >> 24;
    g_hex = (hex_code & 0x00FF0000) >> 16;
    b_hex = (hex_code & 0x0000FF00) >>  8;
    a_hex = (hex_code & 0x000000FF);

    fvector4 color = 
        (fvector4) 
        { 
            (r_hex / 255.0f), 
            (g_hex / 255.0f), 
            (b_hex / 255.0f), 
            (a_hex / 255.0f) 
        };

    return color;
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

#define UI_CONTAINER_MIN_WIDTH 20
#define UI_CONTAINER_MIN_HEIGHT 20

//
fvector2 ui_container_get_effective_size(struct ui_container_t* ui_container)
{
    fvector2 effective_size = 
        (fvector2)
        {
            .comp.x = (ui_container->width + 2*ui_container->padding),
            .comp.y = (ui_container->height + 2*ui_container->padding)
        };

    return effective_size;
}

void ui_container_init(struct ui_container_t* ui_container,
                       enum ui_container_layout_t layout)
{
    memset(ui_container, 0, sizeof(struct ui_container_t));

    ui_container->held = false;

    ui_container->layout = layout;

    ui_container->origin_x = 0.0f;
    ui_container->origin_y = 0.0f;

    ui_container->width  = UI_CONTAINER_MIN_WIDTH;
    ui_container->height = UI_CONTAINER_MIN_HEIGHT;

    ui_container->padding = 0;

    ui_container->num_elements = 0;

    ui_container->function_ui_container_event_react = ui_container_event_handle;

    ui_container->parent_layer = NULL;
}

void ui_container_set_origin(struct ui_container_t* ui_container,
                             float origin_x,
                             float origin_y)
{
    ui_container->origin_x = origin_x;
    ui_container->origin_y = origin_y;
}

void ui_container_set_padding(struct ui_container_t* ui_container,
                             uint16_t padding)
{
    ui_container->padding = padding;
}

void ui_container_render(struct renderer_2d_t* renderer_2d,
                         struct ui_container_t* ui_container,
                         struct ui_theme_t* theme)
{
    const fvector2 container_effective_size = ui_container_get_effective_size(ui_container);

    // Render backdrop box for container
    {
        fmatrix_4x4 transform_matrix;
        {
            fmatrix_4x4 scale_matrix, translation_matrix;

            const fvector3 scale_factors = (fvector3) 
                {
                    {   
                        container_effective_size.comp.x,
                        container_effective_size.comp.y,
                        1.0 
                    }
                };

            const fvector3 translation_vector = (fvector3) 
                { 
                    {   
                        ui_container->origin_x,
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
    }

    // Render unfilled box around contents (for debug, mostly)
    {
        fmatrix_4x4 transform_matrix;
        {
            fmatrix_4x4 scale_matrix, translation_matrix;

            const fvector3 scale_factors = (fvector3) 
                {
                    {   
                        ui_container->width,
                        ui_container->height,
                        1.0 
                    } 
                };

            const fvector3 translation_vector = (fvector3) 
                { 
                    {   
                        ui_container->origin_x + ui_container->padding, 
                        ui_container->origin_y + ui_container->padding,
                        0.0f
                    }
                };

            fmatrix_4x4_init(&scale_matrix);
            fmatrix_4x4_init(&translation_matrix);
            fmatrix_4x4_init(&transform_matrix);

            scale_matrix = fmatrix_4x4_transform_scale(&scale_matrix, scale_factors);
            translation_matrix = fmatrix_4x4_transform_translate(&translation_matrix, translation_vector);
            transform_matrix = fmatrix_4x4_multiply(&scale_matrix, &translation_matrix);
        }

        renderer_2d_draw_unfilled_quad(renderer_2d, &transform_matrix, theme->accent_color);
    }

    // Render elements in container, according to the container layout
    struct ui_element_t* element = ui_container->contained_elements[0].element;

    switch (ui_container->layout)
    {
        case UI_LAYOUT_VERTICAL:
        {
            fvector2 pen_position = { (ui_container->origin_x + ui_container->padding), 
                                      (ui_container->origin_y + ui_container->height + ui_container->padding) };

            for (int i = 0; i < ui_container->num_elements; i++)
            {
                element = ui_container->contained_elements[i].element;

                if(element->function_ui_element_render)
                {
                    element->function_ui_element_render(renderer_2d,
                                                        element, 
                                                        pen_position,
                                                        (fvector2) 
                                                        { 
                                                            .comp.x = (float)ui_container->width,
                                                            .comp.y = (float)ui_container->height
                                                        },
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
                                                        pen_position,
                                                        (fvector2) 
                                                        { 
                                                            .comp.x = ui_container->width,
                                                            .comp.y = ui_container->height
                                                        },
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

    if (position.comp.x > (ui_container->origin_x) &&
        position.comp.x < (ui_container->origin_x + ui_container->width + ui_container->padding) &&

        position.comp.y > (ui_container->origin_y) &&
        position.comp.y < (ui_container->origin_y + ui_container->height + ui_container->padding))
    {
        does_intersect = true;
    }
    
    return does_intersect;
}

static void ui_container_update_movement(struct ui_container_t* ui_container, const fvector2 cur_mouse, const fvector2 delta_mouse_instant)
{
    if (ui_container->held)
    {
        float new_furthest_left_x  = ((ui_container->origin_x) + delta_mouse_instant.comp.x);
        float new_furthest_right_x = ((ui_container->origin_x + (float)(ui_container->width + ui_container->padding)) + delta_mouse_instant.comp.x);

        float new_furthest_down_y = ((ui_container->origin_y) + delta_mouse_instant.comp.y);
        float new_furthest_up_y   = ((ui_container->origin_y + (float)(ui_container->height + 2*ui_container->padding)) + delta_mouse_instant.comp.y);

        float new_origin_x = (ui_container->origin_x + delta_mouse_instant.comp.x);
        float new_origin_y = (ui_container->origin_y + delta_mouse_instant.comp.y);

        ui_container->origin_x = (new_furthest_left_x < ui_container->parent_layer->ui_camera_ortho.left)   ? (0.0f) :
                                 (new_furthest_right_x > ui_container->parent_layer->ui_camera_ortho.right) ? (ui_container->parent_layer->ui_camera_ortho.right - (ui_container->width + ui_container->padding)) : 
                                 (new_origin_x);

        ui_container->origin_y = (new_furthest_down_y < ui_container->parent_layer->ui_camera_ortho.bottom) ? (0.0f) :
                                 (new_furthest_up_y > ui_container->parent_layer->ui_camera_ortho.top)      ? (ui_container->parent_layer->ui_camera_ortho.top - (ui_container->height + 2*ui_container->padding)) : 
                                 (new_origin_y);

        if (!point_intersects_ui_container(ui_container, cur_mouse))
        {
            ui_container->held = false;
        }
    }
}

//

static void ui_container_event_handle(struct ui_container_t* ui_container, struct event_base_t* event)
{
    static float delta_time = 0.0f;

    static fvector2 cur_mouse   = { 0.0f, 0.0f };
    static fvector2 delta_mouse = { 0.0f, 0.0f };

    int i = 0;
    while (!event->handled && i < ui_container->num_elements)
    {
        struct ui_element_t* element = (ui_container->contained_elements[i++].element);
        
        if (element->function_ui_element_event_react)
        {
            element->function_ui_element_event_react(element, event);
        }
    }

    if (!event->handled)
    {
        switch (event->event_type)
        {
            case EVENT_APP_TICK:
            {
                struct event_app_tick_t* app_tick_event = (struct event_app_tick_t*)event;

                delta_time = app_tick_event->delta_time_seconds;
            }
            break;

            case EVENT_MOUSE_MOVED:
            {
                struct event_mouse_moved_t* event_mouse_moved = (struct event_mouse_moved_t*)event;

                float mouse_x = event_mouse_moved->x;
                float mouse_y = event_mouse_moved->y;

                delta_mouse.comp.x = (mouse_x - cur_mouse.comp.x);
                delta_mouse.comp.y = (mouse_y - cur_mouse.comp.y);

                cur_mouse.comp.x = mouse_x;
                cur_mouse.comp.y = mouse_y;

                ui_container_update_movement(ui_container, cur_mouse, delta_mouse);
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
                            if (point_intersects_ui_container(ui_container, cur_mouse))
                            {
                                ui_container->held = true;
                            }
                        }
                        break;
                        // Released
                        case 0:
                        {
                            if (ui_container->held) 
                            {
                                ui_container->held = false;
                            }
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

    framebuffer_init(&ui_layer->ui_framebuffer, width, height);

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

    renderer_2d_init(&(ui_layer->ui_renderer_2d), &(ui_layer->ui_camera_ortho).base, parent_window, "ui_renderer_2d");

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
        ui_container->parent_layer = ui_layer;
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