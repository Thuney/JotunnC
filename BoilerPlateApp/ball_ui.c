#include "ball_ui.h"

#include "font.h"

static char* BALL_UI_STATIC_TEXT_CONTENTS_1 = "Tennessee's embrace,";
static char* BALL_UI_STATIC_TEXT_CONTENTS_2 = "Phase sways to K-pop's rhythm,";
static char* BALL_UI_STATIC_TEXT_CONTENTS_3 = "Heart and Seoul entwined.";
static char* BALL_UI_STATIC_TEXT_CONTENTS_4 = "A Haiku By:";
static char* BALL_UI_STATIC_TEXT_CONTENTS_5 = "ChatGPT";

static struct typeface_t ball_ui_typeface;

//
void ball_ui_layer_init(struct ball_ui_layer_t* ball_layer, uint16_t width, uint16_t height, struct window_t* parent_window)
{
    typeface_init(&ball_ui_typeface, "/usr/share/fonts/noto/NotoSerif-Regular.ttf", 20);

    // ui_theme_init(&ball_layer->ui_theme, 
    //                   (fvector4){ 0.3f, 0.3f, 0.3f, 0.7f },
    //                   (fvector4){ 0.27f, 0.33f, 0.32f, 0.7f },
    //                   (fvector4){ 0.16f, 0.3f, 0.3f, 0.7f });

    ui_theme_init(&ball_layer->ui_theme, 
                      (fvector4){ 0.3f, 0.3f, 0.3f, 0.7f },
                      (fvector4){ 0.27f, 0.33f, 0.32f, 0.7f },
                      (fvector4){ 1.0f, 0.0f, 0.0f, 0.7f });

    ui_layer_init(parent_window, &ball_layer->ui_layer, width, height, ball_layer->ui_theme);

    //

    ui_container_init(&ball_layer->ui_container_1, 
                      UI_LAYOUT_VERTICAL,
                      200.0f,
                      200.0f);

    ball_layer->ui_container_1.padding = 5;

    //

    ui_element_static_text_init(&(ball_layer->ball_ui_text_element1),
                                BALL_UI_STATIC_TEXT_CONTENTS_1,
                                &ball_ui_typeface);

    ui_element_static_text_init(&(ball_layer->ball_ui_text_element2),
                                BALL_UI_STATIC_TEXT_CONTENTS_2,
                                &ball_ui_typeface);

    ui_element_static_text_init(&(ball_layer->ball_ui_text_element3),
                                BALL_UI_STATIC_TEXT_CONTENTS_3,
                                &ball_ui_typeface);

    ui_element_static_text_init(&(ball_layer->ball_ui_text_element4),
                                BALL_UI_STATIC_TEXT_CONTENTS_4,
                                &ball_ui_typeface);

    ui_element_static_text_init(&(ball_layer->ball_ui_text_element5),
                                BALL_UI_STATIC_TEXT_CONTENTS_5,
                                &ball_ui_typeface);

    //

    ui_container_add_element(&(ball_layer->ui_container_1),
                             &(ball_layer->ball_ui_text_element1.base_element));

    ui_container_add_element(&(ball_layer->ui_container_1),
                             &(ball_layer->ball_ui_text_element2.base_element));

    ui_container_add_element(&(ball_layer->ui_container_1),
                             &(ball_layer->ball_ui_text_element3.base_element));

    ui_container_add_element(&(ball_layer->ui_container_1),
                             &(ball_layer->ball_ui_text_element4.base_element));

    ui_container_add_element(&(ball_layer->ui_container_1),
                             &(ball_layer->ball_ui_text_element5.base_element));

    //

    ui_layer_add_container(&(ball_layer->ui_layer),
                           &(ball_layer->ui_container_1));

}

void ball_ui_layer_cleanup(struct ball_ui_layer_t* ball_layer)
{
    typeface_cleanup(&ball_ui_typeface);

    ui_layer_cleanup(&(ball_layer->ui_layer));
}