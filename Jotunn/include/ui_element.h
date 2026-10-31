#pragma once

// Retained UI elements drawn through a renderer_2d: static text, sliders,
//  buttons and checkboxes, each embedding a ui_element_t as its first member.
//  Unfinished and not built: struct ui_theme_t, which the render functions
//  take their colors from, isn't defined yet, and only static text and
//  sliders draw anything

struct event_base_t;
struct renderer_2d_t;
struct font_typeface_t;
struct ui_layer_t;
struct ui_theme_t;

#include "fvector.h"

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 *
 *   ELEMENTS
 *
 ******************************************************************************/

// Where an element sits within the space allocated to it, across its width
enum ui_element_centering_t
{
  UI_ELEMENT_CENTERING_LEFT,
  UI_ELEMENT_CENTERING_RIGHT,
  UI_ELEMENT_CENTERING_CENTER
};

// Types implementing a "UI element"
enum ui_element_type_t
{
  UI_ELEMENT_TYPE_STATIC_TEXT,
  UI_ELEMENT_TYPE_SLIDER,
  UI_ELEMENT_TYPE_BUTTON,
  UI_ELEMENT_TYPE_CHECKBOX,
  UI_ELEMENT_TYPE_DROPDOWN,
  UI_ELEMENT_TYPE_GRAPH,
  UI_ELEMENT_TYPE_INPUT_BOX
};

// The core structure for UI elements
struct ui_element_t
{
  // Type that this UI element is implementing
  enum ui_element_type_t element_type;

  // Centering of this element within its allocated container space
  enum ui_element_centering_t element_centering;

  // Total width and height of all sub-shapes composing the UI element
  uint16_t width;
  uint16_t height;

  uint16_t padding_x;
  uint16_t padding_y;

  void (*function_ui_element_render)(
    struct renderer_2d_t* inout_renderer_2d,
    const struct ui_element_t* in_ui_element,
    union fvector2_t in_origin_position,
    union fvector2_t in_allocated_space,
    const struct ui_theme_t* in_theme);

  void (*function_ui_element_event_react)(
    struct ui_element_t* inout_ui_element,
    struct event_base_t* inout_event);
};

/*******************************************************************************
 *
 *   IMPLEMENTATIONS
 *
 ******************************************************************************/

struct ui_element_static_text_t
{
  struct ui_element_t base_element;
  const char* static_text;
  const struct font_typeface_t* typeface;
};

struct ui_element_slider_t
{
  struct ui_element_t base_element;

  float slider_position;

  float lower_bound;
  float upper_bound;
};

struct ui_element_button_t
{
  struct ui_element_t base_element;
  bool is_pressed;
};

struct ui_element_checkbox_t
{
  struct ui_element_t base_element;
  bool is_checked;
};

/*******************************************************************************
 *
 *   COMMON
 *
 ******************************************************************************/

// Measures an element with its padding on both sides
//
//  in:     in_ui_element
//  ret:    The element's width and height, padding included
union fvector2_t ui_element_get_effective_size(
  const struct ui_element_t* in_ui_element);

// Sets up the part common to every element: its type and size, centered to
//  the left without padding. A NULL function leaves its field as it was, so
//  zero out_ui_element first when passing one
//
//  in:     in_element_type, in_width, in_height,
//            in_function_ui_element_render,
//            in_function_ui_element_event_react
//  out:    out_ui_element
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
    struct event_base_t* inout_event));

// Sets up a static text element, sized to fit the text in the typeface, with
//  padding of 3 across and 8 down. The element keeps both pointers, so the
//  text and typeface must outlive it. It reacts to no events
//
//  in:     in_static_text, in_typeface
//  out:    out_text_element
void ui_element_static_text_init(
  struct ui_element_static_text_t* out_text_element,
  const char* in_static_text,
  const struct font_typeface_t* in_typeface);

// Sets up a 200 by 30 slider between two bounds, without padding. It reacts
//  to no events yet, so stays where it starts
//
//  in:     in_starting_position, in_lower_bound, in_upper_bound
//  out:    out_slider_element
void ui_element_slider_init(
  struct ui_element_slider_t* out_slider_element,
  float in_starting_position,
  float in_lower_bound,
  float in_upper_bound);

// Sets up a 100 by 100 button. It has neither a render nor an event function
//  yet, so draws nothing and is never pressed
//
//  out:    out_button_element
void ui_element_button_init(
  struct ui_element_button_t* out_button_element);

// Sets up a 100 by 100 checkbox. It has neither a render nor an event
//  function yet, and in_is_checked is not yet applied: is_checked is left as
//  it was
//
//  in:     in_is_checked
//  out:    out_checkbox_element
void ui_element_checkbox_init(
  struct ui_element_checkbox_t* out_checkbox_element,
  bool in_is_checked);
