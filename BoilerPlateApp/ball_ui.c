#include "ball_ui.h"

static char* BALL_UI_STATIC_TEXT_CONTENTS = "Ball UI Layer Test Element";

//
void ball_ui_layer_init(struct ball_ui_layer_t* ball_layer, uint16_t width, uint16_t height, struct window_t* parent_window)
{
    ui_theme_init(&ball_layer->ui_theme, 
                      (fvector4){ 255.0, 255.0, 255.0 },
                      (fvector4){ 255.0, 255.0, 255.0 },
                      (fvector4){ 255.0, 255.0, 255.0 });

    ui_layer_init(parent_window, &ball_layer->ui_layer, width, height, ball_layer->ui_theme);

    ui_container_init(&ball_layer->ui_container_1, 
                      UI_LAYOUT_VERTICAL,
                      200,
                      200,
                      150,
                      100);

    ui_element_static_text_init(&(ball_layer->ball_ui_text_element),
                                BALL_UI_STATIC_TEXT_CONTENTS);

    ui_container_add_element(&(ball_layer->ui_container_1),
                             &(ball_layer->ball_ui_text_element.base_element));

    ui_layer_add_container(&(ball_layer->ui_layer),
                           &(ball_layer->ui_container_1));


}

void ball_ui_layer_cleanup(struct ball_ui_layer_t* ball_layer)
{
    ui_layer_cleanup(&(ball_layer->ui_layer));
}