#pragma once

enum event_type_t
{
   EVENT_WINDOW_CLOSE = 0x00, 
   EVENT_WINDOW_RESIZE,
   EVENT_WINDOW_FOCUS,
   EVENT_WINDOW_LOST_FOCUS,
   EVENT_WINDOW_MOVED,

   EVENT_APP_TICK, 
   EVENT_APP_UPDATE, 
   EVENT_APP_RENDER,

   EVENT_KEY_PRESSED,
   EVENT_KEY_RELEASED,
   EVENT_KEY_TYPED,

   EVENT_MOUSE_BUTTON_PRESSED,
   EVENT_MOUSE_BUTTON_RELEASED,
   EVENT_MOUSE_MOVED,
   EVENT_MOUSE_SCROLLED
};

struct event_base_t
{
   enum event_type_t event_type;
   unsigned int handled;
};

// Window events

struct event_window_close_t
{
   struct event_base_t base;
   void* window_handle;
};

struct event_window_resize_t
{
   struct event_base_t base;
   void* window_handle;
   int width, height;
};

struct event_window_focus_t
{
   struct event_base_t base;
   void* window_handle;
   int focused;
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

// App events

// struct event_app_tick_t
// {
//    struct event_base_t base;
// };

// struct event_app_update_t
// {
//    struct event_base_t base;
// };

// struct event_app_render_t
// {
//    struct event_base_t base;
// };

// Key events

// struct event_key_pressed_t
// {
//    struct event_base_t base;
//    int key, scancode, action, mods;
// };

// struct event_key_released_t
// {
//    struct event_base_t base;
//    int key, scancode, action, mods;
// };

// struct event_key_typed_t
// {
//    struct event_base_t base;
//    int key, scancode, action, mods;
// };

// Mouse events

// struct event_mouse_button_pressed_t
// {
//    struct event_base_t base;
//    int button, action, mods;
// };

// struct event_mouse_button_released_t
// {
//    struct event_base_t base;
//    int button, action, mods;
// };

struct event_mouse_moved_t
{
   struct event_base_t base;
   void* window_handle;
   float x, y;
};

struct event_mouse_scrolled_t
{
   struct event_base_t base;
   float x_offset, y_offset;
};