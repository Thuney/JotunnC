#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

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

static struct camera_orthographic_t window1_camera_ortho;
static struct renderer_2d_t window1_renderer_2d;

static struct camera_orthographic_t window2_camera_ortho;
static struct renderer_2d_t window2_renderer_2d;

static struct texture_2d_t aaron_shakespeare_texture;
static struct texture_2d_t lenna_texture;

static struct font_typeface_t window1_typeface;
static struct font_typeface_t window2_typeface;

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Draws window 1's scene with its renderer: a grid of outlined quads,
//  triangles and circles shaded by their place in it, the two textured quads,
//  a line and a line of text. Set as the custom run of window 1's layer
//
//  in/out: inout_window_layer
static void test_app_window_1_run(
  struct window_layer_t* inout_window_layer);

// Draws window 2's scene with its renderer: a grid of triangles and circles
//  shaded by their place in it, a line, and every glyph its typeface loads.
//  Set as the custom run of window 2's layer
//
//  in/out: inout_window_layer
static void test_app_window_2_run(
  struct window_layer_t* inout_window_layer);

// Creates window 1 (600 x 400 with one layer), and once it exists loads its
//  typeface and the two textures (from ../../res/textures, relative to the
//  working directory) and sets up its camera, framebuffer, renderer and
//  layer. The window isn't added to the application
//
//  in/out: inout_app
//  out:    out_window1
//  ret:    0 on success; non-zero when the window can't be created, in which
//            case nothing else is set up
static uint8_t test_app_window_1_init(
  struct application_t* inout_app,
  struct window_t* out_window1);

// Creates window 2 (200 x 200 with one layer), and once it exists loads its
//  typeface and sets up its camera, framebuffer, renderer and layer. The
//  window isn't added to the application
//
//  in/out: inout_app
//  out:    out_window2
//  ret:    0 on success; non-zero when the window can't be created, in which
//            case nothing else is set up
static uint8_t test_app_window_2_init(
  struct application_t* inout_app,
  struct window_t* out_window2);

/*******************************************************************************
 *
 *   Function: main()
 *
 ******************************************************************************/
int main(
  int argc,
  char** argv)
{
  const uint8_t max_windows = 2;

  struct application_t* app_ptr = &(jotunn_app);
  struct window_t* window1_ptr = &(window1);
  struct window_t* window2_ptr = &(window2);

  uint8_t error = 0U;

  error |= application_init(
    app_ptr,
    "JotunnTestApp",
    max_windows);
  test_app_window_1_init(
    app_ptr,
    window1_ptr);
  test_app_window_2_init(
    app_ptr,
    window2_ptr);

  window_layer_set_custom_layer_run(
    &(window1_layer),
    &(test_app_window_1_run));
  window_layer_set_custom_layer_run(
    &(window2_layer),
    &(test_app_window_2_run));

  error |= application_add_window(
    app_ptr,
    window1_ptr);
  error |= application_add_window(
    app_ptr,
    window2_ptr);

  if (!error) {
    application_start(app_ptr);

        #ifdef DEBUG
    fprintf(
      stdout,
      "Running application\n");
        #endif

    while (app_ptr->running) {
      application_run(app_ptr);
    }

    application_stop(app_ptr);

    renderer_2d_cleanup(&(window1_renderer_2d));
    renderer_2d_cleanup(&(window2_renderer_2d));

    framebuffer_cleanup(&(window1_framebuffer));
    framebuffer_cleanup(&(window2_framebuffer));

    texture_2d_cleanup(&(aaron_shakespeare_texture));
    texture_2d_cleanup(&(lenna_texture));

    font_typeface_cleanup(&(window1_typeface));
    font_typeface_cleanup(&(window2_typeface));

    application_cleanup(app_ptr);
  }
#ifdef DEBUG
  else {
    fprintf(
      stdout,
      "Error initializing application\n");
  }
  fprintf(
    stdout,
    "Jobs done from main.c\n");
  fflush(stdout);
#endif
}

/*******************************************************************************
 *
 *   Function: test_app_window_1_run()
 *
 ******************************************************************************/
static void test_app_window_1_run(
  struct window_layer_t* inout_window_layer)
{
  // #ifdef DEBUG
  //     fprintf(stdout, "Running Window 1 Custom Function\n");
  // #endif

  // Grid of shapes (quad - triangle - circle)

  const float spacing = 50.0f;
  const float color_increment = 0.01;

  unsigned int r, c;

  const unsigned int dim = 12;

  const union fvector3_t grid_start_offset_position = {50.0f, 50.0f, 0.0f};

  const float scale_factor = 10.0f;
  const union fvector3_t scale_factors =
    (union fvector3_t){{scale_factor, scale_factor, scale_factor}};

  union fmatrix_4x4_t scale_matrix, transform_matrix;
  fmatrix_4x4_init(&(scale_matrix));

