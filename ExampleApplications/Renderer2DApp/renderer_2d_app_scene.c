#include "renderer_2d_app_scene.h"
#include "renderer_2d_app.h"

#include "util.h"

// Where the rows sit, by their centers, and the size of what's in them
#define RENDERER_2D_APP_SCENE_PRIMITIVE_ROW_Y_PIXELS 520.0f
#define RENDERER_2D_APP_SCENE_PRIMITIVE_SIZE_PIXELS 90.0f
#define RENDERER_2D_APP_SCENE_TEXTURE_ROW_Y_PIXELS 210.0f
#define RENDERER_2D_APP_SCENE_TEXTURE_SIZE_PIXELS 220.0f

// How far each label sits below what it labels
#define RENDERER_2D_APP_SCENE_LABEL_GAP_PIXELS 12.0f

// The bottom edges of each row's contents, which labels hang from
#define RENDERER_2D_APP_SCENE_PRIMITIVE_BOTTOM_Y_PIXELS \
  (RENDERER_2D_APP_SCENE_PRIMITIVE_ROW_Y_PIXELS -       \
   (RENDERER_2D_APP_SCENE_PRIMITIVE_SIZE_PIXELS / 2.0f))
#define RENDERER_2D_APP_SCENE_TEXTURE_BOTTOM_Y_PIXELS \
  (RENDERER_2D_APP_SCENE_TEXTURE_ROW_Y_PIXELS -       \
   (RENDERER_2D_APP_SCENE_TEXTURE_SIZE_PIXELS / 2.0f))

// The pivot each primitive is placed and turned by: its center before its
//  transform. The triangle and quads span the unit square from the origin,
//  and the circle is the unit circle about it
static const union fvector2_t renderer_2d_app_scene_box_pivot = {{0.5f, 0.5f}};
static const union fvector2_t renderer_2d_app_scene_circle_pivot = {{0.0f, 0.0f}};

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Draws the scene's title along the top of the window
//
//  in/out: inout_app
static void renderer_2d_app_scene_draw_title(
  struct renderer_2d_app_t* inout_app);

// Draws a filled triangle, centered on in_center_x_pixels in the primitive
//  row, with renderer_2d_draw_triangle()
//
//  in:     in_center_x_pixels
//  in/out: inout_app
static void renderer_2d_app_scene_draw_triangle(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels);

// Draws a filled quad, centered on in_center_x_pixels in the primitive row,
//  with renderer_2d_draw_quad()
//
//  in:     in_center_x_pixels
//  in/out: inout_app
static void renderer_2d_app_scene_draw_quad(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels);

// Draws a quad's outline, centered on in_center_x_pixels in the primitive
//  row, with renderer_2d_draw_unfilled_quad()
//
//  in:     in_center_x_pixels
//  in/out: inout_app
static void renderer_2d_app_scene_draw_unfilled_quad(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels);

// Draws a filled circle, centered on in_center_x_pixels in the primitive
//  row, with renderer_2d_draw_circle()
//
//  in:     in_center_x_pixels
//  in/out: inout_app
static void renderer_2d_app_scene_draw_circle(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels);

// Draws a diagonal line, centered on in_center_x_pixels in the primitive
//  row, with renderer_2d_draw_line(), which takes its ends rather than a
//  transform
//
//  in:     in_center_x_pixels
//  in/out: inout_app
static void renderer_2d_app_scene_draw_line(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels);

// Draws a quad spinning one way inside an outline spinning the other,
//  centered on in_center_x_pixels in the primitive row, turned by the
//  application's clock
//
//  in:     in_center_x_pixels
//  in/out: inout_app
static void renderer_2d_app_scene_draw_spinner(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels);

// Draws the image loaded from a file, keeping its aspect ratio, centered on
//  in_center_x_pixels in the texture row, with
//  renderer_2d_draw_textured_quad()
//
//  in:     in_center_x_pixels
//  in/out: inout_app
static void renderer_2d_app_scene_draw_image(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels);

// Draws the middle of the image, centered on in_center_x_pixels in the
//  texture row, with renderer_2d_draw_subtextured_quad()
//
//  in:     in_center_x_pixels
//  in/out: inout_app
static void renderer_2d_app_scene_draw_image_part(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels);

// Draws the texture made in code, centered on in_center_x_pixels in the
//  texture row, with renderer_2d_draw_textured_quad()
//
//  in:     in_center_x_pixels
//  in/out: inout_app
static void renderer_2d_app_scene_draw_checker(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels);

// Draws a label centered under something, its top RENDERER_2D_APP_SCENE_
//  LABEL_GAP_PIXELS below that thing's bottom edge, with
//  renderer_2d_draw_string_aligned()
//
//  in:     in_text, in_center_x_pixels, in_bottom_y_pixels
//  in/out: inout_app
static void renderer_2d_app_scene_draw_label(
  struct renderer_2d_app_t* inout_app,
  const char* in_text,
  float in_center_x_pixels,
  float in_bottom_y_pixels);

