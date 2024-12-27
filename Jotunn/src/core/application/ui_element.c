#include "ui_element.h"

#include "renderer_2d.h"
#include "string.h"
#include "ui_layer.h"

//
fvector2 ui_element_get_effective_size(struct ui_element_t* ui_element)
{
    fvector2 effective_size = 
        (fvector2)
        {
            .comp.x = (ui_element->width  + 2*ui_element->padding_x),
            .comp.y = (ui_element->height + 2*ui_element->padding_y)
        };

    return effective_size;
}

// ------------------------------------------------------------------------------------------------------------

void ui_element_init(struct ui_element_t* ui_element,
                     enum ui_element_type_t element_type,
                     uint16_t width, 
                     uint16_t height,
                     void (*function_ui_element_render)(struct renderer_2d_t* renderer_2d,
                                                        struct ui_element_t* ui_element,
                                                        fvector2 origin_position,
                                                        fvector2 allocated_space,
                                                        struct ui_theme_t* ui_theme),
                     void (*function_ui_element_event_react)(struct ui_element_t* ui_element, 
                                                             struct event_base_t* event) )
{
    ui_element->element_type = element_type;

    ui_element->element_centering = UI_ELEMENT_ALIGN_LEFT;

    ui_element->width   = width;
    ui_element->height  = height;

    ui_element->padding_x = 0;
    ui_element->padding_y = 0;

    if (function_ui_element_render)
        ui_element->function_ui_element_render = function_ui_element_render;
    
    if (function_ui_element_event_react) 
        ui_element->function_ui_element_event_react = function_ui_element_event_react;
}

// ------------------------------------------------------------------------------------------------------------

static void ui_element_static_text_render(struct renderer_2d_t* renderer_2d,
                                          struct ui_element_t* ui_element,
                                          fvector2 origin_position,
                                          fvector2 allocated_space,
                                          struct ui_theme_t* theme)
{
    struct ui_element_static_text_t* static_text_element = (struct ui_element_static_text_t*)ui_element;

    fvector2 effective_element_size = ui_element_get_effective_size(ui_element);

    switch (ui_element->element_centering)
    {
        case UI_ELEMENT_ALIGN_LEFT:
        {
            renderer_2d_draw_string(
                renderer_2d, 
                static_text_element->typeface, 
                (fvector3) 
                    { 
                        origin_position.comp.x + ui_element->padding_x,
                        origin_position.comp.y - ui_element->padding_y,
                        0.1f 
                    }, 
                static_text_element->static_text);
        }
        break;
        case UI_ELEMENT_ALIGN_RIGHT:
        {
            renderer_2d_draw_string(
                renderer_2d, 
                static_text_element->typeface, 
                (fvector3) 
                    { 
                        origin_position.comp.x + ui_element->padding_x + allocated_space.comp.x - effective_element_size.comp.x,
                        origin_position.comp.y - ui_element->padding_y,
                        0.1f 
                    },
                static_text_element->static_text);
        }
        break;
        case UI_ELEMENT_ALIGN_CENTER:
        {
            renderer_2d_draw_string(
                renderer_2d, 
                static_text_element->typeface, 
                (fvector3) 
                    {
                        origin_position.comp.x + ui_element->padding_x + 0.5f*(allocated_space.comp.x - effective_element_size.comp.x),
                        origin_position.comp.y -  ui_element->padding_y,
                        0.1f
                    }, 
                static_text_element->static_text);
        }
        break;
    }
}

void ui_element_static_text_init(struct ui_element_static_text_t* text_element,
                                 char* static_text,
                                 struct typeface_t* typeface)
{
    int text_length = strlen(static_text);
    fvector2 text_dimensions = typeface_calculate_string_dimensions(typeface, static_text, text_length);

    ui_element_init(&(text_element->base_element),
                    UI_ELEMENT_STATIC_TEXT,
                    text_dimensions.comp.x,
                    text_dimensions.comp.y,
                    ui_element_static_text_render,
                    (void*)0);

    text_element->static_text = static_text;
    text_element->typeface    = typeface;

    text_element->base_element.padding_x = 3;
    text_element->base_element.padding_y = 8;
}

// ------------------------------------------------------------------------------------------------------------

static fmatrix_4x4 get_transform(const fvector3 scale_factors, 
                                 const fvector3 translation_vector)
{
    fmatrix_4x4 transform_matrix;
    {
        fmatrix_4x4 scale_matrix, translation_matrix;

        fmatrix_4x4_init(&scale_matrix);
        fmatrix_4x4_init(&translation_matrix);
        fmatrix_4x4_init(&transform_matrix);

        scale_matrix = fmatrix_4x4_transform_scale(&scale_matrix, scale_factors);
        translation_matrix = fmatrix_4x4_transform_translate(&translation_matrix, translation_vector);
        transform_matrix = fmatrix_4x4_multiply(&scale_matrix, &translation_matrix);
    }
    return transform_matrix;
}

