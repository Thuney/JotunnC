#include <memory.h>
#include <stdio.h>

#include "application.h"
#include "camera.h"
#include "fvector.h"
#include "renderer_2d.h"
#include "window_layer.h"

#include "texture.h"

static struct application_t jotunn_app;
static struct window_t window1, window2;
static struct window_layer_t window1_layer, window2_layer;
static struct framebuffer_t window1_framebuffer, window2_framebuffer;

static struct camera_ortho_t window1_camera_ortho;
static struct renderer_2d_t window1_renderer_2d;

static struct camera_ortho_t window2_camera_ortho;
static struct renderer_2d_t window2_renderer_2d;

static struct texture_2d_t aaron_shakespeare_texture;
static struct texture_2d_t lenna_texture;

struct typeface_t window1_typeface;
struct typeface_t window2_typeface;

//

void window1_run(struct window_layer_t* window_layer)
{
    // #ifdef DEBUG
    //     fprintf(stdout, "Running Window 1 Custom Function\n");
    // #endif

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
            renderer_2d_draw_quad(&window1_renderer_2d, &transform_matrix, quad_color);
            transform_matrix = fmatrix_4x4_transform_translate(&scale_matrix, triangle_position);
            renderer_2d_draw_triangle(&window1_renderer_2d, &transform_matrix, triangle_color);
            transform_matrix = fmatrix_4x4_transform_translate(&scale_matrix, circle_position);
            renderer_2d_draw_circle(&window1_renderer_2d, &transform_matrix, circle_color);
        }
    }

    // Textured Quads

    const fvector3 aaron_textured_quad_position = { 900.0f, 500.0f, 0.0f };

    const float aaron_textured_quad_scale_factor     = 200.0f;
    const fvector3 aaron_textured_quad_scale_factors = (fvector3) { {aaron_textured_quad_scale_factor, aaron_textured_quad_scale_factor, aaron_textured_quad_scale_factor} };

    fmatrix_4x4 aaron_textured_quad_transform_matrix;
    fmatrix_4x4_init(&aaron_textured_quad_transform_matrix);

    aaron_textured_quad_transform_matrix = fmatrix_4x4_transform_scale(&aaron_textured_quad_transform_matrix, aaron_textured_quad_scale_factors);
    aaron_textured_quad_transform_matrix = fmatrix_4x4_transform_translate(&aaron_textured_quad_transform_matrix, aaron_textured_quad_position);

    renderer_2d_draw_textured_quad(&window1_renderer_2d, &aaron_textured_quad_transform_matrix, &aaron_shakespeare_texture);

    //

    const fvector3 lenna_textured_quad_position = { 400.0f, 800.0f, 0.0f };

    const float lenna_textured_quad_scale_factor     = 350.0f;
    const fvector3 lenna_textured_quad_scale_factors = (fvector3) { {lenna_textured_quad_scale_factor, lenna_textured_quad_scale_factor, lenna_textured_quad_scale_factor} };

    fmatrix_4x4 lenna_textured_quad_transform_matrix;
    fmatrix_4x4_init(&lenna_textured_quad_transform_matrix);

    lenna_textured_quad_transform_matrix = fmatrix_4x4_transform_scale(&lenna_textured_quad_transform_matrix, lenna_textured_quad_scale_factors);
    lenna_textured_quad_transform_matrix = fmatrix_4x4_transform_translate(&lenna_textured_quad_transform_matrix, lenna_textured_quad_position);

    renderer_2d_draw_textured_quad(&window1_renderer_2d, &lenna_textured_quad_transform_matrix, &lenna_texture);

    // Line

    const fvector3 line_position_1 = { 1000.0f, 400.0f, 0.0f };
    const fvector3 line_position_2 = { 850.0f, 100.0f, 0.0f };
    const fvector4 line_color      = { 255.0f, 255.0f, 0.0f, 1.0f };

    renderer_2d_draw_line(&window1_renderer_2d, line_position_1, line_position_2, line_color);

    // String of text
    const fvector3 text_start_position = { 200.0f, 700.0f, 0.0f };
    
    // renderer_2d_draw_string(renderer, &window->renderer_2d.typeface, text_start_position, "A");
    renderer_2d_draw_string(&window1_renderer_2d, &window1_typeface, text_start_position, "This Is Some Sample Text With Spaces");
    // renderer_2d_draw_string(&window->renderer, &window->typeface, text_start_position, "!XA!CDEUVXW981902FFF");
    // renderer_2d_draw_string(&window->renderer, &window->typeface, text_start_position, _FONT_LOADED_GLYPHS_STRING);
}

void window2_run(struct window_layer_t* window_layer)
{
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
            renderer_2d_draw_triangle(&window2_renderer_2d, &transform_matrix, triangle_color);
            transform_matrix = fmatrix_4x4_transform_translate(&scale_matrix, circle_position);
            renderer_2d_draw_circle(&window2_renderer_2d, &transform_matrix, circle_color);
        }
    }

    // Line

    const fvector3 line_position_1 = { 1000.0f, 400.0f, 0.0f };
    const fvector3 line_position_2 = { 850.0f, 100.0f, 0.0f };
    const fvector4 line_color      = { 127.0f, 33.0f, 127.0f, 1.0f };

    renderer_2d_draw_line(&window2_renderer_2d, line_position_1, line_position_2, line_color);

    // String of text
    const fvector3 text_start_position = { 100.0f, 900.0f, 0.0f };
    
    renderer_2d_draw_string(&window2_renderer_2d, &(window2_typeface), text_start_position, _FONT_LOADED_GLYPHS_STRING);
}