  scale_matrix = fmatrix_4x4_transform_scale(
    &(scale_matrix),
    scale_factors);

  union fvector3_t quad_position, triangle_position, circle_position;

  for (r = 0; r <= dim; r++) {
    for (c = 0; c <= dim; c += 3) {
      quad_position =
        (union fvector3_t){{grid_start_offset_position.comp.x + (spacing * c),
                    grid_start_offset_position.comp.y + (spacing * r),
                    0.0f}};
      triangle_position =
        (union fvector3_t){{grid_start_offset_position.comp.x + (spacing * (c + 1)),
                    grid_start_offset_position.comp.y + (spacing * r),
                    0.0f}};
      circle_position =
        (union fvector3_t){{grid_start_offset_position.comp.x + (spacing * (c + 2)),
                    grid_start_offset_position.comp.y + (spacing * r),
                    0.0f}};

      const float val = (color_increment * (float)(r * dim + c));
      const float val2 = (color_increment * (float)(c * dim + r));

      const union fvector4_t quad_color = {val, val2, 0.0f, 1.0f};
      const union fvector4_t triangle_color = {0.0f, val, val2, 1.0f};
      const union fvector4_t circle_color = {0.0f, val2, val, 1.0f};

      transform_matrix = fmatrix_4x4_transform_translate(
        &(scale_matrix),
        quad_position);
      renderer_2d_draw_unfilled_quad(
        &(window1_renderer_2d),
        &(transform_matrix),
        quad_color);
      transform_matrix = fmatrix_4x4_transform_translate(
        &(scale_matrix),
        triangle_position);
      renderer_2d_draw_triangle(
        &(window1_renderer_2d),
        &(transform_matrix),
        triangle_color);
      transform_matrix = fmatrix_4x4_transform_translate(
        &(scale_matrix),
        circle_position);
      renderer_2d_draw_circle(
        &(window1_renderer_2d),
        &(transform_matrix),
        circle_color);
    }
  }

  // Textured Quads

  const union fvector3_t aaron_textured_quad_position = {900.0f, 500.0f, 0.0f};

  const float aaron_textured_quad_scale_factor = 200.0f;
  const union fvector3_t aaron_textured_quad_scale_factors =
    (union fvector3_t){{aaron_textured_quad_scale_factor,
                aaron_textured_quad_scale_factor,
                aaron_textured_quad_scale_factor}};

  union fmatrix_4x4_t aaron_textured_quad_transform_matrix;
  fmatrix_4x4_init(&(aaron_textured_quad_transform_matrix));

  aaron_textured_quad_transform_matrix = fmatrix_4x4_transform_scale(
    &(aaron_textured_quad_transform_matrix),
    aaron_textured_quad_scale_factors);
  aaron_textured_quad_transform_matrix = fmatrix_4x4_transform_translate(
    &(aaron_textured_quad_transform_matrix),
    aaron_textured_quad_position);

  renderer_2d_draw_textured_quad(
    &(window1_renderer_2d),
    &(aaron_textured_quad_transform_matrix),
    &(aaron_shakespeare_texture));

  //

  const union fvector3_t lenna_textured_quad_position = {400.0f, 800.0f, 0.0f};

  const float lenna_textured_quad_scale_factor = 350.0f;
  const union fvector3_t lenna_textured_quad_scale_factors =
    (union fvector3_t){{lenna_textured_quad_scale_factor,
                lenna_textured_quad_scale_factor,
                lenna_textured_quad_scale_factor}};

  union fmatrix_4x4_t lenna_textured_quad_transform_matrix;
  fmatrix_4x4_init(&(lenna_textured_quad_transform_matrix));

  lenna_textured_quad_transform_matrix = fmatrix_4x4_transform_scale(
    &(lenna_textured_quad_transform_matrix),
    lenna_textured_quad_scale_factors);
  lenna_textured_quad_transform_matrix = fmatrix_4x4_transform_translate(
    &(lenna_textured_quad_transform_matrix),
    lenna_textured_quad_position);

  renderer_2d_draw_textured_quad(
    &(window1_renderer_2d),
    &(lenna_textured_quad_transform_matrix),
    &(lenna_texture));

  // Line

  const union fvector3_t line_position_1 = {1000.0f, 400.0f, 0.0f};
  const union fvector3_t line_position_2 = {850.0f, 100.0f, 0.0f};
  const union fvector4_t line_color = {255.0f, 255.0f, 0.0f, 1.0f};

  renderer_2d_draw_line(
    &(window1_renderer_2d),
    line_position_1,
    line_position_2,
    line_color);

  // String of text
  const union fvector3_t text_start_position = {200.0f, 700.0f, 0.0f};

