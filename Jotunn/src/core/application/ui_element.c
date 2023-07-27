#include "ui_element.h"

#include "renderer_2d.h"

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

    ui_element->width        = width;
    ui_element->height       = height;

    if (function_ui_element_render)
        ui_element->function_ui_element_render = function_ui_element_render;
    
    if (function_ui_element_event_react) 
        ui_element->function_ui_element_event_react = function_ui_element_event_react;
}

static void ui_element_static_text_render(struct renderer_2d_t* renderer_2d,
                                          struct ui_element_t* ui_element,
                                          uint16_t origin_x,
                                          uint16_t origin_y,
                                          struct ui_theme_t* theme)
{

    struct ui_element_static_text_t* static_text_element = (struct ui_element_static_text_t*)ui_element;

    renderer_2d_draw_string(renderer_2d, &(renderer_2d->typeface), (fvector3) { origin_x, origin_y, 0.1f }, static_text_element->static_text);
}

void ui_element_static_text_init(struct ui_element_static_text_t* text_element, 
                                 char* static_text)
{
    ui_element_init(&(text_element->base_element),
                    UI_ELEMENT_STATIC_TEXT,
                    100,
                    100,
                    ui_element_static_text_render,
                    (void*)0);

    text_element->static_text = static_text;
}

void ui_element_slider_init(struct ui_element_slider_t* slider_element, 
                            float starting_position)
{
    ui_element_init(&(slider_element->base_element),
                    UI_ELEMENT_SLIDER,
                    100,
                    100,
                    (void*)0,
                    (void*)0);
}

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