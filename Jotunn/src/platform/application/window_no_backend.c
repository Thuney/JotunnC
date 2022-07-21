#include <stdio.h>

#include "window.h"

// Extern function definitions
int window_graphics_init(struct window_t* window)
{
   #ifdef DEBUG
      printf("No graphics backend!\n");
   #endif

   window->metadata.signaled_close = 1;

   return 1;
}

void window_graphics_run(struct window_t* window)
{
   return;
}

void window_graphics_cleanup(struct window_t* window)
{
   return;
}

void window_graphics_set_background_color(struct window_t* window, float red, float green, float blue, float alpha)
{
   return;
}