#include "ball_ui.h"

#include "font.h"

static char* BALL_UI_STATIC_TEXT_CONTENTS_1 = "* Element 1 *";
static char* BALL_UI_STATIC_TEXT_CONTENTS_2 = "- Element 2 -";
// static char* BALL_UI_STATIC_TEXT_CONTENTS_3 = "> Element 3 < Is much longer than the other elements ______!!!";
static char* BALL_UI_STATIC_TEXT_CONTENTS_3 = "> Element 3 <";
static char* BALL_UI_STATIC_TEXT_CONTENTS_4 = "| Element 4 |";
static char* BALL_UI_STATIC_TEXT_CONTENTS_5 = "_ Element 5 _";

static struct typeface_t ball_ui_typeface;

//
void ball_ui_layer_init(struct ball_ui_layer_t* ball_layer, uint16_t width, uint16_t height, struct window_t* parent_window)
{
    typeface_init(&ball_ui_typeface, "/usr/share/fonts/noto/NotoSansMono-Regular.ttf", 18);

    ui_theme_init(&ball_layer->ui_theme,
                  ui_color_from_hex(0xFF0000FF),
                  ui_color_from_hex(0xFFFFFFFF),
                  ui_color_from_hex(0x000000FF));

    ui_layer_init(parent_window, &ball_layer->ui_layer, width, height, ball_layer->ui_theme);

    //

    struct ui_container_t* container = &ball_layer->ui_container_1;

    struct ui_element_static_text_t* static_text1 = &(ball_layer->ball_ui_text_element1);
    struct ui_element_static_text_t* static_text2 = &(ball_layer->ball_ui_text_element2);
    struct ui_element_static_text_t* static_text3 = &(ball_layer->ball_ui_text_element3);
    struct ui_element_static_text_t* static_text4 = &(ball_layer->ball_ui_text_element4);
    struct ui_element_static_text_t* static_text5 = &(ball_layer->ball_ui_text_element5);

    struct ui_element_slider_t* slider = &(ball_layer->slider_element_1);

    ui_container_init(container, 
                      UI_LAYOUT_VERTICAL);

    ui_container_set_padding(container, 5);

    //

    ui_element_static_text_init(static_text1,
                                BALL_UI_STATIC_TEXT_CONTENTS_1,
                                &ball_ui_typeface);
    ball_layer->ball_ui_text_element1.base_element.element_centering = UI_ELEMENT_ALIGN_CENTER;

    ui_element_static_text_init(&(ball_layer->ball_ui_text_element2),
                                BALL_UI_STATIC_TEXT_CONTENTS_2,
                                &ball_ui_typeface);
    ball_layer->ball_ui_text_element2.base_element.element_centering = UI_ELEMENT_ALIGN_CENTER;

    ui_element_static_text_init(&(ball_layer->ball_ui_text_element3),
                                BALL_UI_STATIC_TEXT_CONTENTS_3,
                                &ball_ui_typeface);
    ball_layer->ball_ui_text_element3.base_element.element_centering = UI_ELEMENT_ALIGN_CENTER;

    ui_element_static_text_init(&(ball_layer->ball_ui_text_element4),
                                BALL_UI_STATIC_TEXT_CONTENTS_4,
                                &ball_ui_typeface);
    ball_layer->ball_ui_text_element4.base_element.element_centering = UI_ELEMENT_ALIGN_CENTER;

    ui_element_static_text_init(&(ball_layer->ball_ui_text_element5),
                                BALL_UI_STATIC_TEXT_CONTENTS_5,
                                &ball_ui_typeface);
    ball_layer->ball_ui_text_element5.base_element.element_centering = UI_ELEMENT_ALIGN_CENTER;

    ui_element_slider_init(slider,
                           50.0f,
                           0.0f,
                           100.0f);

    //

    ui_container_add_element(container,
                             &(static_text1->base_element));

    ui_container_add_element(container,
                             &(static_text2->base_element));

    ui_container_add_element(container,
                             &(static_text3->base_element));

    ui_container_add_element(container,
                             &(static_text4->base_element));

    ui_container_add_element(container,
                             &(static_text5->base_element));

    ui_container_add_element(container,
                             &(slider->base_element));

    //

    ui_container_set_origin(&ball_layer->ui_container_1, 
                            0.0f, 
                            (height - ball_layer->ui_container_1.height));

    ui_layer_add_container(&(ball_layer->ui_layer),
                           container);

}

void ball_ui_layer_cleanup(struct ball_ui_layer_t* ball_layer)
{
    typeface_cleanup(&ball_ui_typeface);

    ui_layer_cleanup(&(ball_layer->ui_layer));
}