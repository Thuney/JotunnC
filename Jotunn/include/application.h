#pragma once

// An application runs Jotunn's main loop: it steps a clock, runs a frame of
//  each of its windows at a fixed interval, and routes the events those
//  windows raise to the application, then to the windows and their layers

/** System Includes **/

/** Local Includes  **/
#include "event.h"
#include "timestep.h"
#include "util.h"
#include "window.h"

/*******************************************************************************
 *
 *   APPLICATION
 *
 ******************************************************************************/

// An application and the windows it runs
struct application_t
{
  // Not owned, so it must outlive the application
  const char* name;
  //
  struct window_t* current_window;
  struct window_t** windows;
  uint8_t num_windows;
  uint8_t max_windows;
  //
  struct timestep_t timer;
  // Nonzero while application_run() should run frames
  uint8_t running;

  // Sees each event before the application's own handling; see
  //  application_bind_custom_events()
  void (*function_event_react)(
    struct application_t* inout_app,
    struct event_base_t* inout_event);
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Sets up an application with room for in_max_windows windows, and
//  initializes the font library. The application keeps in_app_name, which
//  must outlive it
//
//  in:     in_app_name, in_max_windows
//  out:    out_app
//  ret:    0
uint8_t application_init(
  struct application_t* out_app,
  const char* in_app_name,
  uint8_t in_max_windows);

// Restarts the application's clock and marks it running, so that
//  application_run() runs frames
//
//  in/out: inout_app
//  ret:    0
uint8_t application_start(
  struct application_t* inout_app);

// Runs one pass of the main loop; call it for as long as the application is
//  running. Once 0.01 seconds have passed since the last frame, it sends the
//  app tick event, carrying the time since the last frame, and runs a frame of
//  each visible window; otherwise it returns at once. Does nothing until the
//  application has a window and has been started. A window asking to close
//  stops the application
//
//  in/out: inout_app
void application_run(
  struct application_t* inout_app);

// Marks the application as no longer running
//
//  in/out: inout_app
//  ret:    0
uint8_t application_stop(
  struct application_t* inout_app);

// Stops the application if it's running, cleans up each of its windows and
//  the font library, and frees what application_init() allocated. The
//  windows' own memory stays the caller's to free. Debug builds then log any
//  heap memory or graphics resources still alive, each of which is a leak
//
//  in/out: inout_app
void application_cleanup(
  struct application_t* inout_app);

/*******************************************************************************
 *
 *   EVENTS
 *
 ******************************************************************************/

// Routes an event, as every window's backend does with the events it raises.
//  The application's custom handler sees it first, if one is bound, and
//  takes it by marking it handled. Otherwise the application handles window
//  resizes and focus itself, by resizing the window concerned or making it
//  the current window, then, unless the event was handled by then, passes it
//  to every window. Every event is recorded for diagnostics
//
//  in/out: inout_app, inout_event
void application_on_event(
  struct application_t* inout_app,
  struct event_base_t* inout_event);

// Binds the handler that sees each event before the application does; NULL
//  unbinds it
//
//  in:     in_custom_event_function
//  in/out: inout_app
void application_bind_custom_events(
  struct application_t* inout_app,
  void (*in_custom_event_function)(struct application_t* inout_app,
                                struct event_base_t* inout_event));

/*******************************************************************************
 *
 *   WINDOWS
 *
 ******************************************************************************/

// Adds a window to the application's windows, making it the current one. The
//  application keeps the pointer, and cleans the window up in
//  application_cleanup(), but doesn't free it
//
//  in:     in_new_window
//  in/out: inout_app
//  ret:    0 on success; 1 when the application already has as many windows
//            as it has room for
uint8_t application_add_window(
  struct application_t* inout_app,
  struct window_t* in_new_window);
// uint8_t application_remove_window(
//   struct application_t* app,
//   struct window_t* remove_window);