  // renderer_2d_draw_string(renderer, &window->renderer_2d.typeface,
  //  text_start_position, "A");
  renderer_2d_draw_string(
    &(window1_renderer_2d),
    &(window1_typeface),
    text_start_position,
    "This Is Some Sample Text With Spaces");
  // renderer_2d_draw_string(&window->renderer, &window->typeface,
  //  text_start_position, "!XA!CDEUVXW981902FFF");
  // renderer_2d_draw_string(&window->renderer, &window->typeface,
  //  text_start_position, _FONT_LOADED_GLYPHS_STRING);
}

/*******************************************************************************
 *
 *   Function: test_app_window_2_run()
 *
 ******************************************************************************/
static void test_app_window_2_run(
  struct window_layer_t* inout_window_layer)
{
  // Grid of shapes (triangle - circle)

  const float spacing = 50.0f;
  const float color_increment = 0.01;

  unsigned int r, c;

  const unsigned int dim = 12;

  const union fvector3_t grid_start_offset_position = {50.0f, 50.0f, 0.0f};

  const float scale_factor = 10.0f;
  const union fvector3_t scale_factors =
    (union fvector3_t){{scale_factor, scale_factor, scale_factor}};

  union fmatrix_4x4_t scale_matrix, transform_matrix;
  fmatrix_4x4_init(&(scale_matrix));

  scale_matrix = fmatrix_4x4_transform_scale(
    &(scale_matrix),
    scale_factors);

  // union fvector3_t quad_position;
  union fvector3_t triangle_position;
  union fvector3_t circle_position;

  for (r = 0; r <= dim; r++) {
    for (c = 0; c <= dim; c += 3) {
      // quad_position = (union fvector3_t) { {grid_start_offset_position.comp.x
      //  + (spacing*c) , grid_start_offset_position.comp.y + (spacing*r), 0.0f}
      //  };
      triangle_position =
        (union fvector3_t){{grid_start_offset_position.comp.x + (spacing * (c + 1)),
                    grid_start_offset_position.comp.y + (spacing * r),
                    0.0f}};
      circle_position =
        (union fvector3_t){{grid_start_offset_position.comp.x + (spacing * (c + 2)),
                    grid_start_offset_position.comp.y + (spacing * r),
                    0.0f}};

      const float val = (color_increment * (float)(r * dim + c));
      const float val2 = (color_increment * (float)(c * dim + r));

      // const union fvector4_t quad_color     = { val, val2, 0.0f, 1.0f  };
      const union fvector4_t triangle_color = {0.0f, val, val2, 1.0f};
      const union fvector4_t circle_color = {0.0f, val2, val, 1.0f};

      // transform_matrix = fmatrix_4x4_transform_translate(&scale_matrix,
      //  quad_position);
      // renderer_2d_draw_quad(renderer, &transform_matrix, quad_color);
      transform_matrix = fmatrix_4x4_transform_translate(
        &(scale_matrix),
        triangle_position);
      renderer_2d_draw_triangle(
        &(window2_renderer_2d),
        &(transform_matrix),
        triangle_color);
      transform_matrix = fmatrix_4x4_transform_translate(
        &(scale_matrix),
        circle_position);
      renderer_2d_draw_circle(
        &(window2_renderer_2d),
        &(transform_matrix),
        circle_color);
    }
  }

  // Line

  const union fvector3_t line_position_1 = {1000.0f, 400.0f, 0.0f};
  const union fvector3_t line_position_2 = {850.0f, 100.0f, 0.0f};
  const union fvector4_t line_color = {127.0f, 33.0f, 127.0f, 1.0f};

  renderer_2d_draw_line(
    &(window2_renderer_2d),
    line_position_1,
    line_position_2,
    line_color);

  // String of text
  const union fvector3_t text_start_position = {100.0f, 900.0f, 0.0f};

  renderer_2d_draw_string(
    &(window2_renderer_2d),
    &(window2_typeface),
    text_start_position,
    _FONT_LOADED_GLYPHS_STRING);
}

/*******************************************************************************
 *
 *   Function: test_app_window_1_init()
 *
 ******************************************************************************/
static uint8_t test_app_window_1_init(
  struct application_t* inout_app,
  struct window_t* out_window1)
{
  uint8_t error = 0U;

  const uint32_t width = 600;
  const uint32_t height = 400;
  const uint8_t num_layers = 1;

  error |=
    window_init(
      out_window1,
      num_layers,
      width,
      height,
      "JotunnWindow1",
      inout_app);