// static bool point_intersects_ui_element(struct ui_element_t* ui_element, const fvector2 position)
// {
//     bool does_intersect = false;

//     if (position.comp.x > (ui_element->origin_x) &&
//         position.comp.x < (ui_element->origin_x + ui_element->width + ui_element->padding) &&

//         position.comp.y > (ui_element->origin_y) &&
//         position.comp.y < (ui_element->origin_y + ui_element->height + ui_element->padding))
//     {
//         does_intersect = true;
//     }
    
//     return does_intersect;
// }

static void ui_element_slider_event_react(struct ui_element_t* ui_element, 
                                   struct event_base_t* event)
{
    struct ui_element_slider_t* slider_element = (struct ui_element_slider_t*)ui_element;

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
                        event->handled = true;
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
}

static void ui_element_slider_render(struct renderer_2d_t* renderer_2d,
                                     struct ui_element_t* ui_element,
                                     fvector2 origin_position,
                                     fvector2 allocated_space,
                                     struct ui_theme_t* theme)
{
    struct ui_element_slider_t* slider_element = (struct ui_element_slider_t*)ui_element;

    float slider_range = (slider_element->upper_bound - slider_element->lower_bound);
    float slider_percentage = ((slider_element->slider_position - slider_element->lower_bound) / slider_range);

    const static float slider_size_px = 20.0f;

    // Render backing box
    const fvector3 box_scale_factors = 
        (fvector3) 
        { 
            ui_element->width, 
            ui_element->height, 
            1.0f 
        };

    const fvector3 box_translation_vector = 
        (fvector3) 
        { 
            origin_position.comp.x + ui_element->padding_x, 
            origin_position.comp.y - (ui_element->height + ui_element->padding_y), 
            0.1f 
        };

    fmatrix_4x4 box_transform = get_transform(box_scale_factors, box_translation_vector);

    renderer_2d_draw_quad(renderer_2d, &box_transform, theme->background_color);

    // Render lines
    const fvector3 line_point1 = 
        (fvector3) 
        { 
            origin_position.comp.x + ui_element->padding_x,
            origin_position.comp.y - ui_element->padding_y - (ui_element->height / 2),
            0.2f
        };

    const fvector3 line_point2 = 
        (fvector3) 
        {
            origin_position.comp.x + ui_element->padding_x + ui_element->width,
            origin_position.comp.y - ui_element->padding_y - (ui_element->height / 2),
            0.2f
        };

    renderer_2d_draw_line(renderer_2d, line_point1, line_point2, theme->foreground_color);

    uint16_t slider_x = (ui_element->width)*slider_percentage;

    const fvector3 slider_point1 = 
        (fvector3)
        {
            origin_position.comp.x + ui_element->padding_x + slider_x,
            origin_position.comp.y - ui_element->padding_y - (ui_element->height / 2) - (slider_size_px / 2),
            0.3f
        };

    const fvector3 slider_point2 = 
        (fvector3)
        {
            origin_position.comp.x + ui_element->padding_x + slider_x,
            origin_position.comp.y - ui_element->padding_y - (ui_element->height / 2) + (slider_size_px / 2),
            0.3f
        };

    renderer_2d_draw_line(renderer_2d, slider_point1, slider_point2, theme->accent_color);
}

void ui_element_slider_init(struct ui_element_slider_t* slider_element, 
                            float starting_position,
                            float lower_bound,
                            float upper_bound)
{
    ui_element_init(&(slider_element->base_element),
                    UI_ELEMENT_SLIDER,
                    200,
                    30,
                    ui_element_slider_render,
                    ui_element_slider_event_react);

    slider_element->slider_position = starting_position;

    slider_element->lower_bound = lower_bound;
    slider_element->upper_bound = upper_bound;

    slider_element->base_element.padding_x = 100;
    slider_element->base_element.padding_y = 8;
}

// ------------------------------------------------------------------------------------------------------------

void ui_element_button_init(struct ui_element_button_t* button_element)
{
    ui_element_init(&(button_element->base_element),
                    UI_ELEMENT_BUTTON,
                    100,
                    100,
                    (void*)0,
                    (void*)0);
}

void ui_element_checkbox_init(struct ui_element_checkbox_t* checkbox_element,
                              bool start_checked)
{
    ui_element_init(&(checkbox_element->base_element),
                    UI_ELEMENT_CHECKBOX,
                    100,
                    100,
                    (void*)0,
                    (void*)0);
}