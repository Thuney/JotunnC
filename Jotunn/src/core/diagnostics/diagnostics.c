#define _POSIX_C_SOURCE 199309L
#include "diagnostics_internal.h"

#include "allocator.h"
#include "resource.h"

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
  .zone_count = DIAGNOSTICS_INTERNAL_ZONE_ENGINE_COUNT
};

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Adds a value to a history, displacing the oldest once it's full
//
//  in:     in_value
//  in/out: inout_history
static void _diagnostics_history_push(
  struct diagnostics_internal_history_t* inout_history,
  const float in_value);

// Records the heap's counts for the frame just ended
static void _diagnostics_record_heap(void);

// Records the graphics memory held at the end of the frame
static void _diagnostics_record_resources(void);

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
  for (uint16_t i = 0; i < diagnostics_state.zone_count; i++)
  {
    if (strncmp(
          diagnostics_state.zones[i].name,
          in_name,
          (DIAGNOSTICS_INTERNAL_NAME_LENGTH - 1)) == 0)
    {
      return i;
    }
  }

  if (diagnostics_state.zone_count >= DIAGNOSTICS_INTERNAL_MAX_ZONES)
  {
    return DIAGNOSTICS_ZONE_INVALID;
  }

  const uint16_t zone_id = diagnostics_state.zone_count;
  diagnostics_state.zone_count++;

  snprintf(
    diagnostics_state.zones[zone_id].name,
    DIAGNOSTICS_INTERNAL_NAME_LENGTH,
    "%s",
    in_name);

  return zone_id;
}

/*******************************************************************************
 *
 *   Function: diagnostics_zone_begin()
 *
 ******************************************************************************/
void diagnostics_zone_begin(
  const uint16_t in_zone_id)
{
  if (in_zone_id >= diagnostics_state.zone_count)
  {
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
  const uint16_t in_zone_id)
{
  if (in_zone_id >= diagnostics_state.zone_count)
  {
    return;
  }

  struct diagnostics_internal_zone_t* zone = &(diagnostics_state.zones[in_zone_id]);

  if (zone->is_open)
  {
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

  struct diagnostics_internal_tab_t* tab =
    &(diagnostics_state.tabs[diagnostics_state.tab_count]);
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
  const float in_target_interval_seconds)
{
  const double now_seconds = diagnostics_internal_get_time_seconds();

  if (diagnostics_state.frame_count == 0)
  {
    diagnostics_state.start_seconds = now_seconds;

    // Allocations made before the first frame aren't counted against it
    for (int tag = 0; tag < ALLOCATOR_TAG_COUNT; tag++)
    {
      struct allocator_tag_stats_t stats;
      allocator_get_tag_stats(
        (enum allocator_tag_t)tag,
        &(stats));

      diagnostics_state.tag_total_counts[tag] = stats.total_count;
    }
  }
  else
  {
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

  for (uint16_t i = 0; i < diagnostics_state.zone_count; i++)
  {
    struct diagnostics_internal_zone_t* zone = &(diagnostics_state.zones[i]);

    // A zone still open is cut at the frame's end and carries on into the
    //  next
    if (zone->is_open)
    {
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
  const enum log_level_t in_level,
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
  if (log->count < DIAGNOSTICS_INTERNAL_LOG_LENGTH)
  {
    log->count++;
  }

  if ((unsigned int)in_level < (unsigned int)LOG_LEVEL_COUNT)
  {
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

  if (in_history->count == 0)
  {
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
  for (uint32_t i = 0; i < in_history->count; i++)
  {
    sum += sorted_values[i];
  }

  const uint32_t percentile_99_index = ((in_history->count - 1) * 99) / 100;

  out_summary->average = (float)(sum / in_history->count);
  out_summary->percentile_99 = sorted_values[percentile_99_index];
  out_summary->maximum = sorted_values[in_history->count - 1];
}

/*******************************************************************************
 *
 *   Function: _diagnostics_history_push()
 *
 ******************************************************************************/
static void _diagnostics_history_push(
  struct diagnostics_internal_history_t* inout_history,
  const float in_value)
{
  inout_history->values[inout_history->next_index] = in_value;
  inout_history->next_index =
    (inout_history->next_index + 1) % DIAGNOSTICS_INTERNAL_HISTORY_LENGTH;

  if (inout_history->count < DIAGNOSTICS_INTERNAL_HISTORY_LENGTH)
  {
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

  for (int tag = 0; tag < ALLOCATOR_TAG_COUNT; tag++)
  {
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

  for (int kind = 0; kind < RESOURCE_KIND_COUNT; kind++)
  {
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
