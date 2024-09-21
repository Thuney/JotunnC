#include "ui_element.h"

#include "renderer_2d.h"
#include "string.h"
#include "ui_layer.h"

//
void ui_element_init(struct ui_element_t* ui_element,
                     enum ui_element_type_t element_type,
                     uint16_t width, 
                     uint16_t height,
                     void (*function_ui_element_render)(struct renderer_2d_t* renderer_2d,
                                                        struct ui_element_t* ui_element,
                                                        uint16_t origin_x,
                                                        uint16_t origin_y,
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
                                          uint16_t origin_x,
                                          uint16_t origin_y,
                                          struct ui_theme_t* theme)
{
    struct ui_element_static_text_t* static_text_element = (struct ui_element_static_text_t*)ui_element;

    renderer_2d_draw_string(renderer_2d, 
                            static_text_element->typeface, 
                            (fvector3) { 
                                         origin_x + ui_element->padding_x,
                                         origin_y - (ui_element->height + ui_element->padding_y),
                                         0.1f 
                                       }, 
                            static_text_element->static_text);
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

    text_element->base_element.padding_x = 10;
    text_element->base_element.padding_y = 3;
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

static void ui_element_slider_render(struct renderer_2d_t* renderer_2d,
                                     struct ui_element_t* ui_element,
                                     uint16_t origin_x,
                                     uint16_t origin_y,
                                     struct ui_theme_t* theme)
{
    struct ui_element_slider_t* slider_element = (struct ui_element_slider_t*)ui_element;

    float slider_range = (slider_element->upper_bound - slider_element->lower_bound);
    float slider_percentage = ((slider_element->slider_position - slider_element->lower_bound) / slider_range);

    uint16_t slider_margin = 10U;

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
            origin_x + ui_element->padding_x, 
            origin_y - (ui_element->height + ui_element->padding_y), 
            0.1f 
        };

    fmatrix_4x4 box_transform = get_transform(box_scale_factors, box_translation_vector);

    renderer_2d_draw_quad(renderer_2d, &box_transform, theme->foreground_color);

    const fvector3 line_point1 = 
        (fvector3) 
        { 
            origin_x + ui_element->padding_x + slider_margin,
            origin_y - ((ui_element->height / 2) + ui_element->padding_y),
            0.2f
        };

    const fvector3 line_point2 = 
        (fvector3) 
        {
            origin_x + ui_element->width + ui_element->padding_x - slider_margin,
            origin_y - ((ui_element->height / 2) + ui_element->padding_y),
            0.2f
        };

    renderer_2d_draw_line(renderer_2d, line_point1, line_point2, theme->accent_color);

    uint16_t slider_x = (ui_element->width - 2*(ui_element->padding_x + slider_margin))*slider_percentage;

    const fvector3 slider_point1 = 
        (fvector3)
        {
            origin_x + ui_element->padding_x + slider_x,
            origin_y - ((ui_element->height / 2) + ui_element->padding_y) + slider_margin,
            0.3f
        };

    const fvector3 slider_point2 = 
        (fvector3)
        {
            origin_x + ui_element->padding_x + slider_x,
            origin_y - ((ui_element->height / 2) + ui_element->padding_y) - slider_margin,
            0.3f
        };

    renderer_2d_draw_line(renderer_2d, slider_point1, slider_point2, theme->foreground_color);
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
                    (void*)0);

    slider_element->slider_position = starting_position;

    slider_element->lower_bound = lower_bound;
    slider_element->upper_bound = upper_bound;

    slider_element->base_element.padding_x = 10;
    slider_element->base_element.padding_y = 3;
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