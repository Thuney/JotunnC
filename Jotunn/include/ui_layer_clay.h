#pragma once

struct typeface_t;

#include "event.h"
#include "renderer_2d.h"
#include "window_layer.h"

struct ui_layer_clay_t
{
    struct window_layer_t ui_window_layer;
    struct framebuffer_t ui_framebuffer;

    struct camera_ortho_t ui_camera_ortho;
    struct renderer_2d_t ui_renderer_2d;

    struct typeface_t* ui_layer_typeface;

    const void* (*function_clay_ui_create_layout)
        (struct ui_layer_clay_t* ui_layer_clay);
};

//
void ui_layer_clay_init(struct window_t* parent_window,
                        struct ui_layer_clay_t* ui_layer_clay,
                        struct typeface_t* ui_layer_typeface,
                        uint32_t width,
                        uint32_t height,
                        const void* (*function_clay_ui_create_layout)(struct ui_layer_clay_t* ui_layer_clay));

void ui_layer_clay_cleanup(struct ui_layer_clay_t* ui_layer_clay);

//