/*******************************************************************************
 *
 *   Function: renderer_2d_app_scene_run()
 *
 ******************************************************************************/
void renderer_2d_app_scene_run(
  struct window_layer_t* inout_window_layer)
{
  struct renderer_2d_app_t* app =
    container_of(
      inout_window_layer,
      struct renderer_2d_app_t,
      scene_layer.window_layer);

  renderer_2d_app_scene_draw_title(app);

  // renderer_2d's primitives, along the top row
  renderer_2d_app_scene_draw_triangle(
    app,
    100.0f);
  renderer_2d_app_scene_draw_quad(
    app,
    240.0f);
  renderer_2d_app_scene_draw_unfilled_quad(
    app,
    380.0f);
  renderer_2d_app_scene_draw_circle(
    app,
    520.0f);
  renderer_2d_app_scene_draw_line(
    app,
    660.0f);
  renderer_2d_app_scene_draw_spinner(
    app,
    880.0f);

  // Textures, along the bottom row
  renderer_2d_app_scene_draw_image(
    app,
    170.0f);
  renderer_2d_app_scene_draw_image_part(
    app,
    480.0f);
  renderer_2d_app_scene_draw_checker(
    app,
    800.0f);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_scene_draw_title()
 *
 ******************************************************************************/
static void renderer_2d_app_scene_draw_title(
  struct renderer_2d_app_t* inout_app)
{
  // Strings are placed by their top-left corner
  renderer_2d_draw_string(
    &(inout_app->scene_layer.renderer_2d),
    &(inout_app->resources.typeface),
    (union fvector3_t){{30.0f, 670.0f, 0.0f}},
    "renderer_2d: primitives, textures and text");
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_scene_draw_triangle()
 *
 ******************************************************************************/
static void renderer_2d_app_scene_draw_triangle(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels)
{
  const union fmatrix_4x4_t transform =
    fmatrix_4x4_transform_compose_2d(
      renderer_2d_app_scene_box_pivot,
      (union fvector2_t){{RENDERER_2D_APP_SCENE_PRIMITIVE_SIZE_PIXELS,
                          RENDERER_2D_APP_SCENE_PRIMITIVE_SIZE_PIXELS}},
      0.0f,
      (union fvector2_t){{in_center_x_pixels, RENDERER_2D_APP_SCENE_PRIMITIVE_ROW_Y_PIXELS}});

  renderer_2d_draw_triangle(
    &(inout_app->scene_layer.renderer_2d),
    &(transform),
    (union fvector4_t){{0.95f, 0.45f, 0.35f, 1.0f}});

  renderer_2d_app_scene_draw_label(
    inout_app,
    "triangle",
    in_center_x_pixels,
    RENDERER_2D_APP_SCENE_PRIMITIVE_BOTTOM_Y_PIXELS);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_scene_draw_quad()
 *
 ******************************************************************************/
static void renderer_2d_app_scene_draw_quad(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels)
{
  const union fmatrix_4x4_t transform =
    fmatrix_4x4_transform_compose_2d(
      renderer_2d_app_scene_box_pivot,
      (union fvector2_t){{RENDERER_2D_APP_SCENE_PRIMITIVE_SIZE_PIXELS,
                          RENDERER_2D_APP_SCENE_PRIMITIVE_SIZE_PIXELS}},
      0.0f,
      (union fvector2_t){{in_center_x_pixels, RENDERER_2D_APP_SCENE_PRIMITIVE_ROW_Y_PIXELS}});

  renderer_2d_draw_quad(
    &(inout_app->scene_layer.renderer_2d),
    &(transform),
    (union fvector4_t){{0.35f, 0.75f, 0.45f, 1.0f}});

  renderer_2d_app_scene_draw_label(
    inout_app,
    "quad",
    in_center_x_pixels,
    RENDERER_2D_APP_SCENE_PRIMITIVE_BOTTOM_Y_PIXELS);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_scene_draw_unfilled_quad()
 *
 ******************************************************************************/
static void renderer_2d_app_scene_draw_unfilled_quad(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels)
{
  const union fmatrix_4x4_t transform =
    fmatrix_4x4_transform_compose_2d(
      renderer_2d_app_scene_box_pivot,
      (union fvector2_t){{RENDERER_2D_APP_SCENE_PRIMITIVE_SIZE_PIXELS,
                          RENDERER_2D_APP_SCENE_PRIMITIVE_SIZE_PIXELS}},
      0.0f,
      (union fvector2_t){{in_center_x_pixels, RENDERER_2D_APP_SCENE_PRIMITIVE_ROW_Y_PIXELS}});

  renderer_2d_draw_unfilled_quad(
    &(inout_app->scene_layer.renderer_2d),
    &(transform),
    (union fvector4_t){{0.95f, 0.85f, 0.35f, 1.0f}});

  renderer_2d_app_scene_draw_label(
    inout_app,
    "unfilled quad",
    in_center_x_pixels,
    RENDERER_2D_APP_SCENE_PRIMITIVE_BOTTOM_Y_PIXELS);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_scene_draw_circle()
 *
 ******************************************************************************/
static void renderer_2d_app_scene_draw_circle(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels)
{
  // The unit circle's radius is 1, so it's scaled by the radius wanted
  const float radius_pixels = (RENDERER_2D_APP_SCENE_PRIMITIVE_SIZE_PIXELS / 2.0f);

  const union fmatrix_4x4_t transform =
    fmatrix_4x4_transform_compose_2d(
      renderer_2d_app_scene_circle_pivot,
      (union fvector2_t){{radius_pixels, radius_pixels}},
      0.0f,
      (union fvector2_t){{in_center_x_pixels, RENDERER_2D_APP_SCENE_PRIMITIVE_ROW_Y_PIXELS}});

  renderer_2d_draw_circle(
    &(inout_app->scene_layer.renderer_2d),
    &(transform),
    (union fvector4_t){{0.35f, 0.55f, 0.95f, 1.0f}});

  renderer_2d_app_scene_draw_label(
    inout_app,
    "circle",
    in_center_x_pixels,
    RENDERER_2D_APP_SCENE_PRIMITIVE_BOTTOM_Y_PIXELS);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_scene_draw_line()
 *
 ******************************************************************************/
static void renderer_2d_app_scene_draw_line(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels)
{
  const float half_size_pixels = (RENDERER_2D_APP_SCENE_PRIMITIVE_SIZE_PIXELS / 2.0f);

  renderer_2d_draw_line(
    &(inout_app->scene_layer.renderer_2d),
    (union fvector3_t){{(in_center_x_pixels - half_size_pixels),
                        (RENDERER_2D_APP_SCENE_PRIMITIVE_ROW_Y_PIXELS - half_size_pixels),
                        0.0f}},
    (union fvector3_t){{(in_center_x_pixels + half_size_pixels),
                        (RENDERER_2D_APP_SCENE_PRIMITIVE_ROW_Y_PIXELS + half_size_pixels),
                        0.0f}},
    (union fvector4_t){{0.85f, 0.45f, 0.95f, 1.0f}});

  renderer_2d_app_scene_draw_label(
    inout_app,
    "line",
    in_center_x_pixels,
    RENDERER_2D_APP_SCENE_PRIMITIVE_BOTTOM_Y_PIXELS);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_scene_draw_spinner()
 *
 ******************************************************************************/
static void renderer_2d_app_scene_draw_spinner(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels)
{
  const float quad_size_pixels = 70.0f;
  const float outline_size_pixels = 120.0f;
  const float spin_radians_per_second = 1.2f;

  const float elapsed_seconds = (float)application_get_elapsed_seconds(&(inout_app->application));
  const float spin_radians = (spin_radians_per_second * elapsed_seconds);
  const union fvector2_t center_pixels =
    {{in_center_x_pixels, RENDERER_2D_APP_SCENE_PRIMITIVE_ROW_Y_PIXELS}};

  // Sized so that the two never overlap: where primitives at the same depth
  //  overlap, whichever kind the batch draws first covers the rest, whatever
  //  order they were added in
  const union fmatrix_4x4_t quad_transform =
    fmatrix_4x4_transform_compose_2d(
      renderer_2d_app_scene_box_pivot,
      (union fvector2_t){{quad_size_pixels, quad_size_pixels}},
      spin_radians,
      center_pixels);
  const union fmatrix_4x4_t outline_transform =
    fmatrix_4x4_transform_compose_2d(
      renderer_2d_app_scene_box_pivot,
      (union fvector2_t){{outline_size_pixels, outline_size_pixels}},
      (-0.5f * spin_radians),
      center_pixels);

  renderer_2d_draw_quad(
    &(inout_app->scene_layer.renderer_2d),
    &(quad_transform),
    (union fvector4_t){{0.95f, 0.6f, 0.2f, 1.0f}});
  renderer_2d_draw_unfilled_quad(
    &(inout_app->scene_layer.renderer_2d),
    &(outline_transform),
    (union fvector4_t){{0.95f, 0.95f, 0.95f, 1.0f}});

  // Below the outline's corners, however it's turned
  const float outline_reach_pixels = (outline_size_pixels * 0.7072f);

  renderer_2d_app_scene_draw_label(
    inout_app,
    "rotated",
    in_center_x_pixels,
    (RENDERER_2D_APP_SCENE_PRIMITIVE_ROW_Y_PIXELS - outline_reach_pixels));
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_scene_draw_image()
 *
 ******************************************************************************/
static void renderer_2d_app_scene_draw_image(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels)
{
  const struct texture_2d_t* image = &(inout_app->resources.image_texture);

  const float aspect_ratio = ((float)image->width / (float)image->height);

  const union fmatrix_4x4_t transform =
    fmatrix_4x4_transform_compose_2d(
      renderer_2d_app_scene_box_pivot,
      (union fvector2_t){{(RENDERER_2D_APP_SCENE_TEXTURE_SIZE_PIXELS * aspect_ratio),
                          RENDERER_2D_APP_SCENE_TEXTURE_SIZE_PIXELS}},
      0.0f,
      (union fvector2_t){{in_center_x_pixels, RENDERER_2D_APP_SCENE_TEXTURE_ROW_Y_PIXELS}});

  renderer_2d_draw_textured_quad(
    &(inout_app->scene_layer.renderer_2d),
    &(transform),
    image);

  renderer_2d_app_scene_draw_label(
    inout_app,
    "texture from a file",
    in_center_x_pixels,
    RENDERER_2D_APP_SCENE_TEXTURE_BOTTOM_Y_PIXELS);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_scene_draw_image_part()
 *
 ******************************************************************************/
static void renderer_2d_app_scene_draw_image_part(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels)
{
  const struct texture_2d_t* image = &(inout_app->resources.image_texture);

  // The texture coordinates of each of the quad's corners, from its
  //  bottom-left going counter-clockwise: the middle of the image
  const union fvector2_t subtexture_coordinates[4] = {
    {{0.3f, 0.4f}},
    {{0.7f, 0.4f}},
    {{0.7f, 0.8f}},
    {{0.3f, 0.8f}}};

  const union fmatrix_4x4_t transform =
    fmatrix_4x4_transform_compose_2d(
      renderer_2d_app_scene_box_pivot,
      (union fvector2_t){{RENDERER_2D_APP_SCENE_TEXTURE_SIZE_PIXELS,
                          RENDERER_2D_APP_SCENE_TEXTURE_SIZE_PIXELS}},
      0.0f,
      (union fvector2_t){{in_center_x_pixels, RENDERER_2D_APP_SCENE_TEXTURE_ROW_Y_PIXELS}});

  renderer_2d_draw_subtextured_quad(
    &(inout_app->scene_layer.renderer_2d),
    &(transform),
    image,
    subtexture_coordinates);

  renderer_2d_app_scene_draw_label(
    inout_app,
    "part of a texture",
    in_center_x_pixels,
    RENDERER_2D_APP_SCENE_TEXTURE_BOTTOM_Y_PIXELS);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_scene_draw_checker()
 *
 ******************************************************************************/
static void renderer_2d_app_scene_draw_checker(
  struct renderer_2d_app_t* inout_app,
  float in_center_x_pixels)
{
  const union fmatrix_4x4_t transform =
    fmatrix_4x4_transform_compose_2d(
      renderer_2d_app_scene_box_pivot,
      (union fvector2_t){{RENDERER_2D_APP_SCENE_TEXTURE_SIZE_PIXELS,
                          RENDERER_2D_APP_SCENE_TEXTURE_SIZE_PIXELS}},
      0.0f,
      (union fvector2_t){{in_center_x_pixels, RENDERER_2D_APP_SCENE_TEXTURE_ROW_Y_PIXELS}});

  renderer_2d_draw_textured_quad(
    &(inout_app->scene_layer.renderer_2d),
    &(transform),
    &(inout_app->resources.checker_texture));

  renderer_2d_app_scene_draw_label(
    inout_app,
    "texture made in code",
    in_center_x_pixels,
    RENDERER_2D_APP_SCENE_TEXTURE_BOTTOM_Y_PIXELS);
}

/*******************************************************************************
 *
 *   Function: renderer_2d_app_scene_draw_label()
 *
 ******************************************************************************/
static void renderer_2d_app_scene_draw_label(
  struct renderer_2d_app_t* inout_app,
  const char* in_text,
  float in_center_x_pixels,
  float in_bottom_y_pixels)
{
  // Placed by the middle of its top edge
  const union fvector3_t text_position =
    {{in_center_x_pixels,
      (in_bottom_y_pixels - RENDERER_2D_APP_SCENE_LABEL_GAP_PIXELS),
      0.0f}};

  renderer_2d_draw_string_aligned(
    &(inout_app->scene_layer.renderer_2d),
    &(inout_app->resources.typeface),
    text_position,
    RENDERER_2D_TEXT_ALIGNMENT_CENTER,
    in_text);
}
