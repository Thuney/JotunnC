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

struct event_window_resize_t
{
   struct event_base_t base;
   void* window_handle;
   int width, height;
};

struct event_mouse_moved_t
{
   struct event_base_t base;
   void* window_handle;
   float x, y;
};

