#pragma once

// Private to Jotunn: the diagnostics' hooks into the engine's frame, what
//  they collect, and the overlay each window shows it in. Without
//  JOTUNN_DIAGNOSTICS the hooks compile to nothing

#include "diagnostics.h"
#include "log.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct event_base_t;
struct texture_2d_t;
struct window_layer_t;
struct window_t;

/*******************************************************************************
 *
 *   ENGINE ZONES
 *
 ******************************************************************************/

// The zones the engine times in every frame, registered ahead of any
//  application's
enum diagnostics_internal_engine_zone_t
{
  // Polling for and reacting to events
  DIAGNOSTICS_INTERNAL_ZONE_EVENTS = 0x00,
  // Rendering every window's layers to their framebuffers
  DIAGNOSTICS_INTERNAL_ZONE_LAYERS,
  // Rendering the diagnostics overlay itself
  DIAGNOSTICS_INTERNAL_ZONE_OVERLAY,
  // Drawing the layers' framebuffers to their windows
  DIAGNOSTICS_INTERNAL_ZONE_COMPOSITE,
  // Presenting the windows, including any wait for vertical sync
  DIAGNOSTICS_INTERNAL_ZONE_PRESENT,
  DIAGNOSTICS_INTERNAL_ZONE_ENGINE_COUNT
};

#ifdef JOTUNN_DIAGNOSTICS

#include "allocator.h"

#define DIAGNOSTICS_INTERNAL_HISTORY_LENGTH 512
#define DIAGNOSTICS_INTERNAL_MAX_ZONES 64
#define DIAGNOSTICS_INTERNAL_MAX_TABS 8
#define DIAGNOSTICS_INTERNAL_NAME_LENGTH 48
#define DIAGNOSTICS_INTERNAL_LOG_LENGTH 256

/*******************************************************************************
 *
 *   COLLECTED STATE
 *
 ******************************************************************************/

// One value per frame for the most recent frames, kept in a ring
struct diagnostics_internal_history_t
{
  float values[DIAGNOSTICS_INTERNAL_HISTORY_LENGTH];
  // Where the next value goes, which once the ring is full is the oldest
  uint32_t next_index;
  uint32_t count;
};

// The spread of the values in a history
struct diagnostics_internal_summary_t
{
  float latest;
  float average;
  float percentile_99;
  float maximum;
};

struct diagnostics_internal_zone_t
{
  char name[DIAGNOSTICS_INTERNAL_NAME_LENGTH];
  bool is_open;
  double begin_seconds;
  // Summed over every entry this frame
  double frame_seconds;
  struct diagnostics_internal_history_t milliseconds;
};

struct diagnostics_internal_log_entry_t
{
  enum log_level_t level;
  // The frame being recorded when the message was written
  uint64_t frame;
  char text[LOG_MESSAGE_MAX_LENGTH];
};

// The most recent log messages, kept in a ring like the histories
struct diagnostics_internal_log_t
{
  struct diagnostics_internal_log_entry_t entries[DIAGNOSTICS_INTERNAL_LOG_LENGTH];
  uint32_t next_index;
  uint32_t count;
  // Every message written at each level, including those since displaced
  uint64_t level_counts[LOG_LEVEL_COUNT];
};

struct diagnostics_internal_tab_t
{
  char name[DIAGNOSTICS_INTERNAL_NAME_LENGTH];
  void (*function_draw)(void* inout_user_data);
  void* user_data;
};

struct diagnostics_internal_state_t
{
  double start_seconds;
  double frame_begin_seconds;
  uint64_t frame_count;
  float target_interval_milliseconds;

  // Frame timing
  struct diagnostics_internal_history_t frame_interval_milliseconds;
  struct diagnostics_internal_history_t frame_work_milliseconds;
  // Passes through the application loop that found it too early to render
  uint64_t idle_call_count;
  struct diagnostics_internal_history_t idle_calls;

  // Heap memory, when the allocator counts it
  bool is_heap_counted;
  struct diagnostics_internal_history_t heap_live_megabytes;
  struct diagnostics_internal_history_t heap_allocations;
  uint64_t tag_total_counts[ALLOCATOR_TAG_COUNT];
  uint64_t tag_frame_allocations[ALLOCATOR_TAG_COUNT];

