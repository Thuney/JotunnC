/** File Include    **/
#include "application.h"

/** System Includes **/
#include <font.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Local Includes  **/
#include "allocator.h"
#include "log.h"
#include "render_api.h"
#include "resource.h"
#include "util.h"

#include "../diagnostics/diagnostics_internal.h"

// Sent to the application each frame, carrying the time since the last one
static struct event_app_tick_t app_tick_event;

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

#ifdef DEBUG
// Prints the heap memory and graphics resources still alive once the
//  application has cleaned up, each of which is a leak
static void _application_report_leaks(void);
#endif

/*******************************************************************************
 *
 *   Function: application_init()
 *
 ******************************************************************************/
uint8_t application_init(
  struct application_t* out_app,
  const char* in_app_name,
  uint8_t in_max_windows)
{
#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Initializing application, with Jotunn at %s",
    PATH_FROM_JOTUNN_ROOT(""));
#endif

  uint8_t error = 0;

  memset(
    out_app,
    0,
    sizeof(struct application_t));

  out_app->name = in_app_name;

  out_app->max_windows = in_max_windows;
  out_app->num_windows = 0;

  out_app->windows =
    (struct window_t**)allocator_allocate(
      (out_app->max_windows * sizeof(struct window_t*)),
      ALLOCATOR_TAG_APPLICATION);

  out_app->current_window = NULL;

  font_init();

  timestep_init(&(out_app->timer));

  app_tick_event =
    (struct event_app_tick_t)
    {
      .base = (struct event_base_t)
        {
          .event_type = EVENT_TYPE_APP_TICK,
          .is_handled = 0U
        },
      .delta_time_seconds = 0.0f
    };

  out_app->function_event_react = NULL;

  return error;
}

/*******************************************************************************
 *
 *   Function: application_start()
 *
 ******************************************************************************/
uint8_t application_start(
  struct application_t* inout_app)
{
#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Starting application");
#endif

  timestep_reset(&(inout_app->timer));
  inout_app->running = 1;

  return 0;
}

/*******************************************************************************
 *
 *   Function: application_run()
 *
 ******************************************************************************/
void application_run(
  struct application_t* inout_app)
{
  const float seconds_between_frames = 0.01f;

  if ((inout_app->num_windows > 0) &&
      inout_app->running)
  {
    timestep_step(&(inout_app->timer));
    app_tick_event.delta_time_seconds += inout_app->timer.delta_time_seconds;
    app_tick_event.base.is_handled = 0U;

    // #ifdef DEBUG
    //  fprintf(stdout, "App Time (sec): %f -- Delta (sec): %f\n",
    //  app->timer.current_time_seconds, app_tick_event.delta_time_seconds);
    //     fflush(stdout);
    // #endif

    if (app_tick_event.delta_time_seconds >= seconds_between_frames) {
      diagnostics_internal_frame_begin(seconds_between_frames);

      application_on_event(
        inout_app,
        &(app_tick_event.base));

      // Pointer to first window, to be iterated over
      struct window_t** cur_window = inout_app->windows;
      for (uint8_t i = 0U; i < inout_app->num_windows; i++) {
        // #ifdef DEBUG
        //     fprintf(stdout, "Running Window %s\n", cur_window->metadata.tag);
        // #endif

        if ((*cur_window)->metadata.is_visible) {
          const uint8_t should_close = window_run(*cur_window);
          if (should_close) {
            inout_app->running = 0U;

          #ifdef DEBUG
            log_write(
              LOG_LEVEL_INFO,
              "Window signaled to close");
          #endif
          }
        }
        cur_window++;
      }

      app_tick_event.delta_time_seconds = 0.0f;

      diagnostics_internal_frame_end();
    }
    else {
      diagnostics_internal_count_idle_call();
    }
  }
}

/*******************************************************************************
 *
 *   Function: application_stop()
 *
 ******************************************************************************/
uint8_t application_stop(
  struct application_t* inout_app)
{
#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Stopping application");
#endif

  inout_app->running = 0;

  return 0;
}

/*******************************************************************************
 *
 *   Function: application_cleanup()
 *
 ******************************************************************************/
void application_cleanup(
  struct application_t* inout_app)
{
#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Cleaning up application");
#endif

  if (inout_app->running) {
    application_stop(inout_app);
  }

  inout_app->name = NULL;

  if (inout_app->num_windows) {
    // Pointer to first window, to be iterated over
    struct window_t** cur_window = inout_app->windows;
    for (uint8_t i = 0U; i < inout_app->num_windows; i++) {
      window_cleanup(*cur_window);
      cur_window++;
    }
  }

  font_cleanup();
  timestep_cleanup();

  inout_app->max_windows = 0;
  inout_app->num_windows = 0;
  allocator_free(inout_app->windows);

#ifdef DEBUG
  _application_report_leaks();
#endif
}

