#include "ui_element.h"

#include "renderer_2d.h"
#include "string.h"
#include "ui_layer.h"

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Draws a static text element's text at the top of the space allocated to it,
//  placed across it as the element's centering asks. in_theme is unused
//
//  in:     in_ui_element, in_origin_position, in_allocated_space, in_theme
//  in/out: inout_renderer_2d
static void ui_element_static_text_render(
  struct renderer_2d_t* inout_renderer_2d,
  const struct ui_element_t* in_ui_element,
  union fvector2_t in_origin_position,
  union fvector2_t in_allocated_space,
  const struct ui_theme_t* in_theme);

// Builds a transform from a scale and a translation
//
//  in:     in_scale_factors, in_translation_vector
//  ret:    The transform
static union fmatrix_4x4_t ui_element_get_transform(
  union fvector3_t in_scale_factors,
  union fvector3_t in_translation_vector);

// Draws a slider at the top left of the space allocated to it: a box in the
//  theme's background color, a line across it in the foreground color, and a
//  mark at the slider's position in the accent color. in_allocated_space is
//  unused
//
//  in:     in_ui_element, in_origin_position, in_allocated_space, in_theme
//  in/out: inout_renderer_2d
static void ui_element_slider_render(
  struct renderer_2d_t* inout_renderer_2d,
  const struct ui_element_t* in_ui_element,
  union fvector2_t in_origin_position,
  union fvector2_t in_allocated_space,
  const struct ui_theme_t* in_theme);

/*******************************************************************************
 *
 *   Function: ui_element_get_effective_size()
 *
 ******************************************************************************/
union fvector2_t ui_element_get_effective_size(
  const struct ui_element_t* in_ui_element)
{
  const union fvector2_t effective_size =
    (union fvector2_t){.comp.x = (in_ui_element->width + 2 * in_ui_element->padding_x),
               .comp.y = (in_ui_element->height + 2 * in_ui_element->padding_y)};

  return effective_size;
}

/*******************************************************************************
 *
 *   Function: ui_element_init()
 *
 ******************************************************************************/
void ui_element_init(
  struct ui_element_t* out_ui_element,
  enum ui_element_type_t in_element_type,
  uint16_t in_width,
  uint16_t in_height,
  void (*in_function_ui_element_render)(
    struct renderer_2d_t* inout_renderer_2d,
    const struct ui_element_t* in_ui_element,
    union fvector2_t in_origin_position,
    union fvector2_t in_allocated_space,
    const struct ui_theme_t* in_theme),
  void (*in_function_ui_element_event_react)(
    struct ui_element_t* inout_ui_element,
    struct event_base_t* inout_event))
{
  out_ui_element->element_type = in_element_type;

  out_ui_element->element_centering = UI_ELEMENT_CENTERING_LEFT;

  out_ui_element->width = in_width;
  out_ui_element->height = in_height;

  out_ui_element->padding_x = 0;
  out_ui_element->padding_y = 0;

  if (in_function_ui_element_render) {
    out_ui_element->function_ui_element_render = in_function_ui_element_render;
  }

  if (in_function_ui_element_event_react) {
    out_ui_element->function_ui_element_event_react = in_function_ui_element_event_react;
  }
}

/*******************************************************************************
 *
 *   Function: ui_element_static_text_init()
 *
 ******************************************************************************/
void ui_element_static_text_init(
  struct ui_element_static_text_t* out_text_element,
  const char* in_static_text,
  const struct font_typeface_t* in_typeface)
{
  int text_length = strlen(in_static_text);
  union fvector2_t text_dimensions =
    font_typeface_calculate_string_dimensions(
      in_typeface,
      in_static_text,
      text_length);

  ui_element_init(
    &(out_text_element->base_element),
    UI_ELEMENT_TYPE_STATIC_TEXT,
    text_dimensions.comp.x,
    text_dimensions.comp.y,
    ui_element_static_text_render,
    (void*)0);

  out_text_element->static_text = in_static_text;
  out_text_element->typeface = in_typeface;

  out_text_element->base_element.padding_x = 3;
  out_text_element->base_element.padding_y = 8;
}

/*******************************************************************************
 *
 *   Function: ui_element_slider_init()
 *
 ******************************************************************************/
void ui_element_slider_init(
  struct ui_element_slider_t* out_slider_element,
  float in_starting_position,
  float in_lower_bound,
  float in_upper_bound)
{
  ui_element_init(
    &(out_slider_element->base_element),
    UI_ELEMENT_TYPE_SLIDER,
    200,
    30,
    ui_element_slider_render,
    (void*)0);

  out_slider_element->slider_position = in_starting_position;

  out_slider_element->lower_bound = in_lower_bound;
  out_slider_element->upper_bound = in_upper_bound;

  out_slider_element->base_element.padding_x = 0;
  out_slider_element->base_element.padding_y = 0;
}

/*******************************************************************************
 *
 *   Function: ui_element_button_init()
 *
 ******************************************************************************/
void ui_element_button_init(
  struct ui_element_button_t* out_button_element)
{
  ui_element_init(
    &(out_button_element->base_element),
    UI_ELEMENT_TYPE_BUTTON,
    100,
    100,
    (void*)0,
    (void*)0);
}

/*******************************************************************************
 *
 *   Function: ui_element_checkbox_init()
 *
 ******************************************************************************/
void ui_element_checkbox_init(
  struct ui_element_checkbox_t* out_checkbox_element,
  bool in_is_checked)
{
  ui_element_init(
    &(out_checkbox_element->base_element),
    UI_ELEMENT_TYPE_CHECKBOX,
    100,
    100,
    (void*)0,
    (void*)0);
}

/*******************************************************************************
 *
 *   Function: ui_element_static_text_render()
 *
 ******************************************************************************/
