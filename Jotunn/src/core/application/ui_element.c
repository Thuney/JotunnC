#include "ui_element.h"

//
void ui_element_init(struct ui_element_t* ui_element,
                     enum ui_element_type_t element_type,
                     uint16_t width, 
                     uint16_t height,
                     void (*function_ui_element_render)(struct ui_element_t* ui_element,
                                                        uint16_t origin_x,
                                                        uint16_t origin_y),
                     void (*function_ui_element_event_react)(struct ui_element_t* ui_element, 
                                                             struct event_base_t* event) )
{
    ui_element->element_type = element_type;

    ui_element->width        = width;
    ui_element->height       = height;

    ui_element->function_ui_element_render      = function_ui_element_render;
    ui_element->function_ui_element_event_react = function_ui_element_event_react;
}