/*******************************************************************************
 *
 *   Function: application_bind_custom_events()
 *
 ******************************************************************************/
void application_bind_custom_events(
  struct application_t* inout_app,
  void (*in_custom_event_function)(struct application_t* inout_app,
                                struct event_base_t* inout_event))
{
  inout_app->function_event_react = in_custom_event_function;
}

/*******************************************************************************
 *
 *   Function: application_on_event()
 *
 ******************************************************************************/
void application_on_event(
  struct application_t* inout_app,
  struct event_base_t* inout_event)
{
  if (inout_app->function_event_react) {
    inout_app->function_event_react(
      inout_app,
      inout_event);

    if (inout_event->is_handled) {
      diagnostics_internal_event_record(
        inout_app,
        inout_event);

      return;
    }
  }

  switch (inout_event->event_type)
  {
    case EVENT_TYPE_WINDOW_RESIZE:
    {
      struct event_window_resize_t* window_resize_event =
        container_of(
          inout_event,
          struct event_window_resize_t,
          base);

      uint32_t new_width, new_height;
      new_width = window_resize_event->width;
      new_height = window_resize_event->height;

      if (inout_app->num_windows) {
        // Pointer to first window, to be iterated over
        struct window_t** cur_window = inout_app->windows;
        for (uint8_t i = 0U; i < inout_app->num_windows; i++) {
          if ((*cur_window)->context_data.window_handle ==
              window_resize_event->window_handle)
          {
          #ifdef DEBUG
            fprintf(
              stdout,
              "Resizing window - %s\n",
              (*cur_window)->metadata.tag);
          #endif

            (*cur_window)->metadata.width = new_width;
            (*cur_window)->metadata.height = new_height;
            (*cur_window)->metadata.is_resized = 1;
          }

          cur_window++;
        }
      }
    }
    break;

    case EVENT_TYPE_WINDOW_FOCUS:
    {
      struct event_window_focus_t* window_focus_event =
        container_of(
          inout_event,
          struct event_window_focus_t,
          base);

      if (window_focus_event->is_focused) {
        if (inout_app->num_windows) {
          // Pointer to first window, to be iterated over
          struct window_t** cur_window = inout_app->windows;
          for (uint8_t i = 0U; i < inout_app->num_windows; i++) {
            if ((*cur_window)->context_data.window_handle ==
                window_focus_event->window_handle)
            {
            // #ifdef DEBUG
            //   fprintf(stdout,
            //           "Refocusing window - %s\n",
            //           (*cur_window)->metadata.tag);
            // #endif

              inout_app->current_window = (*cur_window);
              break;
            }
            cur_window++;
          }
        }
      }
    }
    break;

    case EVENT_TYPE_MOUSE_MOVED:
    {
      struct event_mouse_moved_t* mouse_moved_event =
        container_of(
          inout_event,
          struct event_mouse_moved_t,
          base);

      float x, y;
      x = mouse_moved_event->x;
      y = (inout_app->current_window->metadata.height - mouse_moved_event->y);
    }
    break;

    default:
    {
    }
    break;
  }

  // Custom events

  if (!inout_event->is_handled) {
    // Pointer to first window, to be iterated over
    struct window_t** cur_window = inout_app->windows;
    for (uint8_t i = 0U; i < inout_app->num_windows; i++) {
      window_handle_event(*cur_window, inout_event);

      cur_window++;
    }
  }

  // Recorded once every handler has seen it, so it's known whether one took it
  diagnostics_internal_event_record(
    inout_app,
    inout_event);
}

/*******************************************************************************
 *
 *   Function: application_add_window()
 *
 ******************************************************************************/
uint8_t application_add_window(
  struct application_t* inout_app,
  struct window_t* in_new_window)
{
    #ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Adding window to application");
    #endif

  uint8_t error = 0U;

  if (inout_app->num_windows < inout_app->max_windows) {
    struct window_t** window_ptr;
    window_ptr = (inout_app->windows + inout_app->num_windows);
    *window_ptr = in_new_window;
    inout_app->current_window = *window_ptr;
    inout_app->num_windows++;
  }
  else {
    error |= 1U;
  }

  return error;
}

#ifdef DEBUG

/*******************************************************************************
 *
 *   Function: _application_report_leaks()
 *
 ******************************************************************************/
static void _application_report_leaks(void)
{
  for (int tag = 0; tag < ALLOCATOR_TAG_COUNT; tag++) {
    struct allocator_tag_stats_t stats;
    allocator_get_tag_stats(
      (enum allocator_tag_t)tag,
      &(stats));

    if (stats.live_count > 0) {
      fprintf(
        stdout,
        "Live heap memory: %s, %zu bytes in %llu blocks\n",
        allocator_get_tag_name((enum allocator_tag_t)tag),
        stats.live_bytes,
        (unsigned long long)stats.live_count);
    }
  }

  resource_log_live();
}

#endif
