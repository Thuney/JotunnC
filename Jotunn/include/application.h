#pragma once

/** System Includes **/

/** Local Includes  **/
#include "event.h"
#include "timestep.h"
#include "util.h"
#include "window.h"

/************************************************************
 *
 * Struct Definitions
 *
 ************************************************************/
struct application_t
{
  //
  char* name;
  //
  struct window_t* current_window;
  struct window_t** windows;
  uint8_t num_windows;
  uint8_t max_windows;
  //
  struct timestep_t timer;
  uint8_t running;

  void (*function_event_react)(
    struct application_t* app,
    struct event_base_t* event);
};

/************************************************************
 *
 * Interface Functions
 *
 ************************************************************/
uint8_t application_init(
  struct application_t* app,
  const char* app_name,
  const uint8_t max_windows);

uint8_t application_start(
  struct application_t* app);
void application_run(
  struct application_t* app);
uint8_t application_stop(
  struct application_t* app);

void application_cleanup(
  struct application_t* app);

void application_on_event(
  struct application_t* app,
  struct event_base_t* event);
void application_bind_custom_events(
  struct application_t* app,
  void (*custom_event_function)(struct application_t*,
                                struct event_base_t*));

uint8_t application_add_window(
  struct application_t* app,
  struct window_t* new_window);
// uint8_t application_remove_window(
//   struct application_t* app,
//   struct window_t* remove_window);
