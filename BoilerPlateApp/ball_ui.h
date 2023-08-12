#pragma once

#include <stdint.h>

#include "ui_layer.h"

struct ball_ui_layer_t
{
    struct ui_layer_t ui_layer;
    struct ui_theme_t ui_theme;

    struct ui_container_t ui_container_1;

    struct ui_element_static_text_t ball_ui_text_element1;
    struct ui_element_static_text_t ball_ui_text_element2;
    struct ui_element_static_text_t ball_ui_text_element3;
    struct ui_element_static_text_t ball_ui_text_element4;
    struct ui_element_static_text_t ball_ui_text_element5;

};

void ball_ui_layer_init(struct ball_ui_layer_t* ball_layer, uint16_t width, uint16_t height, struct window_t* parent_window);

void ball_ui_layer_cleanup(struct ball_ui_layer_t* ball_layer);