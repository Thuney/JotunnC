#pragma once

#include "renderer_2d.h"
#include "ui_element.h"
#include "window_layer.h"

#define MAX_UI_CONTAINERS 16
// -- 4x4 grid OR
// -- 16 vertical / horizontal stretch 
// -- Seems reasonable for now, per container
#define MAX_UI_CONTAINER_ELEMENTS 16

// Layout types of a UI container
enum ui_container_layout_t
{
    UI_LAYOUT_VERTICAL,
    UI_LAYOUT_HORIZONTAL,
    UI_LAYOUT_GRID
};

// Container storing UI elements
struct ui_container_t
{
    // Internal layout of elements in this container
    enum ui_container_layout_t layout;

    // "Origin" being the bottom left of the container
    uint16_t origin_x;
    uint16_t origin_y;

    // Total width and height of all UI elements in the container
    uint16_t width;
    uint16_t height;

    uint16_t padding;

    // Number of elements currently stored in this container 
    uint16_t num_elements;

    // Stored references to UI elements in this container
    struct 
    {
        struct ui_element_t* element;
        uint16_t row;
        uint16_t column;

    } contained_elements[MAX_UI_CONTAINER_ELEMENTS];
};

// Theming - coloring and whatnot
struct ui_theme_t
{
    fvector4 accent_color;
    fvector4 foreground_color;
    fvector4 background_color;
};

struct ui_layer_t
{
    struct window_layer_t ui_window_layer;
    struct framebuffer_t ui_framebuffer;

    struct camera_ortho_t ui_camera_ortho;
    struct renderer_2d_t ui_renderer_2d;

    struct ui_theme_t ui_theme;

    uint16_t num_containers;
    struct ui_container_t* ui_containers[MAX_UI_CONTAINERS];
};

//
void ui_theme_init(struct ui_theme_t* ui_theme,
                   fvector4 accent_color,
                   fvector4 foreground_color,
                   fvector4 background_color);

//
void ui_container_init(struct ui_container_t* ui_container,
                       enum ui_container_layout_t layout,
                       uint16_t origin_x, 
                       uint16_t origin_y);

void ui_container_add_element(struct ui_container_t* ui_container,
                              struct ui_element_t* ui_element);

void ui_container_render(struct renderer_2d_t* renderer_2d,
                         struct ui_container_t* ui_container,
                         struct ui_theme_t* theme);

//
void ui_layer_init(struct window_t* parent_window,
                   struct ui_layer_t* ui_layer,
                   uint32_t width,
                   uint32_t height,
                   struct ui_theme_t theme);

void ui_layer_cleanup(struct ui_layer_t* ui_layer);

void ui_layer_add_container(struct ui_layer_t* ui_layer,
                            struct ui_container_t* ui_container);

//