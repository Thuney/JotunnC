#pragma once

// The events windows and the application raise. Each event's struct begins
//  with a struct event_base_t, whose event_type says which struct it is, so
//  handlers take a struct event_base_t* and cast it once they've checked the
//  type. Events from a window carry its window_handle, the same as the
//  window's context_data.window_handle, to tell which window they came from

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 *
 *   EVENT TYPES
 *
 ******************************************************************************/

// What an event is, which says which struct it's in
enum event_type_t
{
  EVENT_TYPE_WINDOW_CLOSE = 0x00,
  EVENT_TYPE_WINDOW_RESIZE,
  EVENT_TYPE_WINDOW_FOCUS,
  EVENT_TYPE_WINDOW_LOST_FOCUS,
  EVENT_TYPE_WINDOW_MOVED,

  EVENT_TYPE_APP_TICK,
  EVENT_TYPE_APP_UPDATE,
  EVENT_TYPE_APP_RENDER,

  EVENT_TYPE_KEY_PRESSED,
  EVENT_TYPE_KEY_RELEASED,
  EVENT_TYPE_KEY_TYPED,

  EVENT_TYPE_MOUSE_BUTTON,
  EVENT_TYPE_MOUSE_MOVED,
  EVENT_TYPE_MOUSE_SCROLLED,
  EVENT_TYPE_MOUSE_ENTERED,

  EVENT_TYPE_COUNT
};

/*******************************************************************************
 *
 *   KEYS
 *
 ******************************************************************************/

// Physical keys, independent of the windowing backend. Laid out in groups
//  that stay contiguous (digits, letters, function keys and keypad digits) so
//  that they can be translated by offset
enum event_key_code_t
{
  EVENT_KEY_CODE_UNKNOWN = 0x00,
  EVENT_KEY_CODE_TAB,
  EVENT_KEY_CODE_LEFT_ARROW,
  EVENT_KEY_CODE_RIGHT_ARROW,
  EVENT_KEY_CODE_UP_ARROW,
  EVENT_KEY_CODE_DOWN_ARROW,
  EVENT_KEY_CODE_PAGE_UP,
  EVENT_KEY_CODE_PAGE_DOWN,
  EVENT_KEY_CODE_HOME,
  EVENT_KEY_CODE_END,
  EVENT_KEY_CODE_INSERT,
  EVENT_KEY_CODE_DELETE,
  EVENT_KEY_CODE_BACKSPACE,
  EVENT_KEY_CODE_SPACE,
  EVENT_KEY_CODE_ENTER,
  EVENT_KEY_CODE_ESCAPE,
  EVENT_KEY_CODE_LEFT_CONTROL,
  EVENT_KEY_CODE_LEFT_SHIFT,
  EVENT_KEY_CODE_LEFT_ALT,
  EVENT_KEY_CODE_LEFT_SUPER,
  EVENT_KEY_CODE_RIGHT_CONTROL,
  EVENT_KEY_CODE_RIGHT_SHIFT,
  EVENT_KEY_CODE_RIGHT_ALT,
  EVENT_KEY_CODE_RIGHT_SUPER,
  EVENT_KEY_CODE_MENU,
  EVENT_KEY_CODE_0,
  EVENT_KEY_CODE_1,
  EVENT_KEY_CODE_2,
  EVENT_KEY_CODE_3,
  EVENT_KEY_CODE_4,
  EVENT_KEY_CODE_5,
  EVENT_KEY_CODE_6,
  EVENT_KEY_CODE_7,
  EVENT_KEY_CODE_8,
  EVENT_KEY_CODE_9,
  EVENT_KEY_CODE_A,
  EVENT_KEY_CODE_B,
  EVENT_KEY_CODE_C,
  EVENT_KEY_CODE_D,
  EVENT_KEY_CODE_E,
  EVENT_KEY_CODE_F,
  EVENT_KEY_CODE_G,
  EVENT_KEY_CODE_H,
  EVENT_KEY_CODE_I,
  EVENT_KEY_CODE_J,
  EVENT_KEY_CODE_K,
  EVENT_KEY_CODE_L,
  EVENT_KEY_CODE_M,
  EVENT_KEY_CODE_N,
  EVENT_KEY_CODE_O,
  EVENT_KEY_CODE_P,
  EVENT_KEY_CODE_Q,
  EVENT_KEY_CODE_R,
  EVENT_KEY_CODE_S,
  EVENT_KEY_CODE_T,
  EVENT_KEY_CODE_U,
  EVENT_KEY_CODE_V,
  EVENT_KEY_CODE_W,
  EVENT_KEY_CODE_X,
  EVENT_KEY_CODE_Y,
  EVENT_KEY_CODE_Z,
  EVENT_KEY_CODE_F1,
  EVENT_KEY_CODE_F2,
  EVENT_KEY_CODE_F3,
  EVENT_KEY_CODE_F4,
  EVENT_KEY_CODE_F5,
  EVENT_KEY_CODE_F6,
  EVENT_KEY_CODE_F7,
  EVENT_KEY_CODE_F8,
  EVENT_KEY_CODE_F9,
  EVENT_KEY_CODE_F10,
  EVENT_KEY_CODE_F11,
  EVENT_KEY_CODE_F12,
  EVENT_KEY_CODE_F13,
  EVENT_KEY_CODE_F14,
  EVENT_KEY_CODE_F15,
  EVENT_KEY_CODE_F16,
  EVENT_KEY_CODE_F17,
  EVENT_KEY_CODE_F18,
  EVENT_KEY_CODE_F19,
  EVENT_KEY_CODE_F20,
  EVENT_KEY_CODE_F21,
  EVENT_KEY_CODE_F22,
  EVENT_KEY_CODE_F23,
  EVENT_KEY_CODE_F24,
  EVENT_KEY_CODE_APOSTROPHE,
  EVENT_KEY_CODE_COMMA,
  EVENT_KEY_CODE_MINUS,
  EVENT_KEY_CODE_PERIOD,
  EVENT_KEY_CODE_SLASH,
  EVENT_KEY_CODE_SEMICOLON,
  EVENT_KEY_CODE_EQUAL,
  EVENT_KEY_CODE_LEFT_BRACKET,
  EVENT_KEY_CODE_BACKSLASH,
  EVENT_KEY_CODE_RIGHT_BRACKET,
  EVENT_KEY_CODE_GRAVE_ACCENT,
  EVENT_KEY_CODE_CAPS_LOCK,
  EVENT_KEY_CODE_SCROLL_LOCK,
  EVENT_KEY_CODE_NUM_LOCK,
  EVENT_KEY_CODE_PRINT_SCREEN,
  EVENT_KEY_CODE_PAUSE,
  EVENT_KEY_CODE_KEYPAD_0,
  EVENT_KEY_CODE_KEYPAD_1,
  EVENT_KEY_CODE_KEYPAD_2,
  EVENT_KEY_CODE_KEYPAD_3,
  EVENT_KEY_CODE_KEYPAD_4,
  EVENT_KEY_CODE_KEYPAD_5,
  EVENT_KEY_CODE_KEYPAD_6,
  EVENT_KEY_CODE_KEYPAD_7,
  EVENT_KEY_CODE_KEYPAD_8,
  EVENT_KEY_CODE_KEYPAD_9,
  EVENT_KEY_CODE_KEYPAD_DECIMAL,
  EVENT_KEY_CODE_KEYPAD_DIVIDE,
  EVENT_KEY_CODE_KEYPAD_MULTIPLY,
  EVENT_KEY_CODE_KEYPAD_SUBTRACT,
  EVENT_KEY_CODE_KEYPAD_ADD,
  EVENT_KEY_CODE_KEYPAD_ENTER,
  EVENT_KEY_CODE_KEYPAD_EQUAL,
  EVENT_KEY_CODE_OEM_102,
  EVENT_KEY_CODE_COUNT
};