  if (!error) {
    font_typeface_init(
      &(window1_typeface),
      "/usr/share/fonts/noto/NotoSerif-Regular.ttf",
      18);

    char* aaron_shakespeare_texture_path =
      realpath(
        "../../res/textures/AaronShakespeare.png",
        NULL);

    printf(
      "aaron_shakespeare image path resolved to: '%s'\n",
      aaron_shakespeare_texture_path);

    char* lenna_texture_path = realpath(
      "../../res/textures/lenna.png",
      NULL);

    printf(
      "lenna image path resolved to: '%s'\n",
      lenna_texture_path);

    texture_2d_create_from_file_path(
      &(aaron_shakespeare_texture),
      aaron_shakespeare_texture_path,
      1);
    texture_2d_create_from_file_path(
      &(lenna_texture),
      lenna_texture_path,
      1);

    if (aaron_shakespeare_texture_path != NULL) {
      free(aaron_shakespeare_texture_path);
    }

    if (lenna_texture_path != NULL) {
      free(lenna_texture_path);
    }

    window_set_context(out_window1);

    // Camera stuff
    const union fvector3_t camera_position = (union fvector3_t){{0.0f, 0.0f, 2.0f}};
    const union fvector3_t camera_up = (union fvector3_t){{0.0f, 1.0f, 0.0f}};
    const union fvector3_t camera_front = (union fvector3_t){{0.0f, 0.0f, -1.0f}};

    const float ortho_left = 0.0f;
    const float ortho_right = (float)width;
    const float ortho_top = (float)height;
    const float ortho_bottom = 0.0f;
    const float ortho_near_plane = -3.0f;
    const float ortho_far_plane = 100.0f;

    camera_orthographic_init(
      &(window1_camera_ortho),
      camera_position,
      camera_up,
      camera_front);

    camera_orthographic_set_projection(
      &(window1_camera_ortho),
      ortho_left,
      ortho_right,
      ortho_top,
      ortho_bottom,
      ortho_near_plane,
      ortho_far_plane);

    framebuffer_init(
      &(window1_framebuffer),
      width,
      height);

    renderer_2d_init(
      &(window1_renderer_2d),
      &(window1_camera_ortho.base),
      out_window1,
      "window1_renderer_2d");

    window_layer_init(
      out_window1,
      &(window1_layer),
      &(window1_framebuffer),
      &(window1_camera_ortho.base),
      &(window1_renderer_2d.base));

    window_add_layer(
      out_window1,
      &(window1_layer));

    union fvector4_t window1_background_color;
    fvector4_set(
      &(window1_background_color),
      0.3f,
      0.3f,
      0.3f,
      1.0f);
    // fvector4_set(&window1_background_color, 1.0f, 1.0f, 1.0f, 1.0f);

    window_set_background_color(
      out_window1,
      window1_background_color);
  }

  return error;
}

/*******************************************************************************
 *
 *   Function: test_app_window_2_init()
 *
 ******************************************************************************/
static uint8_t test_app_window_2_init(
  struct application_t* inout_app,
  struct window_t* out_window2)
{
  uint8_t error = 0U;

  const uint32_t width = 200;
  const uint32_t height = 200;
  const uint8_t num_layers = 1;

  error |= window_init(
    out_window2,
    num_layers,
    width,
    height,
    "JotunnWindow2",
    inout_app);

  if (!error) {
    font_typeface_init(
      &(window2_typeface),
      "/usr/share/fonts/noto/NotoSerif-Regular.ttf",
      18);

    window_set_context(out_window2);

    // Camera stuff
    const union fvector3_t camera_position = (union fvector3_t){{0.0f, 0.0f, 2.0f}};
    const union fvector3_t camera_up = (union fvector3_t){{0.0f, 1.0f, 0.0f}};
    const union fvector3_t camera_front = (union fvector3_t){{0.0f, 0.0f, -1.0f}};

    const float ortho_left = 0.0f;
    const float ortho_right = (float)width;
    const float ortho_top = (float)height;
    const float ortho_bottom = 0.0f;
    const float ortho_near_plane = -3.0f;
    const float ortho_far_plane = 100.0f;

    camera_orthographic_init(
      &(window2_camera_ortho),
      camera_position,
      camera_up,
      camera_front);

    camera_orthographic_set_projection(
      &(window2_camera_ortho),
      ortho_left,
      ortho_right,
      ortho_top,
      ortho_bottom,
      ortho_near_plane,
      ortho_far_plane);

    framebuffer_init(
      &(window2_framebuffer),
      width,
      height);

    renderer_2d_init(
      &(window2_renderer_2d),
      &(window2_camera_ortho.base),
      out_window2,
      "window2_renderer_2d");

    window_layer_init(
      out_window2,
      &(window2_layer),
      &(window2_framebuffer),
      &(window2_camera_ortho.base),
      &(window2_renderer_2d.base));

    window_add_layer(
      out_window2,
      &(window2_layer));

    union fvector4_t window2_background_color;
    fvector4_set(
      &(window2_background_color),
      0.3f,
      0.3f,
      0.3f,
      1.0f);

    window_set_background_color(
      out_window2,
      window2_background_color);
  }

  return error;
}
