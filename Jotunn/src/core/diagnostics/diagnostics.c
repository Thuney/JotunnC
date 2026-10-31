#define _POSIX_C_SOURCE 199309L
#include "diagnostics_internal.h"

#include "allocator.h"
#include "application.h"
#include "render_api.h"
#include "renderer.h"
#include "resource.h"
#include "util.h"
#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*******************************************************************************
 *
 *   STATE
 *
 ******************************************************************************/

static struct diagnostics_internal_state_t diagnostics_state =
{
  .zones =
  {
    [DIAGNOSTICS_INTERNAL_ZONE_EVENTS]    = { .name = "events" },
    [DIAGNOSTICS_INTERNAL_ZONE_LAYERS]    = { .name = "layers" },
    [DIAGNOSTICS_INTERNAL_ZONE_OVERLAY]   = { .name = "diagnostics overlay" },
    [DIAGNOSTICS_INTERNAL_ZONE_COMPOSITE] = { .name = "composite" },
    [DIAGNOSTICS_INTERNAL_ZONE_PRESENT]   = { .name = "present" },
  },
  .zone_count = DIAGNOSTICS_INTERNAL_ZONE_ENGINE_COUNT,
  // Each arrives every frame, or nearly, so they'd crowd out everything else
  .events =
  {
    .types =
    {
      [EVENT_TYPE_APP_TICK]    = { .is_excluded = true },
      [EVENT_TYPE_MOUSE_MOVED] = { .is_excluded = true },
    }
  }
};

static const char* const diagnostics_event_type_names[] =
{
  [EVENT_TYPE_WINDOW_CLOSE]      = "window close",
  [EVENT_TYPE_WINDOW_RESIZE]     = "window resize",
  [EVENT_TYPE_WINDOW_FOCUS]      = "window focus",
  [EVENT_TYPE_WINDOW_LOST_FOCUS] = "window lost focus",
  [EVENT_TYPE_WINDOW_MOVED]      = "window moved",
  [EVENT_TYPE_APP_TICK]          = "app tick",
  [EVENT_TYPE_APP_UPDATE]        = "app update",
  [EVENT_TYPE_APP_RENDER]        = "app render",
  [EVENT_TYPE_KEY_PRESSED]       = "key pressed",
  [EVENT_TYPE_KEY_RELEASED]      = "key released",
  [EVENT_TYPE_KEY_TYPED]         = "key typed",
  [EVENT_TYPE_MOUSE_BUTTON]      = "mouse button",
  [EVENT_TYPE_MOUSE_MOVED]       = "mouse moved",
  [EVENT_TYPE_MOUSE_SCROLLED]    = "mouse scrolled",
  [EVENT_TYPE_MOUSE_ENTERED]     = "mouse entered",
};

static_assert(
  (sizeof(diagnostics_event_type_names) / sizeof(diagnostics_event_type_names[0])) ==
    EVENT_TYPE_COUNT,
  "Every event type needs a name");

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Finds the zone registered under a name, timed on the CPU or the GPU,
//  registering it the first time
//
//  in:     in_name, in_is_gpu
//  ret:    The zone's id; DIAGNOSTICS_ZONE_INVALID when every zone is taken
static uint16_t _diagnostics_register_zone(
  const char* in_name,
  bool in_is_gpu);

// Finds the zone timing part of a window's frame, named after the window's
//  tag and in_part, registering it the first time
//
//  in:     in_window, in_part, in_is_gpu
//  ret:    The zone's id; DIAGNOSTICS_ZONE_INVALID when every zone is taken
static uint16_t _diagnostics_get_window_zone(
  const struct window_t* in_window,
  const char* in_part,
  bool in_is_gpu);

// Finds a window's GPU timing, taking a slot for it when asked to and it has
//  none yet
//
//  in:     in_window, in_should_create
//  ret:    The window's GPU timing; NULL when it has none
static struct diagnostics_internal_gpu_window_t* _diagnostics_find_gpu_window(
  const struct window_t* in_window,
  bool in_should_create);

// Charges the GPU time of every pending frame the GPU has finished to its
//  zones
//
//  in/out: inout_gpu_window
static void _diagnostics_collect_gpu_frames(
  struct diagnostics_internal_gpu_window_t* inout_gpu_window);

// Adds a value to a history, displacing the oldest once it's full
//
//  in:     in_value
//  in/out: inout_history
static void _diagnostics_history_push(
  struct diagnostics_internal_history_t* inout_history,
  float in_value);

// Records the heap's counts for the frame just ended
static void _diagnostics_record_heap(void);

// Records the graphics memory held at the end of the frame
static void _diagnostics_record_resources(void);

// Writes an event's particulars, such as a key or a position, for display
//
//  in:     in_event, in_text_length
//  out:    out_text
static void _diagnostics_describe_event(
  const struct event_base_t* in_event,
  char* out_text,
  size_t in_text_length);

// Finds a renderer's slot, taking a new one the first time it's seen; the
//  last slot is shared by every renderer once the rest are taken
//
//  in:     in_renderer
//  ret:    The index of the renderer's slot
static uint8_t _diagnostics_find_renderer(
  const struct renderer_base_t* in_renderer);

