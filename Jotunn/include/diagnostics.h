#pragma once

// Jotunn's diagnostics: an overlay, toggled with F3 in each window, with tabs
//  on frame timing, heap memory and graphics resources. Applications can time
//  their own code in zones, shown on the Timing tab, and add tabs of their
//  own. Built when Jotunn is configured with JOTUNN_ENABLE_DIAGNOSTICS;
//  otherwise these calls compile to nothing

#include <stdbool.h>
#include <stdint.h>

// Returned in place of a zone that couldn't be registered; beginning or
//  ending it does nothing
#define DIAGNOSTICS_ZONE_INVALID UINT16_MAX

#ifdef JOTUNN_DIAGNOSTICS

/*******************************************************************************
 *
 *   ZONES
 *
 ******************************************************************************/

// Registers a named zone of code to time each frame. Registering a name
//  again returns the zone already registered under it
//
//  in:     in_name
//  ret:    The zone's id; DIAGNOSTICS_ZONE_INVALID when every zone is taken
uint16_t diagnostics_zone_register(
  const char* in_name);

// Starts timing a zone. A zone may be entered several times a frame; the
//  times are summed
//
//  in:     in_zone_id
void diagnostics_zone_begin(
  const uint16_t in_zone_id);

// Stops timing a zone begun with diagnostics_zone_begin()
//
//  in:     in_zone_id
void diagnostics_zone_end(
  const uint16_t in_zone_id);

/*******************************************************************************
 *
 *   TABS
 *
 ******************************************************************************/

// Adds a tab to the diagnostics panel, after the engine's own.
//  in_function_draw makes ig* and ImPlot_* calls for the tab's contents while
//  the tab is open
//
//  in:     in_name, in_function_draw, in_user_data
//  ret:    true when the tab was added; false when in_function_draw is NULL
//            or every tab is taken
bool diagnostics_tab_add(
  const char* in_name,
  void (*in_function_draw)(void* inout_user_data),
  void* in_user_data);

#else

// Without diagnostics the calls vanish, though their arguments are still
//  evaluated, so code using them needs no #ifdefs of its own
#define diagnostics_zone_register(in_name) \
  ((void)(in_name), (uint16_t)DIAGNOSTICS_ZONE_INVALID)
#define diagnostics_zone_begin(in_zone_id) \
  ((void)(in_zone_id))
#define diagnostics_zone_end(in_zone_id) \
  ((void)(in_zone_id))
#define diagnostics_tab_add(in_name, in_function_draw, in_user_data) \
  ((void)(in_name), (void)(in_function_draw), (void)(in_user_data), false)

#endif
