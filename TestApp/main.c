#include <memory.h>
#include <stdio.h>

#include "application.h"
#include "fvector.h"

struct application_t jotunn_app;
struct window_t window1, window2;

void window1_run(struct window_t* window)
{
    // #ifdef DEBUG
    //     fprintf(stdout, "Running Window 1 Custom Function\n");
    // #endif

    struct renderer_2d_t* renderer = &window->renderer_2d;

    // Grid of shapes (quad - triangle - circle)

    const float spacing = 50.0f;
    const float color_increment = 0.01;

    unsigned int r, c; 

    const unsigned int dim = 12;

    const fvector3 grid_start_offset_position = { 50.0f, 50.0f, 0.0f };

    const float scale_factor = 10.0f;
    const fvector3 scale_factors = (fvector3) { {scale_factor, scale_factor, scale_factor} };

    fmatrix_4x4 scale_matrix, transform_matrix;
    fmatrix_4x4_init(&scale_matrix);

    scale_matrix = fmatrix_4x4_transform_scale(&scale_matrix, scale_factors);

    fvector3 quad_position, triangle_position, circle_position;

    for (r = 0; r <= dim; r++)
    {
        for (c = 0; c <= dim; c += 3)
        {
            quad_position     = (fvector3) { {grid_start_offset_position.comp.x + (spacing*c)    , grid_start_offset_position.comp.y + (spacing*r), 0.0f} };
            triangle_position = (fvector3) { {grid_start_offset_position.comp.x + (spacing*(c+1)), grid_start_offset_position.comp.y + (spacing*r), 0.0f} };
            circle_position   = (fvector3) { {grid_start_offset_position.comp.x + (spacing*(c+2)), grid_start_offset_position.comp.y + (spacing*r), 0.0f} };

            const float val  = (color_increment*(float)(r*dim + c));
            const float val2 = (color_increment*(float)(c*dim + r));

            const fvector4 quad_color     = { val, val2, 0.0f, 1.0f  };
            const fvector4 triangle_color = { 0.0f,  val, val2, 1.0f };
            const fvector4 circle_color   = { 0.0f, val2,  val, 1.0f };

            transform_matrix = fmatrix_4x4_transform_translate(&scale_matrix, quad_position);
            renderer_2d_draw_quad(renderer, &transform_matrix, quad_color);
            transform_matrix = fmatrix_4x4_transform_translate(&scale_matrix, triangle_position);
            renderer_2d_draw_triangle(renderer, &transform_matrix, triangle_color);
            transform_matrix = fmatrix_4x4_transform_translate(&scale_matrix, circle_position);
            renderer_2d_draw_circle(renderer, &transform_matrix, circle_color);
        }
    }

    // Textured Quad

    // const fvector3 textured_quad_position = { 900.0f, 500.0f, 0.0f };

    // const float textured_quad_scale_factor     = 200.0f;
    // const fvector3 textured_quad_scale_factors = (fvector3) { {textured_quad_scale_factor, textured_quad_scale_factor, textured_quad_scale_factor} };

    // fmatrix_4x4 textured_quad_transform_matrix;
    // fmatrix_4x4_init(&textured_quad_transform_matrix);

    // textured_quad_transform_matrix = fmatrix_4x4_transform_scale(&textured_quad_transform_matrix, textured_quad_scale_factors);
    // textured_quad_transform_matrix = fmatrix_4x4_transform_translate(&textured_quad_transform_matrix, textured_quad_position);

    // renderer_2d_draw_textured_quad(renderer, &textured_quad_transform_matrix, &window->test_texture);

    // Line

    const fvector3 line_position_1 = { 1000.0f, 400.0f, 0.0f };
    const fvector3 line_position_2 = { 850.0f, 100.0f, 0.0f };
    const fvector4 line_color      = { 255.0f, 255.0f, 0.0f, 1.0f };

    renderer_2d_draw_line(renderer, line_position_1, line_position_2, line_color);

    // String of text
    const fvector3 text_start_position = { 200.0f, 700.0f, 0.0f };

    renderer_2d_draw_string(renderer, &window->metadata.parent_application->app_typeface, text_start_position, "This Is Some Sample Text With Spaces");
    // renderer_2d_draw_string(&window->renderer, &window->typeface, text_start_position, "!XA!CDEUVXW981902FFF");
    // renderer_2d_draw_string(&window->renderer, &window->typeface, text_start_position, _FONT_LOADED_GLYPHS_STRING);
}