static void ui_element_static_text_render(
  struct renderer_2d_t* inout_renderer_2d,
  const struct ui_element_t* in_ui_element,
  union fvector2_t in_origin_position,
  union fvector2_t in_allocated_space,
  const struct ui_theme_t* in_theme)
{
  const struct ui_element_static_text_t* static_text_element =
    (const struct ui_element_static_text_t*)in_ui_element;

  union fvector2_t effective_element_size = ui_element_get_effective_size(in_ui_element);

  switch (in_ui_element->element_centering)
  {
    case UI_ELEMENT_CENTERING_LEFT:
    {
      renderer_2d_draw_string(
        inout_renderer_2d,
        static_text_element->typeface,
        (union fvector3_t){in_origin_position.comp.x + in_ui_element->padding_x,
                   in_origin_position.comp.y - in_ui_element->padding_y,
                   0.1f},
        static_text_element->static_text);
    }
    break;
    case UI_ELEMENT_CENTERING_RIGHT:
    {
      renderer_2d_draw_string(
        inout_renderer_2d,
        static_text_element->typeface,
        (union fvector3_t){in_origin_position.comp.x + in_ui_element->padding_x +
                     in_allocated_space.comp.x - effective_element_size.comp.x,
                   in_origin_position.comp.y - in_ui_element->padding_y,
                   0.1f},
        static_text_element->static_text);
    }
    break;
    case UI_ELEMENT_CENTERING_CENTER:
    {
      renderer_2d_draw_string(
        inout_renderer_2d,
        static_text_element->typeface,
        (union fvector3_t){in_origin_position.comp.x + in_ui_element->padding_x +
                     0.5f *
                       (in_allocated_space.comp.x - effective_element_size.comp.x),
                   in_origin_position.comp.y - in_ui_element->padding_y,
                   0.1f},
        static_text_element->static_text);
    }
    break;
  }
}

/*******************************************************************************
 *
 *   Function: ui_element_get_transform()
 *
 ******************************************************************************/
static union fmatrix_4x4_t ui_element_get_transform(
  union fvector3_t in_scale_factors,
  union fvector3_t in_translation_vector)
{
  union fmatrix_4x4_t transform_matrix;
  {
    union fmatrix_4x4_t scale_matrix, translation_matrix;

    fmatrix_4x4_init(&(scale_matrix));
    fmatrix_4x4_init(&(translation_matrix));
    fmatrix_4x4_init(&(transform_matrix));

    scale_matrix = fmatrix_4x4_transform_scale(
      &(scale_matrix),
      in_scale_factors);
    translation_matrix =
      fmatrix_4x4_transform_translate(
        &(translation_matrix),
        in_translation_vector);
    transform_matrix = fmatrix_4x4_multiply(
      &(scale_matrix),
      &(translation_matrix));
  }
  return transform_matrix;
}

/*******************************************************************************
 *
 *   Function: ui_element_slider_render()
 *
 ******************************************************************************/
static void ui_element_slider_render(
  struct renderer_2d_t* inout_renderer_2d,
  const struct ui_element_t* in_ui_element,
  union fvector2_t in_origin_position,
  union fvector2_t in_allocated_space,
  const struct ui_theme_t* in_theme)
{
  const struct ui_element_slider_t* slider_element =
    (const struct ui_element_slider_t*)in_ui_element;

  float slider_range = (slider_element->upper_bound - slider_element->lower_bound);
  float slider_percentage =
    ((slider_element->slider_position - slider_element->lower_bound) /
     slider_range);

  uint16_t slider_margin = 10U;

  const union fvector3_t box_scale_factors =
    (union fvector3_t){in_ui_element->width, in_ui_element->height, 1.0f};

  const union fvector3_t box_translation_vector = (union fvector3_t){
    in_origin_position.comp.x + in_ui_element->padding_x,
    in_origin_position.comp.y - (in_ui_element->height + in_ui_element->padding_y),
    0.1f};

  union fmatrix_4x4_t box_transform =
    ui_element_get_transform(
      box_scale_factors,
      box_translation_vector);

  renderer_2d_draw_quad(
    inout_renderer_2d,
    &(box_transform),
    in_theme->background_color);

  const union fvector3_t line_point1 = (union fvector3_t){
    in_origin_position.comp.x + in_ui_element->padding_x + slider_margin,
    in_origin_position.comp.y - ((in_ui_element->height / 2) + in_ui_element->padding_y),
    0.2f};

  const union fvector3_t line_point2 = (union fvector3_t){
    in_origin_position.comp.x + in_ui_element->width + in_ui_element->padding_x -
      slider_margin,
    in_origin_position.comp.y - ((in_ui_element->height / 2) + in_ui_element->padding_y),
    0.2f};

  renderer_2d_draw_line(
    inout_renderer_2d,
    line_point1,
    line_point2,
    in_theme->foreground_color);

  uint16_t slider_x =
    (in_ui_element->width - 2 * (in_ui_element->padding_x + slider_margin)) *
    slider_percentage;

  const union fvector3_t slider_point1 = (union fvector3_t){
    in_origin_position.comp.x + in_ui_element->padding_x + slider_x,
    in_origin_position.comp.y -
      ((in_ui_element->height / 2) + in_ui_element->padding_y) + slider_margin,
    0.3f};

  const union fvector3_t slider_point2 =
    (union fvector3_t)
      {
        in_origin_position.comp.x + in_ui_element->padding_x + slider_x,
        in_origin_position.comp.y -
          ((in_ui_element->height / 2) + in_ui_element->padding_y) - slider_margin,
        0.3f
      };

  renderer_2d_draw_line(
    inout_renderer_2d,
    slider_point1,
    slider_point2,
    in_theme->accent_color);
}