  // Graphics memory
  struct diagnostics_internal_history_t resource_live_megabytes;

  struct diagnostics_internal_zone_t zones[DIAGNOSTICS_INTERNAL_MAX_ZONES];
  uint16_t zone_count;

  struct diagnostics_internal_tab_t tabs[DIAGNOSTICS_INTERNAL_MAX_TABS];
  uint8_t tab_count;

  struct diagnostics_internal_log_t log;
};

/*******************************************************************************
 *
 *   FRAMES
 *
 ******************************************************************************/

// Marks the start of a frame of the application loop
//
//  in:     in_target_interval_seconds
void diagnostics_internal_frame_begin(
  const float in_target_interval_seconds);

// Marks the end of a frame, recording what it measured
void diagnostics_internal_frame_end(void);

// Counts a pass through the application loop that rendered nothing
void diagnostics_internal_count_idle_call(void);

// Reads what the diagnostics have collected
//
//  ret:    The collected state
const struct diagnostics_internal_state_t* diagnostics_internal_get_state(void);

// Reads the monotonic clock the diagnostics time with
//
//  ret:    The time in seconds from an arbitrary start
double diagnostics_internal_get_time_seconds(void);

// Summarizes the values in a history
//
//  in:     in_history
//  out:    out_summary
void diagnostics_internal_history_summarize(
  const struct diagnostics_internal_history_t* in_history,
  struct diagnostics_internal_summary_t* out_summary);

/*******************************************************************************
 *
 *   OVERLAY
 *
 ******************************************************************************/

// Shows or hides the window's overlay when F3 is pressed in it, and passes
//  other events to it while it's shown, marking those it takes as handled
//
//  in/out: inout_window, inout_event
//  ret:    true when the overlay took the event; otherwise false, even when
//            the event was already handled
bool diagnostics_internal_overlay_event_react(
  struct window_t* inout_window,
  struct event_base_t* inout_event);

// Readies the window's overlay to be rendered this frame, creating its layer
//  the first time it's shown. The window's graphics context must be current
//
//  in/out: inout_window
//  ret:    The overlay's layer when it's shown; otherwise NULL
struct window_layer_t* diagnostics_internal_overlay_prepare(
  struct window_t* inout_window);

// Releases the window's overlay. Its layer makes the window's graphics
//  context current to release its graphics memory, leaving none current
//
//  in/out: inout_window
void diagnostics_internal_overlay_cleanup(
  struct window_t* inout_window);

/*******************************************************************************
 *
 *   PANEL
 *
 ******************************************************************************/

// Draws the diagnostics panel into the current ImGui frame. The panel clears
//  inout_is_open when it's closed. in_target_texture is the texture the panel
//  is rendered into, which it can't preview while rendering to it
//
//  in:     in_target_texture
//  in/out: inout_is_open
void diagnostics_internal_panel_draw(
  bool* inout_is_open,
  const struct texture_2d_t* in_target_texture);

/*******************************************************************************
 *
 *   LOG
 *
 ******************************************************************************/

// Keeps a log message for the Log tab, displacing the oldest once the log is
//  full
//
//  in:     in_level, in_text
void diagnostics_internal_log_record(
  const enum log_level_t in_level,
  const char* in_text);

// Empties the log kept for the Log tab, keeping the counts per level
void diagnostics_internal_log_clear(void);

#else

#define diagnostics_internal_log_record(in_level, in_text) \
  ((void)(in_level), (void)(in_text))
#define diagnostics_internal_frame_begin(in_target_interval_seconds) \
  ((void)(in_target_interval_seconds))
#define diagnostics_internal_frame_end() ((void)0)
#define diagnostics_internal_count_idle_call() ((void)0)
#define diagnostics_internal_overlay_event_react(inout_window, inout_event) \
  ((void)(inout_window), (void)(inout_event), false)
#define diagnostics_internal_overlay_prepare(inout_window) \
  ((void)(inout_window), (struct window_layer_t*)NULL)
#define diagnostics_internal_overlay_cleanup(inout_window) \
  ((void)(inout_window))

#endif
