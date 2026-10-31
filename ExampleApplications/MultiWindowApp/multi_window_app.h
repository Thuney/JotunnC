#pragma once

// The multi-window example's application: two windows, each with two
//  layers, sharing one set of state. Setting up the application and adding
//  its windows is here, in multi_window_app.c; each window sets itself up in
//  its own file, multi_window_app_scene_window.c and
//  multi_window_app_controls_window.c

#include "application.h"
#include "multi_window_app_controls_window.h"
#include "multi_window_app_scene_window.h"
#include "multi_window_app_state.h"

#include <stdint.h>

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// Everything the application owns
struct multi_window_app_t
{
  struct application_t application;
  // Shared by both windows, which each keep a pointer to it
  struct multi_window_app_state_t state;
  //
  struct multi_window_app_scene_window_t scene_window;
  struct multi_window_app_controls_window_t controls_window;
};

/*******************************************************************************
 *
 *   LIFETIME
 *
 ******************************************************************************/

// Sets up the application and both its windows, and adds the windows to it
//
//  out:    out_app
//  ret:    0 on success; non-zero when the application, either window or any
//            of their layers can't be set up
uint8_t multi_window_app_init(
  struct multi_window_app_t* out_app);

// Releases what multi_window_app_init() set up
//
//  in/out: inout_app
void multi_window_app_cleanup(
  struct multi_window_app_t* inout_app);