// Moves each renderer's counts for the frame just ended to its last frame's
static void _diagnostics_record_rendering(void);

// Finds the tag of the window an event came from
//
//  in:     in_app, in_event
//  ret:    The window's tag; "" for application events and unknown windows
static const char* _diagnostics_find_event_window_tag(
  const struct application_t* in_app,
  const struct event_base_t* in_event);

// Orders floats for qsort()
//
//  in:     in_left, in_right
//  ret:    Negative, zero or positive as in_left sorts before, with or after
//            in_right
static int _diagnostics_compare_floats(
  const void* in_left,
  const void* in_right);

/*******************************************************************************
 *
 *   Function: diagnostics_zone_register()
 *
 ******************************************************************************/
uint16_t diagnostics_zone_register(
  const char* in_name)
{
  return _diagnostics_register_zone(
    in_name,
    false);
}

/*******************************************************************************
 *
 *   Function: diagnostics_zone_begin()
 *
 ******************************************************************************/
void diagnostics_zone_begin(
  uint16_t in_zone_id)
{
  if (in_zone_id >= diagnostics_state.zone_count) {
    return;
  }

  struct diagnostics_internal_zone_t* zone = &(diagnostics_state.zones[in_zone_id]);

  zone->is_open = true;
  zone->begin_seconds = diagnostics_internal_get_time_seconds();
}

/*******************************************************************************
 *
 *   Function: diagnostics_zone_end()
 *
 ******************************************************************************/
void diagnostics_zone_end(
  uint16_t in_zone_id)
{
  if (in_zone_id >= diagnostics_state.zone_count) {
    return;
  }

  struct diagnostics_internal_zone_t* zone = &(diagnostics_state.zones[in_zone_id]);

  if (zone->is_open) {
    zone->frame_seconds += (diagnostics_internal_get_time_seconds() - zone->begin_seconds);
    zone->is_open = false;
  }
}

/*******************************************************************************
 *
 *   Function: diagnostics_tab_add()
 *
 ******************************************************************************/