void window2_run(struct window_t* window)
{
    struct renderer_2d_t* renderer = &window->renderer_2d;

    // Grid of shapes (triangle - circle)

    const float spacing = 50.0f;
    const float color_increment = 0.01;

    unsigned int r, c; 

    const unsigned int dim = 12;

    const fvector3 grid_start_offset_position = { 50.0f, 50.0f, 0.0f };

    const float scale_factor = 10.0f;
    const fvector3 scale_factors = (fvector3) { {scale_factor, scale_factor, scale_factor} };

    fmatrix_4x4 scale_matrix, transform_matrix;
    fmatrix_4x4_init(&scale_matrix);

    scale_matrix = fmatrix_4x4_transform_scale(&scale_matrix, scale_factors);

    // fvector3 quad_position;
    fvector3 triangle_position; 
    fvector3 circle_position;

    for (r = 0; r <= dim; r++)
    {
        for (c = 0; c <= dim; c += 3)
        {
            // quad_position     = (fvector3) { {grid_start_offset_position.comp.x + (spacing*c)    , grid_start_offset_position.comp.y + (spacing*r), 0.0f} };
            triangle_position = (fvector3) { {grid_start_offset_position.comp.x + (spacing*(c+1)), grid_start_offset_position.comp.y + (spacing*r), 0.0f} };
            circle_position   = (fvector3) { {grid_start_offset_position.comp.x + (spacing*(c+2)), grid_start_offset_position.comp.y + (spacing*r), 0.0f} };

            const float val  = (color_increment*(float)(r*dim + c));
            const float val2 = (color_increment*(float)(c*dim + r));

            // const fvector4 quad_color     = { val, val2, 0.0f, 1.0f  };
            const fvector4 triangle_color = { 0.0f,  val, val2, 1.0f };
            const fvector4 circle_color   = { 0.0f, val2,  val, 1.0f };

            // transform_matrix = fmatrix_4x4_transform_translate(&scale_matrix, quad_position);
            // renderer_2d_draw_quad(renderer, &transform_matrix, quad_color);
            transform_matrix = fmatrix_4x4_transform_translate(&scale_matrix, triangle_position);
            renderer_2d_draw_triangle(renderer, &transform_matrix, triangle_color);
            transform_matrix = fmatrix_4x4_transform_translate(&scale_matrix, circle_position);
            renderer_2d_draw_circle(renderer, &transform_matrix, circle_color);
        }
    }
}

uint8_t init_window1(struct application_t* app, struct window_t* window1)
{
    uint8_t error = 0U;

    error |= window_init(window1, 600, 400, "JotunnWindow1", app);

    fvector4 window1_background_color;
    fvector4_set(&window1_background_color, 0.1f, 0.1f, 0.1f, 1.0f);

    window_set_background_color(window1, window1_background_color);

    return error;
}

uint8_t init_window2(struct application_t* app, struct window_t* window2)
{
    uint8_t error = 0U;

    error |= window_init(window2, 200, 200, "JotunnWindow2", app);

    fvector4 window2_background_color;
    fvector4_set(&window2_background_color, 0.1f, 0.1f, 0.1f, 1.0f);

    window_set_background_color(window2, window2_background_color);

    return error;
}

int main(int argc, char** argv)
{
    const uint8_t max_windows = 2;

    struct application_t* app_ptr = &jotunn_app;
    struct window_t *window1_ptr = &window1;
    struct window_t *window2_ptr = &window2;

    uint8_t error = 0U;

    init_window1(app_ptr, window1_ptr);
    init_window2(app_ptr, window2_ptr);

    error |= application_init(app_ptr, "JotunnTestApp", max_windows);

    window_set_function_custom_window_run(window1_ptr, &window1_run);
    window_set_function_custom_window_run(window2_ptr, &window2_run);

    error |= application_add_window(app_ptr, window1);
    error |= application_add_window(app_ptr, window2);

    if (!error)
    {
        application_start(app_ptr);

        #ifdef DEBUG
            fprintf(stdout, "Running application\n");
        #endif

        while (app_ptr->running)
        {
            application_run(app_ptr);
        }

        application_stop(app_ptr);
        application_cleanup(app_ptr);
    }
#ifdef DEBUG
    else
    {
        fprintf(stdout, "Error initializing application\n");
    }
    fprintf(stdout, "Jobs done from main.c\n");
    fflush(stdout);
#endif  
}