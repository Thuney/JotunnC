#include <stdio.h>

#include "window.h"

// Extern function definitions
void window_graphics_init(struct window_t* window)
{
   #ifdef DEBUG
      printf("No graphics backend!\n");
   #endif

   window->metadata.signaled_close = 1;
}

void window_graphics_run(struct window_t* window)
{
   return;
}

void window_graphics_cleanup(struct window_t* window)
{
   return;
}