bool diagnostics_tab_add(
  const char* in_name,
  void (*in_function_draw)(void* inout_user_data),
  void* in_user_data)
{
  if ((in_function_draw == NULL) ||
      (diagnostics_state.tab_count >= DIAGNOSTICS_INTERNAL_MAX_TABS))
  {
    return false;
  }

  struct diagnostics_internal_tab_t* tab = &(diagnostics_state.tabs[diagnostics_state.tab_count]);
  diagnostics_state.tab_count++;

  snprintf(
    tab->name,
    DIAGNOSTICS_INTERNAL_NAME_LENGTH,
    "%s",
    in_name);
  tab->function_draw = in_function_draw;
  tab->user_data = in_user_data;

  return true;
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_frame_begin()
 *
 ******************************************************************************/
void diagnostics_internal_frame_begin(
  float in_target_interval_seconds)
{
  const double now_seconds = diagnostics_internal_get_time_seconds();

  if (diagnostics_state.frame_count == 0) {
    diagnostics_state.start_seconds = now_seconds;

    // Allocations made before the first frame aren't counted against it
    for (int tag = 0; tag < ALLOCATOR_TAG_COUNT; tag++) {
      struct allocator_tag_stats_t stats;
      allocator_get_tag_stats(
        (enum allocator_tag_t)tag,
        &(stats));

      diagnostics_state.tag_total_counts[tag] = stats.total_count;
    }
  }
  else {
    _diagnostics_history_push(
      &(diagnostics_state.frame_interval_milliseconds),
      (float)((now_seconds - diagnostics_state.frame_begin_seconds) * 1000.0));
  }

  _diagnostics_history_push(
    &(diagnostics_state.idle_calls),
    (float)diagnostics_state.idle_call_count);
  diagnostics_state.idle_call_count = 0;

  diagnostics_state.frame_begin_seconds = now_seconds;
  diagnostics_state.target_interval_milliseconds = (in_target_interval_seconds * 1000.0f);
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_frame_end()
 *
 ******************************************************************************/
void diagnostics_internal_frame_end(void)
{
  const double now_seconds = diagnostics_internal_get_time_seconds();

  _diagnostics_history_push(
    &(diagnostics_state.frame_work_milliseconds),
    (float)((now_seconds - diagnostics_state.frame_begin_seconds) * 1000.0));

  for (uint16_t i = 0; i < diagnostics_state.zone_count; i++) {
    struct diagnostics_internal_zone_t* zone = &(diagnostics_state.zones[i]);

    // A zone still open is cut at the frame's end and carries on into the
    //  next
    if (zone->is_open) {
      zone->frame_seconds += (now_seconds - zone->begin_seconds);
      zone->begin_seconds = now_seconds;
    }

    _diagnostics_history_push(
      &(zone->milliseconds),
      (float)(zone->frame_seconds * 1000.0));
    zone->frame_seconds = 0.0;
  }

  _diagnostics_record_heap();
  _diagnostics_record_resources();

  _diagnostics_record_rendering();

  _diagnostics_history_push(
    &(diagnostics_state.events.per_frame),
    (float)diagnostics_state.events.frame_count);
  diagnostics_state.events.frame_count = 0;

  diagnostics_state.frame_count++;
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_count_idle_call()
 *
 ******************************************************************************/
void diagnostics_internal_count_idle_call(void)
{
  diagnostics_state.idle_call_count++;
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_log_record()
 *
 ******************************************************************************/
void diagnostics_internal_log_record(
  enum log_level_t in_level,
  const char* in_text)
{
  struct diagnostics_internal_log_t* log = &(diagnostics_state.log);
  struct diagnostics_internal_log_entry_t* entry = &(log->entries[log->next_index]);

  entry->level = in_level;
  entry->frame = diagnostics_state.frame_count;
  snprintf(
    entry->text,
    sizeof(entry->text),
    "%s",
    in_text);

  log->next_index = (log->next_index + 1) % DIAGNOSTICS_INTERNAL_LOG_LENGTH;
  if (log->count < DIAGNOSTICS_INTERNAL_LOG_LENGTH) {
    log->count++;
  }

  if ((unsigned int)in_level < (unsigned int)LOG_LEVEL_COUNT) {
    log->level_counts[in_level]++;
  }
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_log_clear()
 *
 ******************************************************************************/
void diagnostics_internal_log_clear(void)
{
  diagnostics_state.log.next_index = 0;
  diagnostics_state.log.count = 0;
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_event_record()
 *
 ******************************************************************************/
void diagnostics_internal_event_record(
  const struct application_t* in_app,
  const struct event_base_t* in_event)
{
  if ((unsigned int)in_event->event_type >= (unsigned int)EVENT_TYPE_COUNT) {
    return;
  }

  struct diagnostics_internal_events_t* events = &(diagnostics_state.events);
  struct diagnostics_internal_event_type_stats_t* type_stats =
    &(events->types[in_event->event_type]);

  struct diagnostics_internal_event_entry_t entry =
    {
      .type = in_event->event_type,
      .is_handled = (in_event->is_handled != 0U),
      .frame = diagnostics_state.frame_count
    };
  snprintf(
    entry.window_tag,
    sizeof(entry.window_tag),
    "%s",
    _diagnostics_find_event_window_tag(
      in_app,
      in_event));
  _diagnostics_describe_event(
    in_event,
    entry.details,
    sizeof(entry.details));

  type_stats->count++;
  if (entry.is_handled) {
    type_stats->handled_count++;
  }
  type_stats->latest = entry;
  type_stats->has_latest = true;

  if (in_event->event_type != EVENT_TYPE_APP_TICK) {
    events->frame_count++;
  }

  if (events->is_paused ||
      type_stats->is_excluded)
  {
    return;
  }

  events->entries[events->next_index] = entry;
  events->next_index = (events->next_index + 1) % DIAGNOSTICS_INTERNAL_EVENT_LOG_LENGTH;
  if (events->count < DIAGNOSTICS_INTERNAL_EVENT_LOG_LENGTH) {
    events->count++;
  }
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_event_get_type_name()
 *
 ******************************************************************************/
const char* diagnostics_internal_event_get_type_name(
  enum event_type_t in_type)
{
  return ((unsigned int)in_type < (unsigned int)EVENT_TYPE_COUNT) ?
    diagnostics_event_type_names[in_type] :
    "unknown";
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_events_set_excluded()
 *
 ******************************************************************************/
void diagnostics_internal_events_set_excluded(
  enum event_type_t in_type,
  bool in_is_excluded)
{
  if ((unsigned int)in_type < (unsigned int)EVENT_TYPE_COUNT) {
    diagnostics_state.events.types[in_type].is_excluded = in_is_excluded;
  }
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_events_set_paused()
 *
 ******************************************************************************/
void diagnostics_internal_events_set_paused(
  bool in_is_paused)
{
  diagnostics_state.events.is_paused = in_is_paused;
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_events_clear()
 *
 ******************************************************************************/
void diagnostics_internal_events_clear(void)
{
  diagnostics_state.events.next_index = 0;
  diagnostics_state.events.count = 0;
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_scene_begin()
 *
 ******************************************************************************/
void diagnostics_internal_scene_begin(
  const struct renderer_base_t* in_renderer)
{
  struct diagnostics_internal_rendering_t* rendering = &(diagnostics_state.rendering);
  const uint8_t renderer_index = _diagnostics_find_renderer(in_renderer);

  // A scene too deep to track is charged to the deepest one tracked
  if (rendering->scene_depth < DIAGNOSTICS_INTERNAL_MAX_SCENE_DEPTH) {
    rendering->scene_renderer_indices[rendering->scene_depth] = renderer_index;
  }

  rendering->scene_depth++;
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_scene_end()
 *
 ******************************************************************************/
void diagnostics_internal_scene_end(void)
{
  if (diagnostics_state.rendering.scene_depth > 0) {
    diagnostics_state.rendering.scene_depth--;
  }
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_count_draw()
 *
 ******************************************************************************/
void diagnostics_internal_count_draw(
  uint32_t in_element_count)
{
  struct diagnostics_internal_rendering_t* rendering = &(diagnostics_state.rendering);

  uint8_t renderer_index;
  if (rendering->scene_depth == 0) {
    renderer_index = _diagnostics_find_renderer(NULL);
  }
  else if (rendering->scene_depth > DIAGNOSTICS_INTERNAL_MAX_SCENE_DEPTH) {
    renderer_index = rendering->scene_renderer_indices[DIAGNOSTICS_INTERNAL_MAX_SCENE_DEPTH - 1];
  }
  else {
    renderer_index = rendering->scene_renderer_indices[rendering->scene_depth - 1];
  }

  struct diagnostics_internal_renderer_t* renderer = &(rendering->renderers[renderer_index]);

  renderer->frame_draw_calls++;
  renderer->frame_elements += in_element_count;
  renderer->last_frame = diagnostics_state.frame_count;
  rendering->frame_draw_calls++;
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_get_layer_zone()
 *
 ******************************************************************************/
uint16_t diagnostics_internal_get_layer_zone(
  const struct window_t* in_window,
  uint8_t in_layer_index)
{
  char part[DIAGNOSTICS_INTERNAL_NAME_LENGTH];
  snprintf(
    part,
    sizeof(part),
    "layer %u",
    (unsigned int)in_layer_index);

  return _diagnostics_get_window_zone(
    in_window,
    part,
    false);
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_gpu_frame_begin()
 *
 ******************************************************************************/
void diagnostics_internal_gpu_frame_begin(
  const struct window_t* in_window)
{
  struct diagnostics_internal_gpu_t* gpu = &(diagnostics_state.gpu);

  if (!gpu->is_support_checked) {
    gpu->is_supported = render_api_has_timestamps();
    gpu->is_support_checked = true;
  }

  if (!gpu->is_supported) {
    return;
  }

  struct diagnostics_internal_gpu_window_t* gpu_window =
    _diagnostics_find_gpu_window(
      in_window,
      true);

  if (gpu_window == NULL) {
    return;
  }

  if (!gpu_window->is_created) {
    for (uint8_t i = 0; i < DIAGNOSTICS_INTERNAL_GPU_FRAME_DELAY; i++) {
      render_api_create_timestamps(
        gpu_window->frames[i].timestamp_names,
        DIAGNOSTICS_INTERNAL_GPU_MAX_MARKS);
    }

    gpu_window->is_created = true;
  }

  _diagnostics_collect_gpu_frames(gpu_window);

  struct diagnostics_internal_gpu_frame_t* frame = &(gpu_window->frames[gpu_window->current_frame]);

  // Its timestamps are still in use, so this frame goes untimed rather than
  //  waiting for the GPU
  if (frame->is_pending) {
    gpu_window->is_frame_open = false;
    gpu->skipped_frame_count++;
    return;
  }

  frame->mark_count = 0;
  gpu_window->is_frame_open = true;

  // The first mark only starts the first part
  diagnostics_internal_gpu_mark(
    in_window,
    NULL);
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_gpu_mark()
 *
 ******************************************************************************/
void diagnostics_internal_gpu_mark(
  const struct window_t* in_window,
  const char* in_part)
{
  struct diagnostics_internal_gpu_window_t* gpu_window =
    _diagnostics_find_gpu_window(
      in_window,
      false);

  if ((gpu_window == NULL) ||
      !gpu_window->is_frame_open)
  {
    return;
  }

  struct diagnostics_internal_gpu_frame_t* frame = &(gpu_window->frames[gpu_window->current_frame]);

  if (frame->mark_count >= DIAGNOSTICS_INTERNAL_GPU_MAX_MARKS) {
    return;
  }

  frame->zone_ids[frame->mark_count] =
    (in_part != NULL) ?
      _diagnostics_get_window_zone(
        in_window,
        in_part,
        true) :
      DIAGNOSTICS_ZONE_INVALID;

  render_api_record_timestamp(frame->timestamp_names[frame->mark_count]);
  frame->mark_count++;
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_gpu_mark_layer()
 *
 ******************************************************************************/
void diagnostics_internal_gpu_mark_layer(
  const struct window_t* in_window,
  uint8_t in_layer_index)
{
  char part[DIAGNOSTICS_INTERNAL_NAME_LENGTH];
  snprintf(
    part,
    sizeof(part),
    "layer %u",
    (unsigned int)in_layer_index);

  diagnostics_internal_gpu_mark(
    in_window,
    part);
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_gpu_frame_end()
 *
 ******************************************************************************/
void diagnostics_internal_gpu_frame_end(
  const struct window_t* in_window)
{
  struct diagnostics_internal_gpu_window_t* gpu_window =
    _diagnostics_find_gpu_window(
      in_window,
      false);

  if ((gpu_window == NULL) ||
      !gpu_window->is_frame_open)
  {
    return;
  }

  struct diagnostics_internal_gpu_frame_t* frame = &(gpu_window->frames[gpu_window->current_frame]);

  frame->frame_zone_id =
    _diagnostics_get_window_zone(
      in_window,
      "frame",
      true);
  frame->is_pending = true;

  gpu_window->current_frame =
    (gpu_window->current_frame + 1) % DIAGNOSTICS_INTERNAL_GPU_FRAME_DELAY;
  gpu_window->is_frame_open = false;
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_gpu_cleanup()
 *
 ******************************************************************************/
void diagnostics_internal_gpu_cleanup(
  const struct window_t* in_window)
{
  struct diagnostics_internal_gpu_t* gpu = &(diagnostics_state.gpu);
  struct diagnostics_internal_gpu_window_t* gpu_window =
    _diagnostics_find_gpu_window(
      in_window,
      false);

  if (gpu_window == NULL) {
    return;
  }

  if (gpu_window->is_created) {
    for (uint8_t i = 0; i < DIAGNOSTICS_INTERNAL_GPU_FRAME_DELAY; i++) {
      render_api_delete_timestamps(
        gpu_window->frames[i].timestamp_names,
        DIAGNOSTICS_INTERNAL_GPU_MAX_MARKS);
    }
  }

  // The last window's timing takes the slot, keeping the rest packed
  gpu->window_count--;
  *gpu_window = gpu->windows[gpu->window_count];
  memset(
    &(gpu->windows[gpu->window_count]),
    0,
    sizeof(struct diagnostics_internal_gpu_window_t));
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_get_state()
 *
 ******************************************************************************/
const struct diagnostics_internal_state_t* diagnostics_internal_get_state(void)
{
  return &(diagnostics_state);
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_get_time_seconds()
 *
 ******************************************************************************/
double diagnostics_internal_get_time_seconds(void)
{
  struct timespec now;
  clock_gettime(
    CLOCK_MONOTONIC,
    &(now));

  return ((double)now.tv_sec + ((double)now.tv_nsec / 1000000000.0));
}

/*******************************************************************************
 *
 *   Function: diagnostics_internal_history_summarize()
 *
 ******************************************************************************/
void diagnostics_internal_history_summarize(
  const struct diagnostics_internal_history_t* in_history,
  struct diagnostics_internal_summary_t* out_summary)
{
  memset(
    out_summary,
    0,
    sizeof(struct diagnostics_internal_summary_t));

  if (in_history->count == 0) {
    return;
  }

  // The newest value sits just behind the next to be written
  const uint32_t latest_index =
    (in_history->next_index + DIAGNOSTICS_INTERNAL_HISTORY_LENGTH - 1) %
    DIAGNOSTICS_INTERNAL_HISTORY_LENGTH;
  out_summary->latest = in_history->values[latest_index];

  // The ring's order doesn't matter once the values are sorted
  static float sorted_values[DIAGNOSTICS_INTERNAL_HISTORY_LENGTH];
  memcpy(
    sorted_values,
    in_history->values,
    (in_history->count * sizeof(float)));
  qsort(
    sorted_values,
    in_history->count,
    sizeof(float),
    &(_diagnostics_compare_floats));

  double sum = 0.0;
  for (uint32_t i = 0; i < in_history->count; i++) {
    sum += sorted_values[i];
  }

  const uint32_t percentile_99_index = ((in_history->count - 1) * 99) / 100;

  out_summary->average = (float)(sum / in_history->count);
  out_summary->percentile_99 = sorted_values[percentile_99_index];
  out_summary->maximum = sorted_values[in_history->count - 1];
}

/*******************************************************************************
 *
 *   Function: _diagnostics_register_zone()
 *
 ******************************************************************************/
static uint16_t _diagnostics_register_zone(
  const char* in_name,
  bool in_is_gpu)
{
  for (uint16_t i = 0; i < diagnostics_state.zone_count; i++) {
    if ((diagnostics_state.zones[i].is_gpu == in_is_gpu) &&
        (strncmp(
          diagnostics_state.zones[i].name,
          in_name,
          (DIAGNOSTICS_INTERNAL_NAME_LENGTH - 1)) == 0))
    {
      return i;
    }
  }

  if (diagnostics_state.zone_count >= DIAGNOSTICS_INTERNAL_MAX_ZONES) {
    return DIAGNOSTICS_ZONE_INVALID;
  }

  const uint16_t zone_id = diagnostics_state.zone_count;
  diagnostics_state.zone_count++;

  snprintf(
    diagnostics_state.zones[zone_id].name,
    DIAGNOSTICS_INTERNAL_NAME_LENGTH,
    "%s",
    in_name);
  diagnostics_state.zones[zone_id].is_gpu = in_is_gpu;

  return zone_id;
}

/*******************************************************************************
 *
 *   Function: _diagnostics_get_window_zone()
 *
 ******************************************************************************/
static uint16_t _diagnostics_get_window_zone(
  const struct window_t* in_window,
  const char* in_part,
  bool in_is_gpu)
{
  char name[DIAGNOSTICS_INTERNAL_NAME_LENGTH];
  snprintf(
    name,
    sizeof(name),
    "%s %s",
    (in_window->metadata.tag != NULL) ? in_window->metadata.tag : "window",
    in_part);

  // Looked up by name every time, so a window gone can't leave a stale zone
  //  to one that takes its place
  return _diagnostics_register_zone(
    name,
    in_is_gpu);
}

/*******************************************************************************
 *
 *   Function: _diagnostics_find_gpu_window()
 *
 ******************************************************************************/
static struct diagnostics_internal_gpu_window_t* _diagnostics_find_gpu_window(
  const struct window_t* in_window,
  bool in_should_create)
{
  struct diagnostics_internal_gpu_t* gpu = &(diagnostics_state.gpu);

  for (uint8_t i = 0; i < gpu->window_count; i++) {
    if (gpu->windows[i].window == in_window) {
      return &(gpu->windows[i]);
    }
  }

  if (!in_should_create ||
      (gpu->window_count >= DIAGNOSTICS_INTERNAL_GPU_MAX_WINDOWS))
  {
    return NULL;
  }

  struct diagnostics_internal_gpu_window_t* gpu_window = &(gpu->windows[gpu->window_count]);
  gpu->window_count++;

  memset(
    gpu_window,
    0,
    sizeof(struct diagnostics_internal_gpu_window_t));
  gpu_window->window = in_window;

  return gpu_window;
}

/*******************************************************************************
 *
 *   Function: _diagnostics_collect_gpu_frames()
 *
 ******************************************************************************/
static void _diagnostics_collect_gpu_frames(
  struct diagnostics_internal_gpu_window_t* inout_gpu_window)
{
  for (uint8_t i = 0; i < DIAGNOSTICS_INTERNAL_GPU_FRAME_DELAY; i++) {
    struct diagnostics_internal_gpu_frame_t* frame = &(inout_gpu_window->frames[i]);

    if (!frame->is_pending) {
      continue;
    }

    if (frame->mark_count == 0) {
      frame->is_pending = false;
      continue;
    }

    // A frame is charged only once every one of its timestamps is ready, as
    //  their results needn't arrive in order
    uint64_t times_nanoseconds[DIAGNOSTICS_INTERNAL_GPU_MAX_MARKS];
    bool is_ready = true;

    for (uint8_t mark = 0; is_ready && (mark < frame->mark_count); mark++) {
      is_ready =
        render_api_read_timestamp(
          frame->timestamp_names[mark],
          &(times_nanoseconds[mark]));
    }

    if (!is_ready) {
      continue;
    }

    for (uint8_t mark = 1; mark < frame->mark_count; mark++) {
      if (frame->zone_ids[mark] < diagnostics_state.zone_count) {
        diagnostics_state.zones[frame->zone_ids[mark]].frame_seconds +=
          ((double)(times_nanoseconds[mark] - times_nanoseconds[mark - 1]) / 1000000000.0);
      }
    }

    if (frame->frame_zone_id < diagnostics_state.zone_count) {
      diagnostics_state.zones[frame->frame_zone_id].frame_seconds +=
        ((double)(times_nanoseconds[frame->mark_count - 1] - times_nanoseconds[0]) /
         1000000000.0);
    }

    frame->is_pending = false;
  }
}

/*******************************************************************************
 *
 *   Function: _diagnostics_history_push()
 *
 ******************************************************************************/
static void _diagnostics_history_push(
  struct diagnostics_internal_history_t* inout_history,
  float in_value)
{
  inout_history->values[inout_history->next_index] = in_value;
  inout_history->next_index = (inout_history->next_index + 1) % DIAGNOSTICS_INTERNAL_HISTORY_LENGTH;

  if (inout_history->count < DIAGNOSTICS_INTERNAL_HISTORY_LENGTH) {
    inout_history->count++;
  }
}

/*******************************************************************************
 *
 *   Function: _diagnostics_record_heap()
 *
 ******************************************************************************/
static void _diagnostics_record_heap(void)
{
  size_t live_bytes = 0;
  uint64_t frame_allocations = 0;

  for (int tag = 0; tag < ALLOCATOR_TAG_COUNT; tag++) {
    struct allocator_tag_stats_t stats;
    diagnostics_state.is_heap_counted =
      allocator_get_tag_stats(
        (enum allocator_tag_t)tag,
        &(stats));

    live_bytes += stats.live_bytes;

    diagnostics_state.tag_frame_allocations[tag] =
      (stats.total_count - diagnostics_state.tag_total_counts[tag]);
    diagnostics_state.tag_total_counts[tag] = stats.total_count;

    frame_allocations += diagnostics_state.tag_frame_allocations[tag];
  }

  _diagnostics_history_push(
    &(diagnostics_state.heap_live_megabytes),
    (float)((double)live_bytes / (1024.0 * 1024.0)));
  _diagnostics_history_push(
    &(diagnostics_state.heap_allocations),
    (float)frame_allocations);
}

/*******************************************************************************
 *
 *   Function: _diagnostics_record_resources()
 *
 ******************************************************************************/
static void _diagnostics_record_resources(void)
{
  size_t live_bytes = 0;

  for (int kind = 0; kind < RESOURCE_KIND_COUNT; kind++) {
    struct resource_kind_stats_t stats;
    resource_get_kind_stats(
      (enum resource_kind_t)kind,
      &(stats));

    live_bytes += stats.live_bytes;
  }

  _diagnostics_history_push(
    &(diagnostics_state.resource_live_megabytes),
    (float)((double)live_bytes / (1024.0 * 1024.0)));
}

/*******************************************************************************
 *
 *   Function: _diagnostics_describe_event()
 *
 ******************************************************************************/
static void _diagnostics_describe_event(
  const struct event_base_t* in_event,
  char* out_text,
  size_t in_text_length)
{
  out_text[0] = '\0';

  switch (in_event->event_type)
  {
    case EVENT_TYPE_WINDOW_RESIZE:
    {
      const struct event_window_resize_t* event =
        container_of(
          in_event,
          struct event_window_resize_t,
          base);

      snprintf(
        out_text,
        in_text_length,
        "%d x %d",
        event->width,
        event->height);
    }
    break;

    case EVENT_TYPE_WINDOW_FOCUS:
    {
      const struct event_window_focus_t* event =
        container_of(
          in_event,
          struct event_window_focus_t,
          base);

      snprintf(
        out_text,
        in_text_length,
        "%s",
        event->is_focused ? "gained" : "lost");
    }
    break;

    case EVENT_TYPE_APP_TICK:
    {
      const struct event_app_tick_t* event =
        container_of(
          in_event,
          struct event_app_tick_t,
          base);

      snprintf(
        out_text,
        in_text_length,
        "%.2f ms since the last",
        (event->delta_time_seconds * 1000.0f));
    }
    break;

    case EVENT_TYPE_KEY_PRESSED:
    {
      const struct event_key_pressed_t* event =
        container_of(
          in_event,
          struct event_key_pressed_t,
          base);

      snprintf(
        out_text,
        in_text_length,
        "key %d, modifiers 0x%02x%s",
        (int)event->key,
        (unsigned int)event->modifiers,
        event->is_repeat ? ", repeating" : "");
    }
    break;

    case EVENT_TYPE_KEY_RELEASED:
    {
      const struct event_key_released_t* event =
        container_of(
          in_event,
          struct event_key_released_t,
          base);

      snprintf(
        out_text,
        in_text_length,
        "key %d, modifiers 0x%02x",
        (int)event->key,
        (unsigned int)event->modifiers);
    }
    break;

    case EVENT_TYPE_KEY_TYPED:
    {
      const struct event_key_typed_t* event =
        container_of(
          in_event,
          struct event_key_typed_t,
          base);

      // Printable ASCII is shown as itself as well
      if ((event->codepoint >= 0x20U) &&
          (event->codepoint < 0x7FU))
      {
        snprintf(
          out_text,
          in_text_length,
          "'%c' (U+%04X)",
          (char)event->codepoint,
          (unsigned int)event->codepoint);
      }
      else {
        snprintf(
          out_text,
          in_text_length,
          "U+%04X",
          (unsigned int)event->codepoint);
      }
    }
    break;

    case EVENT_TYPE_MOUSE_BUTTON:
    {
      const struct event_mouse_button_t* event =
        container_of(
          in_event,
          struct event_mouse_button_t,
          base);

      snprintf(
        out_text,
        in_text_length,
        "button %d %s, modifiers 0x%02x",
        event->button,
        (event->action == 1) ? "pressed" : "released",
        (unsigned int)event->mods);
    }
    break;

    case EVENT_TYPE_MOUSE_MOVED:
    {
      const struct event_mouse_moved_t* event =
        container_of(
          in_event,
          struct event_mouse_moved_t,
          base);

      snprintf(
        out_text,
        in_text_length,
        "%.1f, %.1f",
        (double)event->x,
        (double)event->y);
    }
    break;

    case EVENT_TYPE_MOUSE_SCROLLED:
    {
      const struct event_mouse_scrolled_t* event =
        container_of(
          in_event,
          struct event_mouse_scrolled_t,
          base);

      snprintf(
        out_text,
        in_text_length,
        "%.2f, %.2f",
        (double)event->x_offset,
        (double)event->y_offset);
    }
    break;

    case EVENT_TYPE_MOUSE_ENTERED:
    {
      const struct event_mouse_entered_t* event =
        container_of(
          in_event,
          struct event_mouse_entered_t,
          base);

      snprintf(
        out_text,
        in_text_length,
        "%s",
        event->is_entered ? "entered" : "left");
    }
    break;

    default:
    {
    }
    break;
  }
}

/*******************************************************************************
 *
 *   Function: _diagnostics_find_renderer()
 *
 ******************************************************************************/
static uint8_t _diagnostics_find_renderer(
  const struct renderer_base_t* in_renderer)
{
  struct diagnostics_internal_rendering_t* rendering = &(diagnostics_state.rendering);

  uint8_t renderer_index = 0;
  while ((renderer_index < rendering->renderer_count) &&
         (rendering->renderers[renderer_index].renderer != in_renderer))
  {
    renderer_index++;
  }

  if (renderer_index == rendering->renderer_count) {
    if (rendering->renderer_count < DIAGNOSTICS_INTERNAL_MAX_RENDERERS) {
      rendering->renderer_count++;
    }
    else {
      renderer_index = (DIAGNOSTICS_INTERNAL_MAX_RENDERERS - 1);
    }

    rendering->renderers[renderer_index].renderer = in_renderer;
  }

  struct diagnostics_internal_renderer_t* renderer = &(rendering->renderers[renderer_index]);

  // Named afresh each time, as a renderer gone may leave its address to
  //  another
  if (in_renderer == NULL) {
    snprintf(
      renderer->name,
      sizeof(renderer->name),
      "outside any scene");
  }
  else {
    const char* window_tag =
      ((in_renderer->parent_window != NULL) &&
       (in_renderer->parent_window->metadata.tag != NULL)) ?
        in_renderer->parent_window->metadata.tag :
        "";

    snprintf(
      renderer->name,
      sizeof(renderer->name),
      "%s: %s",
      window_tag,
      (in_renderer->tag != NULL) ? in_renderer->tag : "unnamed");
  }

  renderer->last_frame = diagnostics_state.frame_count;

  return renderer_index;
}

/*******************************************************************************
 *
 *   Function: _diagnostics_record_rendering()
 *
 ******************************************************************************/
static void _diagnostics_record_rendering(void)
{
  struct diagnostics_internal_rendering_t* rendering = &(diagnostics_state.rendering);

  for (uint8_t i = 0; i < rendering->renderer_count; i++) {
    struct diagnostics_internal_renderer_t* renderer = &(rendering->renderers[i]);

    renderer->draw_calls = renderer->frame_draw_calls;
    renderer->elements = renderer->frame_elements;
    _diagnostics_history_push(
      &(renderer->draw_calls_per_frame),
      (float)renderer->frame_draw_calls);

    renderer->frame_draw_calls = 0;
    renderer->frame_elements = 0;
  }

  _diagnostics_history_push(
    &(rendering->draw_calls_per_frame),
    (float)rendering->frame_draw_calls);
  rendering->frame_draw_calls = 0;
}

/*******************************************************************************
 *
 *   Function: _diagnostics_find_event_window_tag()
 *
 ******************************************************************************/
static const char* _diagnostics_find_event_window_tag(
  const struct application_t* in_app,
  const struct event_base_t* in_event)
{
  // Only these carry their window; the lost focus and moved events have no
  //  structure of their own yet
  const void* window_handle = NULL;

  switch (in_event->event_type)
  {
    case EVENT_TYPE_WINDOW_CLOSE:
    {
      window_handle =
        container_of(
          in_event,
          struct event_window_close_t,
          base)->window_handle;
    }
    break;

    case EVENT_TYPE_WINDOW_RESIZE:
    {
      window_handle =
        container_of(
          in_event,
          struct event_window_resize_t,
          base)->window_handle;
    }
    break;

    case EVENT_TYPE_WINDOW_FOCUS:
    {
      window_handle =
        container_of(
          in_event,
          struct event_window_focus_t,
          base)->window_handle;
    }
    break;

    case EVENT_TYPE_KEY_PRESSED:
    {
      window_handle =
        container_of(
          in_event,
          struct event_key_pressed_t,
          base)->window_handle;
    }
    break;

    case EVENT_TYPE_KEY_RELEASED:
    {
      window_handle =
        container_of(
          in_event,
          struct event_key_released_t,
          base)->window_handle;
    }
    break;

    case EVENT_TYPE_KEY_TYPED:
    {
      window_handle =
        container_of(
          in_event,
          struct event_key_typed_t,
          base)->window_handle;
    }
    break;

    case EVENT_TYPE_MOUSE_BUTTON:
    {
      window_handle =
        container_of(
          in_event,
          struct event_mouse_button_t,
          base)->window_handle;
    }
    break;

    case EVENT_TYPE_MOUSE_MOVED:
    {
      window_handle =
        container_of(
          in_event,
          struct event_mouse_moved_t,
          base)->window_handle;
    }
    break;

    case EVENT_TYPE_MOUSE_SCROLLED:
    {
      window_handle =
        container_of(
          in_event,
          struct event_mouse_scrolled_t,
          base)->window_handle;
    }
    break;

    case EVENT_TYPE_MOUSE_ENTERED:
    {
      window_handle =
        container_of(
          in_event,
          struct event_mouse_entered_t,
          base)->window_handle;
    }
    break;

    default:
    {
    }
    break;
  }

  if (window_handle == NULL) {
    return "";
  }

  for (uint8_t i = 0; i < in_app->num_windows; i++) {
    const struct window_t* window = in_app->windows[i];

    if ((window->context_data.window_handle == window_handle) &&
        (window->metadata.tag != NULL))
    {
      return window->metadata.tag;
    }
  }

  return "";
}

/*******************************************************************************
 *
 *   Function: _diagnostics_compare_floats()
 *
 ******************************************************************************/
static int _diagnostics_compare_floats(
  const void* in_left,
  const void* in_right)
{
  const float left = *((const float*)in_left);
  const float right = *((const float*)in_right);

  return (left > right) - (left < right);
}
