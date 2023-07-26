#pragma once 

struct event_base_t;
struct renderer_2d_t;
struct ui_layer_t;

#include <stdbool.h>
#include <stdint.h>

// Types implementing a "UI element"
enum ui_element_type_t
{
    UI_ELEMENT_STATIC_TEXT,
    UI_ELEMENT_SLIDER,
    UI_ELEMENT_BUTTON,
    UI_ELEMENT_CHECKBOX,
    UI_ELEMENT_DROPDOWN,
    UI_ELEMENT_GRAPH,
    UI_ELEMENT_INPUT_BOX
};

// The core structure for UI elements
struct ui_element_t
{
    // Type that this UI element is implementing
    enum ui_element_type_t element_type;

    // Total width and height of all sub-shapes composing the UI element
    uint16_t width;
    uint16_t height;

    void (*function_ui_element_render)(struct renderer_2d_t* renderer_2d,
                                       struct ui_element_t* ui_element,
                                       uint16_t origin_x,
                                       uint16_t origin_y);

    void (*function_ui_element_event_react)(struct ui_element_t* ui_element, 
                                            struct event_base_t* event);
};

// Implementation structs
struct ui_element_static_text_t
{
    struct ui_element_t base_element;
    char* static_text;
};

struct ui_element_slider_t
{
    struct ui_element_t base_element;
    float slider_position;
};

struct ui_element_button_t
{
    struct ui_element_t base_element;
    bool state;
};

struct ui_element_checkbox_t
{
    struct ui_element_t base_element;
    bool checked;
};

//
void ui_element_init(struct ui_element_t* ui_element,
                     enum ui_element_type_t element_type,
                     uint16_t width, 
                     uint16_t height, 
                     void (*function_ui_element_render)(struct renderer_2d_t* renderer_2d,
                                                        struct ui_element_t* ui_element,
                                                        uint16_t origin_x,
                                                        uint16_t origin_y),
                     void (*function_ui_element_event_react)(struct ui_element_t* ui_element, 
                                                             struct event_base_t* event) );

void ui_element_static_text_init(struct ui_element_static_text_t* text_element, 
                                 char* static_text);

void ui_element_slider_init(struct ui_element_slider_t* slider_element,
                            float starting_position);

void ui_element_button_init(struct ui_element_button_t* button_element);

void ui_element_checkbox_init(struct ui_element_checkbox_t* checkbox_element,
                              bool start_checked);