// Modifier keys held during an input event, combined as a bit set
enum event_key_modifier_t
{
  EVENT_KEY_MODIFIER_NONE    = 0x00,
  EVENT_KEY_MODIFIER_SHIFT   = 0x01,
  EVENT_KEY_MODIFIER_CONTROL = 0x02,
  EVENT_KEY_MODIFIER_ALT     = 0x04,
  EVENT_KEY_MODIFIER_SUPER   = 0x08
};

/*******************************************************************************
 *
 *   BASE
 *
 ******************************************************************************/

// The start of every event
struct event_base_t
{
  enum event_type_t event_type;
  // Set by the handler that takes the event, which stops it going further
  unsigned int is_handled;
};

/*******************************************************************************
 *
 *   WINDOW EVENTS
 *
 ******************************************************************************/

// A window was asked to close
struct event_window_close_t
{
  struct event_base_t base;
  void* window_handle;
};

// A window's size changed
struct event_window_resize_t
{
  struct event_base_t base;
  void* window_handle;
  int width, height;
};

// A window gained or lost focus
struct event_window_focus_t
{
  struct event_base_t base;
  void* window_handle;
  int is_focused;
};

// struct event_window_lost_focus_t
// {
//    struct event_base_t base;
//    void* window_handle;
// };

// struct event_window_moved_t
// {
//    struct event_base_t base;
//    void* window_handle;
//    float x, y;
// };

/*******************************************************************************
 *
 *   APPLICATION EVENTS
 *
 ******************************************************************************/

// Sent once a frame, before the application's windows run
struct event_app_tick_t
{
  struct event_base_t base;
  // Time since the last tick
  float delta_time_seconds;
};

// struct event_app_update_t
// {
//    struct event_base_t base;
// };

// struct event_app_render_t
// {
//    struct event_base_t base;
// };

/*******************************************************************************
 *
 *   KEY EVENTS
 *
 ******************************************************************************/

// A key was pressed, or is repeating while held
struct event_key_pressed_t
{
  struct event_base_t base;
  void* window_handle;
  enum event_key_code_t key;
  // Bit set of enum event_key_modifier_t
  uint8_t modifiers;
  // Set when the key is being held and the press is repeating
  bool is_repeat;
};

// A key was released
struct event_key_released_t
{
  struct event_base_t base;
  void* window_handle;
  enum event_key_code_t key;
  // Bit set of enum event_key_modifier_t
  uint8_t modifiers;
};

// A character of text input, after keyboard layout and modifiers are applied
struct event_key_typed_t
{
  struct event_base_t base;
  void* window_handle;
  uint32_t codepoint;
};

/*******************************************************************************
 *
 *   MOUSE EVENTS
 *
 ******************************************************************************/

// A mouse button was pressed or released
struct event_mouse_button_t
{
  struct event_base_t base;
  void* window_handle;
  // 0 is the left button, 1 the right and 2 the middle
  int button;
  // 1 when pressed, 0 when released
  int action;
  // Bit set of enum event_key_modifier_t
  int mods;
};

// The cursor moved over a window
struct event_mouse_moved_t
{
  struct event_base_t base;
  void* window_handle;
  // Measured from the window's bottom-left corner
  float x, y;
};

// A mouse wheel or touchpad scrolled
struct event_mouse_scrolled_t
{
  struct event_base_t base;
  void* window_handle;
  float x_offset, y_offset;
};

// The cursor crossed into or out of a window's content area
struct event_mouse_entered_t
{
  struct event_base_t base;
  void* window_handle;
  bool is_entered;
};