uint8_t init_window1(struct application_t* app, struct window_t* window1)
{
    uint8_t error = 0U;

    const uint32_t width  = 600;
    const uint32_t height = 400;
    const uint8_t num_layers = 1;

    error |= window_init(window1, num_layers, width, height, "JotunnWindow1", app);

    if (!error)
    {
        framebuffer_init(&window1_framebuffer, width, height);
        window_layer_init(window1, &window1_layer, &window1_framebuffer, (struct camera_base_t*)&window1_camera_ortho, (struct renderer_base_t*)&window1_renderer_2d);
        window_add_layer(window1, &window1_layer);

        fvector4 window1_background_color;
        fvector4_set(&window1_background_color, 0.3f, 0.3f, 0.3f, 1.0f);
        // fvector4_set(&window1_background_color, 1.0f, 1.0f, 1.0f, 1.0f);

        // Camera stuff
        const fvector3 camera_position = (fvector3) { {0.0f, 0.0f,  2.0f} };
        const fvector3 camera_up       = (fvector3) { {0.0f, 1.0f,  0.0f} };
        const fvector3 camera_front    = (fvector3) { {0.0f, 0.0f, -1.0f} };

        const float ortho_left   = 0.0f; 
        const float ortho_right  = (float)width;
        const float ortho_top    = (float)height;
        const float ortho_bottom = 0.0f;
        const float ortho_near_plane = -3.0f;
        const float ortho_far_plane  = 100.0f;

        window_set_context(window1);

        typeface_init(&window1_typeface, "/usr/share/fonts/noto/NotoSerif-Regular.ttf", 18);

        camera_init_orthographic(&window1_camera_ortho, camera_position, camera_up, camera_front);
        camera_set_projection_orthographic(&window1_camera_ortho, ortho_left, ortho_right, ortho_top, ortho_bottom, ortho_near_plane, ortho_far_plane);

        renderer_2d_init(&window1_renderer_2d, window1_layer.camera, window1, "renderer_2d");

        texture_2d_create_from_file_path(&aaron_shakespeare_texture, "/home/loki/Repos/JotunnC/Jotunn/res/textures/AaronShakespeare.png", 1);
        texture_2d_create_from_file_path(&lenna_texture, "/home/loki/Repos/JotunnC/Jotunn/res/textures/lenna.png", 1);

        window_set_background_color(window1, window1_background_color);

        window_release_context();
    }

    return error;
}

uint8_t init_window2(struct application_t* app, struct window_t* window2)
{
    uint8_t error = 0U;

    const uint32_t width  = 200;
    const uint32_t height = 200;
    const uint8_t num_layers = 1;

    error |= window_init(window2, num_layers, width, height, "JotunnWindow2", app);

    if (!error)
    {
        framebuffer_init(&window2_framebuffer, width, height);
        window_layer_init(window2, &window2_layer, &window2_framebuffer, (struct camera_base_t*)&window2_camera_ortho, (struct renderer_base_t*)&window2_renderer_2d);
        window_add_layer(window2, &window2_layer);

        fvector4 window2_background_color;
        fvector4_set(&window2_background_color, 0.3f, 0.3f, 0.3f, 1.0f);
        // fvector4_set(&window2_background_color, 1.0f, 1.0f, 1.0f, 1.0f);

        // Camera stuff
        const fvector3 camera_position = (fvector3) { {0.0f, 0.0f,  2.0f} };
        const fvector3 camera_up       = (fvector3) { {0.0f, 1.0f,  0.0f} };
        const fvector3 camera_front    = (fvector3) { {0.0f, 0.0f, -1.0f} };

        const float ortho_left   = 0.0f; 
        const float ortho_right  = (float)width;
        const float ortho_top    = (float)height;
        const float ortho_bottom = 0.0f;
        const float ortho_near_plane = -3.0f;
        const float ortho_far_plane  = 100.0f;

        window_set_context(window2);

        typeface_init(&window2_typeface, "/usr/share/fonts/noto/NotoSerif-Regular.ttf", 18);

        camera_init_orthographic(&window2_camera_ortho, camera_position, camera_up, camera_front);
        camera_set_projection_orthographic(&window2_camera_ortho, ortho_left, ortho_right, ortho_top, ortho_bottom, ortho_near_plane, ortho_far_plane);

        renderer_2d_init(&window2_renderer_2d, window2_layer.camera, window2, "renderer_2d");

        window_set_background_color(window2, window2_background_color);

        window_release_context();
    }

    return error;
}

int main(int argc, char** argv)
{
    const uint8_t max_windows = 2;

    struct application_t* app_ptr = &jotunn_app;
    struct window_t *window1_ptr = &window1;
    struct window_t *window2_ptr = &window2;

    uint8_t error = 0U;

    error |= application_init(app_ptr, "JotunnTestApp", max_windows);
    init_window1(app_ptr, window1_ptr);
    init_window2(app_ptr, window2_ptr);

    window_layer_set_custom_layer_run(&window1_layer, &window1_run);
    window_layer_set_custom_layer_run(&window2_layer, &window2_run);

    error |= application_add_window(app_ptr, window1_ptr);
    error |= application_add_window(app_ptr, window2_ptr);

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

        renderer_2d_cleanup(&window1_renderer_2d);
        renderer_2d_cleanup(&window2_renderer_2d);

        framebuffer_cleanup(&(window1_framebuffer));
        framebuffer_cleanup(&(window2_framebuffer));

        texture_2d_cleanup(&aaron_shakespeare_texture);
        texture_2d_cleanup(&lenna_texture);

        typeface_cleanup(&(window1_typeface));
        typeface_cleanup(&(window2_typeface));
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