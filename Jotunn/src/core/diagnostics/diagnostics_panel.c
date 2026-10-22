#include "diagnostics_internal.h"

#include "allocator.h"
#include "framebuffer.h"
#include "resource.h"
#include "texture.h"

#include "cimgui.h"
#include "cimplot.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define _DIAGNOSTICS_PANEL_PLOT_HEIGHT_PIXELS 180.0f
#define _DIAGNOSTICS_PANEL_PREVIEW_SIZE_PIXELS 256.0f
#define _DIAGNOSTICS_PANEL_RESOURCE_LIST_HEIGHT_PIXELS 240.0f
#define _DIAGNOSTICS_PANEL_EVENT_LIST_HEIGHT_PIXELS 220.0f
#define _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS 72.0f
#define _DIAGNOSTICS_PANEL_BYTES_TEXT_LENGTH 32
#define _DIAGNOSTICS_PANEL_FILTER_TEXT_LENGTH 128

/*******************************************************************************
 *
 *   STATE
 *
 ******************************************************************************/

// Textures rendered into by the graphics API are stored bottom row first, so
//  previews are shown flipped unless asked otherwise
static bool is_preview_flipped = true;

// The Log tab's filters and whether it follows new messages
static bool is_log_level_shown[LOG_LEVEL_COUNT] = { true, true, true, true };
static char log_filter_text[_DIAGNOSTICS_PANEL_FILTER_TEXT_LENGTH] = "";
static bool is_log_auto_scrolled = true;

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Draws the Overview tab: frame rate, frame times and the loop's idle passes
//
//  in:     in_state
static void _diagnostics_panel_draw_overview(
  const struct diagnostics_internal_state_t* in_state);

// Draws the Timing tab: every zone's time per frame
//
//  in:     in_state
static void _diagnostics_panel_draw_timing(
  const struct diagnostics_internal_state_t* in_state);

// Draws the Rendering tab: draw calls per frame, by renderer
//
//  in:     in_state
static void _diagnostics_panel_draw_rendering(
  const struct diagnostics_internal_state_t* in_state);

// Draws the Memory tab: the heap, by allocator tag
//
//  in:     in_state
static void _diagnostics_panel_draw_memory(
  const struct diagnostics_internal_state_t* in_state);

// Draws the Resources tab: graphics resources, by kind and one by one
//
//  in:     in_state, in_target_texture
static void _diagnostics_panel_draw_resources(
  const struct diagnostics_internal_state_t* in_state,
  const struct texture_2d_t* in_target_texture);

// Draws the Events tab: counts and the latest of each type of event, and a log
//  of recent events
//
//  in:     in_state
static void _diagnostics_panel_draw_events(
  const struct diagnostics_internal_state_t* in_state);

// Draws the Log tab: the log's recent messages, filtered by level and text
//
//  in:     in_state
static void _diagnostics_panel_draw_log(
  const struct diagnostics_internal_state_t* in_state);

// Starts a table summarizing histories, one per row, with columns for the
//  latest value, the average, the 99th percentile and the maximum
//
//  in:     in_id, in_first_heading
//  ret:    true when the table is visible, to be ended with igEndTable()
static bool _diagnostics_panel_begin_summary_table(
  const char* in_id,
  const char* in_first_heading);

// Adds a history's row to a summary table
//
//  in:     in_label, in_history, in_decimal_places
static void _diagnostics_panel_draw_summary_row(
  const char* in_label,
  const struct diagnostics_internal_history_t* in_history,
  const int in_decimal_places);

// Starts a plot of histories against how many frames ago each value was
//  recorded. Its y axis starts at zero, with room above in_y_maximum so that
//  a steady line doesn't sit on the plot's edge
//
//  in:     in_id, in_y_label, in_y_maximum
//  ret:    true when the plot is visible, to be ended with ImPlot_EndPlot()
static bool _diagnostics_panel_begin_history_plot(
  const char* in_id,
  const char* in_y_label,
  const float in_y_maximum);

// Finds the largest value in a history
//
//  in:     in_history
//  ret:    The largest value; 0 when the history is empty
static float _diagnostics_panel_get_history_maximum(
  const struct diagnostics_internal_history_t* in_history);

// Plots a history as a line in the current plot
//
//  in:     in_label, in_history
static void _diagnostics_panel_plot_history(
  const char* in_label,
  const struct diagnostics_internal_history_t* in_history);

// Reads ImPlot's default plot item specification
//
//  ret:    A copy of the defaults
static ImPlotSpec_c _diagnostics_panel_get_default_spec(void);

// Shows a texture in a tooltip while the last item drawn is hovered
//
//  in:     in_texture, in_target_texture
static void _diagnostics_panel_draw_preview_tooltip(
  const struct texture_2d_t* in_texture,
  const struct texture_2d_t* in_target_texture);

// Writes a size in bytes in the largest unit that keeps it at least 1
//
//  in:     in_size_bytes, in_text_length
//  out:    out_text
static void _diagnostics_panel_format_bytes(
  const size_t in_size_bytes,
  char* out_text,
  const size_t in_text_length);

/*******************************************************************************
 *
 *   Function: diagnostics_internal_panel_draw()
 *
 ******************************************************************************/
void diagnostics_internal_panel_draw(
  bool* inout_is_open,
  const struct texture_2d_t* in_target_texture)
{
  const struct diagnostics_internal_state_t* state = diagnostics_internal_get_state();

  igSetNextWindowPos(
    (ImVec2_c){ 20.0f, 20.0f },
    ImGuiCond_FirstUseEver,
    (ImVec2_c){ 0.0f, 0.0f });
  igSetNextWindowSize(
    (ImVec2_c){ 600.0f, 520.0f },
    ImGuiCond_FirstUseEver);

  if (igBegin(
        "Jotunn Diagnostics (F3)",
        inout_is_open,
        0))
  {
    if (igBeginTabBar(
          "diagnostics_tabs",
          0))
    {
      if (igBeginTabItem(
            "Overview",
            NULL,
            0))
      {
        _diagnostics_panel_draw_overview(state);
        igEndTabItem();
      }

      if (igBeginTabItem(
            "Timing",
            NULL,
            0))
      {
        _diagnostics_panel_draw_timing(state);
        igEndTabItem();
      }

      if (igBeginTabItem(
            "Rendering",
            NULL,
            0))
      {
        _diagnostics_panel_draw_rendering(state);
        igEndTabItem();
      }

      if (igBeginTabItem(
            "Memory",
            NULL,
            0))
      {
        _diagnostics_panel_draw_memory(state);
        igEndTabItem();
      }

      if (igBeginTabItem(
            "Resources",
            NULL,
            0))
      {
        _diagnostics_panel_draw_resources(
          state,
          in_target_texture);
        igEndTabItem();
      }

      if (igBeginTabItem(
            "Events",
            NULL,
            0))
      {
        _diagnostics_panel_draw_events(state);
        igEndTabItem();
      }

      if (igBeginTabItem(
            "Log",
            NULL,
            0))
      {
        _diagnostics_panel_draw_log(state);
        igEndTabItem();
      }

      for (uint8_t i = 0; i < state->tab_count; i++)
      {
        const struct diagnostics_internal_tab_t* tab = &(state->tabs[i]);

        if (igBeginTabItem(
              tab->name,
              NULL,
              0))
        {
          tab->function_draw(tab->user_data);
          igEndTabItem();
        }
      }

      igEndTabBar();
    }
  }

  // Ended whether or not the window is collapsed
  igEnd();
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_draw_overview()
 *
 ******************************************************************************/
static void _diagnostics_panel_draw_overview(
  const struct diagnostics_internal_state_t* in_state)
{
  struct diagnostics_internal_summary_t interval;
  diagnostics_internal_history_summarize(
    &(in_state->frame_interval_milliseconds),
    &(interval));

  const float frames_per_second =
    (interval.average > 0.0f) ?
      (1000.0f / interval.average) :
      0.0f;
  const float target_frames_per_second =
    (in_state->target_interval_milliseconds > 0.0f) ?
      (1000.0f / in_state->target_interval_milliseconds) :
      0.0f;

  igText(
    "%.1f frames per second (target %.0f)",
    frames_per_second,
    target_frames_per_second);
  igText(
    "Frame %llu, running %.1f s",
    (unsigned long long)in_state->frame_count,
    (diagnostics_internal_get_time_seconds() - in_state->start_seconds));
  igTextDisabled(
    "Dear ImGui %s",
    igGetVersion());

  if (_diagnostics_panel_begin_summary_table(
        "overview_summary",
        "Per frame"))
  {
    _diagnostics_panel_draw_summary_row(
      "Interval (ms)",
      &(in_state->frame_interval_milliseconds),
      2);
    _diagnostics_panel_draw_summary_row(
      "Work (ms)",
      &(in_state->frame_work_milliseconds),
      2);
    _diagnostics_panel_draw_summary_row(
      "  in this overlay (ms)",
      &(in_state->zones[DIAGNOSTICS_INTERNAL_ZONE_OVERLAY].milliseconds),
      2);
    _diagnostics_panel_draw_summary_row(
      "Idle loop passes",
      &(in_state->idle_calls),
      0);

    igEndTable();
  }

  igTextDisabled("Idle loop passes count how often the application loop ran without");
  igTextDisabled("rendering while it waited for the next frame to be due");

  float y_maximum = in_state->target_interval_milliseconds;
  const float interval_maximum =
    _diagnostics_panel_get_history_maximum(&(in_state->frame_interval_milliseconds));
  const float work_maximum =
    _diagnostics_panel_get_history_maximum(&(in_state->frame_work_milliseconds));

  if (interval_maximum > y_maximum)
  {
    y_maximum = interval_maximum;
  }

  if (work_maximum > y_maximum)
  {
    y_maximum = work_maximum;
  }

  if (_diagnostics_panel_begin_history_plot(
        "##frame_times",
        "ms",
        y_maximum))
  {
    // Drawn first, so a frame interval right on target still shows over it
    ImPlotSpec_c target_spec = _diagnostics_panel_get_default_spec();
    target_spec.Flags = ImPlotInfLinesFlags_Horizontal;

    const double target_milliseconds = in_state->target_interval_milliseconds;
    ImPlot_PlotInfLines_doublePtr(
      "target",
      &(target_milliseconds),
      1,
      target_spec);

    _diagnostics_panel_plot_history(
      "interval",
      &(in_state->frame_interval_milliseconds));
    _diagnostics_panel_plot_history(
      "work",
      &(in_state->frame_work_milliseconds));

    ImPlot_EndPlot();
  }
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_draw_timing()
 *
 ******************************************************************************/
static void _diagnostics_panel_draw_timing(
  const struct diagnostics_internal_state_t* in_state)
{
  float y_maximum = 0.0f;

  if (_diagnostics_panel_begin_summary_table(
        "timing_summary",
        "Zone (ms)"))
  {
    for (uint16_t i = 0; i < in_state->zone_count; i++)
    {
      _diagnostics_panel_draw_summary_row(
        in_state->zones[i].name,
        &(in_state->zones[i].milliseconds),
        2);
    }

    igEndTable();
  }

  for (uint16_t i = 0; i < in_state->zone_count; i++)
  {
    const float zone_maximum =
      _diagnostics_panel_get_history_maximum(&(in_state->zones[i].milliseconds));

    if (zone_maximum > y_maximum)
    {
      y_maximum = zone_maximum;
    }
  }

  if (_diagnostics_panel_begin_history_plot(
        "##zone_times",
        "ms",
        y_maximum))
  {
    for (uint16_t i = 0; i < in_state->zone_count; i++)
    {
      _diagnostics_panel_plot_history(
        in_state->zones[i].name,
        &(in_state->zones[i].milliseconds));
    }

    ImPlot_EndPlot();
  }
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_draw_rendering()
 *
 ******************************************************************************/
static void _diagnostics_panel_draw_rendering(
  const struct diagnostics_internal_state_t* in_state)
{
  const struct diagnostics_internal_rendering_t* rendering = &(in_state->rendering);

  if (_diagnostics_panel_begin_summary_table(
        "draw_call_summary",
        "Draw calls"))
  {
    _diagnostics_panel_draw_summary_row(
      "Every renderer",
      &(rendering->draw_calls_per_frame),
      0);

    // Only the renderers at work in the last frame, as one gone may leave
    //  its slot behind
    for (uint8_t i = 0; i < rendering->renderer_count; i++)
    {
      const struct diagnostics_internal_renderer_t* renderer = &(rendering->renderers[i]);

      if ((renderer->last_frame + 1) >= in_state->frame_count)
      {
        _diagnostics_panel_draw_summary_row(
          renderer->name,
          &(renderer->draw_calls_per_frame),
          0);
      }
    }

    igEndTable();
  }

  if (igBeginTable(
        "renderer_elements",
        3,
        (ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV),
        (ImVec2_c){ 0.0f, 0.0f },
        0.0f))
  {
    igTableSetupColumn(
      "Last frame",
      ImGuiTableColumnFlags_WidthStretch,
      0.0f,
      0);
    igTableSetupColumn(
      "Draw calls",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableSetupColumn(
      "Elements",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableHeadersRow();

    for (uint8_t i = 0; i < rendering->renderer_count; i++)
    {
      const struct diagnostics_internal_renderer_t* renderer = &(rendering->renderers[i]);

      if ((renderer->last_frame + 1) < in_state->frame_count)
      {
        continue;
      }

      igTableNextRow(
        0,
        0.0f);
      igTableNextColumn();
      igText(
        "%s",
        renderer->name);
      igTableNextColumn();
      igText(
        "%u",
        renderer->draw_calls);
      igTableNextColumn();
      igText(
        "%llu",
        (unsigned long long)renderer->elements);
    }

    igEndTable();
  }

  igTextDisabled("Elements are the indices, or vertices, the draw calls drew");

  if (_diagnostics_panel_begin_history_plot(
        "##draw_calls",
        "draw calls",
        _diagnostics_panel_get_history_maximum(&(rendering->draw_calls_per_frame))))
  {
    _diagnostics_panel_plot_history(
      "every renderer",
      &(rendering->draw_calls_per_frame));

    for (uint8_t i = 0; i < rendering->renderer_count; i++)
    {
      const struct diagnostics_internal_renderer_t* renderer = &(rendering->renderers[i]);

      if ((renderer->last_frame + 1) >= in_state->frame_count)
      {
        _diagnostics_panel_plot_history(
          renderer->name,
          &(renderer->draw_calls_per_frame));
      }
    }

    ImPlot_EndPlot();
  }
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_draw_memory()
 *
 ******************************************************************************/
static void _diagnostics_panel_draw_memory(
  const struct diagnostics_internal_state_t* in_state)
{
  if (!in_state->is_heap_counted)
  {
    igTextDisabled("Heap allocations aren't counted in this build");
    return;
  }

  if (igBeginTable(
        "memory_tags",
        6,
        (ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV),
        (ImVec2_c){ 0.0f, 0.0f },
        0.0f))
  {
    igTableSetupColumn(
      "Tag",
      ImGuiTableColumnFlags_WidthStretch,
      0.0f,
      0);
    igTableSetupColumn(
      "Live",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableSetupColumn(
      "Peak",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableSetupColumn(
      "Blocks",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableSetupColumn(
      "Total",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableSetupColumn(
      "This frame",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableHeadersRow();

    for (int tag = 0; tag < ALLOCATOR_TAG_COUNT; tag++)
    {
      struct allocator_tag_stats_t stats;
      allocator_get_tag_stats(
        (enum allocator_tag_t)tag,
        &(stats));

      char live_text[_DIAGNOSTICS_PANEL_BYTES_TEXT_LENGTH];
      char peak_text[_DIAGNOSTICS_PANEL_BYTES_TEXT_LENGTH];
      _diagnostics_panel_format_bytes(
        stats.live_bytes,
        live_text,
        sizeof(live_text));
      _diagnostics_panel_format_bytes(
        stats.peak_live_bytes,
        peak_text,
        sizeof(peak_text));

      igTableNextRow(
        0,
        0.0f);
      igTableNextColumn();
      igText(
        "%s",
        allocator_get_tag_name((enum allocator_tag_t)tag));
      igTableNextColumn();
      igText(
        "%s",
        live_text);
      igTableNextColumn();
      igText(
        "%s",
        peak_text);
      igTableNextColumn();
      igText(
        "%llu",
        (unsigned long long)stats.live_count);
      igTableNextColumn();
      igText(
        "%llu",
        (unsigned long long)stats.total_count);
      igTableNextColumn();
      igText(
        "%llu",
        (unsigned long long)in_state->tag_frame_allocations[tag]);
    }

    igEndTable();
  }

  if (_diagnostics_panel_begin_history_plot(
        "##heap_live",
        "MB",
        _diagnostics_panel_get_history_maximum(&(in_state->heap_live_megabytes))))
  {
    _diagnostics_panel_plot_history(
      "heap",
      &(in_state->heap_live_megabytes));

    ImPlot_EndPlot();
  }

  // Once running, a frame shouldn't need the heap at all
  if (_diagnostics_panel_begin_history_plot(
        "##heap_allocations",
        "allocations",
        _diagnostics_panel_get_history_maximum(&(in_state->heap_allocations))))
  {
    _diagnostics_panel_plot_history(
      "allocations per frame",
      &(in_state->heap_allocations));

    ImPlot_EndPlot();
  }
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_draw_resources()
 *
 ******************************************************************************/
static void _diagnostics_panel_draw_resources(
  const struct diagnostics_internal_state_t* in_state,
  const struct texture_2d_t* in_target_texture)
{
  if (igBeginTable(
        "resource_kinds",
        5,
        (ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV),
        (ImVec2_c){ 0.0f, 0.0f },
        0.0f))
  {
    igTableSetupColumn(
      "Kind",
      ImGuiTableColumnFlags_WidthStretch,
      0.0f,
      0);
    igTableSetupColumn(
      "Live",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableSetupColumn(
      "Memory",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableSetupColumn(
      "Peak",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableSetupColumn(
      "Created",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableHeadersRow();

    for (int kind = 0; kind < RESOURCE_KIND_COUNT; kind++)
    {
      struct resource_kind_stats_t stats;
      resource_get_kind_stats(
        (enum resource_kind_t)kind,
        &(stats));

      char live_text[_DIAGNOSTICS_PANEL_BYTES_TEXT_LENGTH];
      char peak_text[_DIAGNOSTICS_PANEL_BYTES_TEXT_LENGTH];
      _diagnostics_panel_format_bytes(
        stats.live_bytes,
        live_text,
        sizeof(live_text));
      _diagnostics_panel_format_bytes(
        stats.peak_live_bytes,
        peak_text,
        sizeof(peak_text));

      igTableNextRow(
        0,
        0.0f);
      igTableNextColumn();
      igText(
        "%s",
        resource_get_kind_name((enum resource_kind_t)kind));
      igTableNextColumn();
      igText(
        "%llu",
        (unsigned long long)stats.live_count);
      igTableNextColumn();
      igText(
        "%s",
        live_text);
      igTableNextColumn();
      igText(
        "%s",
        peak_text);
      igTableNextColumn();
      igText(
        "%llu",
        (unsigned long long)stats.total_count);
    }

    igEndTable();
  }

  igTextDisabled("Memory is estimated from each resource's size and format");

  igSeparatorText("Live resources");
  igCheckbox(
    "Flip previews vertically",
    &(is_preview_flipped));
  igTextDisabled("Hover over a texture or framebuffer to preview it");

  if (igBeginTable(
        "resource_list",
        3,
        (ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY),
        (ImVec2_c){ 0.0f, _DIAGNOSTICS_PANEL_RESOURCE_LIST_HEIGHT_PIXELS },
        0.0f))
  {
    igTableSetupScrollFreeze(
      0,
      1);
    igTableSetupColumn(
      "Kind",
      0,
      0.0f,
      0);
    igTableSetupColumn(
      "Name",
      ImGuiTableColumnFlags_WidthStretch,
      0.0f,
      0);
    igTableSetupColumn(
      "Memory",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableHeadersRow();

    for (const struct resource_t* resource = resource_get_first();
         resource != NULL;
         resource = resource->next)
    {
      char size_text[_DIAGNOSTICS_PANEL_BYTES_TEXT_LENGTH];
      _diagnostics_panel_format_bytes(
        resource->size_bytes,
        size_text,
        sizeof(size_text));

      igTableNextRow(
        0,
        0.0f);
      igTableNextColumn();
      igText(
        "%s",
        resource_get_kind_name(resource->kind));
      igTableNextColumn();
      igText(
        "%s",
        (resource->name != NULL) ? resource->name : "");

      // A framebuffer previews as its color attachment
      const struct framebuffer_t* framebuffer = resource_get_framebuffer(resource);
      const struct texture_2d_t* preview_texture =
        (framebuffer != NULL) ?
          &(framebuffer->color_buffer.texture) :
          resource_get_texture(resource);

      if (preview_texture != NULL)
      {
        _diagnostics_panel_draw_preview_tooltip(
          preview_texture,
          in_target_texture);
      }

      igTableNextColumn();
      igText(
        "%s",
        size_text);
    }

    igEndTable();
  }

  if (_diagnostics_panel_begin_history_plot(
        "##resource_memory",
        "MB",
        _diagnostics_panel_get_history_maximum(&(in_state->resource_live_megabytes))))
  {
    _diagnostics_panel_plot_history(
      "graphics memory",
      &(in_state->resource_live_megabytes));

    ImPlot_EndPlot();
  }
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_draw_events()
 *
 ******************************************************************************/
static void _diagnostics_panel_draw_events(
  const struct diagnostics_internal_state_t* in_state)
{
  const struct diagnostics_internal_events_t* events = &(in_state->events);

  if (igBeginTable(
        "event_types",
        5,
        (ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV),
        (ImVec2_c){ 0.0f, 0.0f },
        0.0f))
  {
    igTableSetupColumn(
      "Type",
      ImGuiTableColumnFlags_WidthFixed,
      130.0f,
      0);
    igTableSetupColumn(
      "Count",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableSetupColumn(
      "Handled",
      ImGuiTableColumnFlags_WidthFixed,
      _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
      0);
    igTableSetupColumn(
      "Logged",
      ImGuiTableColumnFlags_WidthFixed,
      48.0f,
      0);
    igTableSetupColumn(
      "Latest",
      ImGuiTableColumnFlags_WidthStretch,
      0.0f,
      0);
    igTableHeadersRow();

    // Only the types seen so far, to keep the table short
    for (int type = 0; type < EVENT_TYPE_COUNT; type++)
    {
      const struct diagnostics_internal_event_type_stats_t* type_stats = &(events->types[type]);

      if (type_stats->count == 0)
      {
        continue;
      }

      igPushID_Int(type);

      igTableNextRow(
        0,
        0.0f);
      igTableNextColumn();
      igText(
        "%s",
        diagnostics_internal_event_get_type_name((enum event_type_t)type));
      igTableNextColumn();
      igText(
        "%llu",
        (unsigned long long)type_stats->count);
      igTableNextColumn();
      igText(
        "%llu",
        (unsigned long long)type_stats->handled_count);
      igTableNextColumn();

      bool is_logged = !type_stats->is_excluded;
      if (igCheckbox(
            "##logged",
            &(is_logged)))
      {
        diagnostics_internal_events_set_excluded(
          (enum event_type_t)type,
          !is_logged);
      }

      igTableNextColumn();
      if (type_stats->has_latest)
      {
        igText(
          "frame %llu  %s  %s",
          (unsigned long long)type_stats->latest.frame,
          type_stats->latest.window_tag,
          type_stats->latest.details);
      }

      igPopID();
    }

    igEndTable();
  }

  bool is_paused = events->is_paused;
  if (igCheckbox(
        "Pause the log",
        &(is_paused)))
  {
    diagnostics_internal_events_set_paused(is_paused);
  }

  igSameLine(
    0.0f,
    -1.0f);

  if (igSmallButton("Clear"))
  {
    diagnostics_internal_events_clear();
  }

  if (igBeginTable(
        "event_log",
        5,
        (ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY),
        (ImVec2_c){ 0.0f, _DIAGNOSTICS_PANEL_EVENT_LIST_HEIGHT_PIXELS },
        0.0f))
  {
    igTableSetupScrollFreeze(
      0,
      1);
    igTableSetupColumn(
      "Frame",
      ImGuiTableColumnFlags_WidthFixed,
      56.0f,
      0);
    igTableSetupColumn(
      "Type",
      ImGuiTableColumnFlags_WidthFixed,
      130.0f,
      0);
    igTableSetupColumn(
      "Window",
      ImGuiTableColumnFlags_WidthFixed,
      110.0f,
      0);
    igTableSetupColumn(
      "Handled",
      ImGuiTableColumnFlags_WidthFixed,
      56.0f,
      0);
    igTableSetupColumn(
      "Details",
      ImGuiTableColumnFlags_WidthStretch,
      0.0f,
      0);
    igTableHeadersRow();

    // Once the ring is full its oldest event is the next to be overwritten
    const uint32_t first_index =
      (events->count == DIAGNOSTICS_INTERNAL_EVENT_LOG_LENGTH) ?
        events->next_index :
        0;

    for (uint32_t i = 0; i < events->count; i++)
    {
      const struct diagnostics_internal_event_entry_t* entry =
        &(events->entries[(first_index + i) % DIAGNOSTICS_INTERNAL_EVENT_LOG_LENGTH]);

      igTableNextRow(
        0,
        0.0f);
      igTableNextColumn();
      igText(
        "%llu",
        (unsigned long long)entry->frame);
      igTableNextColumn();
      igText(
        "%s",
        diagnostics_internal_event_get_type_name(entry->type));
      igTableNextColumn();
      igText(
        "%s",
        entry->window_tag);
      igTableNextColumn();
      igText(
        "%s",
        entry->is_handled ? "yes" : "");
      igTableNextColumn();
      igText(
        "%s",
        entry->details);
    }

    // Kept at the bottom while it's there already, as in the Log tab
    if (!events->is_paused &&
        (igGetScrollY() >= igGetScrollMaxY()))
    {
      igSetScrollHereY(1.0f);
    }

    igEndTable();
  }

  if (_diagnostics_panel_begin_history_plot(
        "##events_per_frame",
        "events",
        _diagnostics_panel_get_history_maximum(&(events->per_frame))))
  {
    _diagnostics_panel_plot_history(
      "events per frame, besides ticks",
      &(events->per_frame));

    ImPlot_EndPlot();
  }
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_draw_log()
 *
 ******************************************************************************/
static void _diagnostics_panel_draw_log(
  const struct diagnostics_internal_state_t* in_state)
{
  static const ImVec4_c level_colors[LOG_LEVEL_COUNT] =
  {
    [LOG_LEVEL_DEBUG]   = { 0.60f, 0.60f, 0.60f, 1.0f },
    [LOG_LEVEL_INFO]    = { 0.90f, 0.90f, 0.90f, 1.0f },
    [LOG_LEVEL_WARNING] = { 1.00f, 0.80f, 0.30f, 1.0f },
    [LOG_LEVEL_ERROR]   = { 1.00f, 0.40f, 0.40f, 1.0f },
  };

  const struct diagnostics_internal_log_t* log = &(in_state->log);

  for (int level = 0; level < LOG_LEVEL_COUNT; level++)
  {
    char label[_DIAGNOSTICS_PANEL_BYTES_TEXT_LENGTH];
    snprintf(
      label,
      sizeof(label),
      "%s (%llu)",
      log_get_level_name((enum log_level_t)level),
      (unsigned long long)log->level_counts[level]);

    igCheckbox(
      label,
      &(is_log_level_shown[level]));
    igSameLine(
      0.0f,
      -1.0f);
  }

  if (igSmallButton("Clear"))
  {
    diagnostics_internal_log_clear();
  }

  igSetNextItemWidth(240.0f);
  igInputText(
    "Filter",
    log_filter_text,
    sizeof(log_filter_text),
    0,
    NULL,
    NULL);
  igSameLine(
    0.0f,
    -1.0f);
  igCheckbox(
    "Follow new messages",
    &(is_log_auto_scrolled));

  // Ended whether or not the child is visible
  if (igBeginChild_Str(
        "log_messages",
        (ImVec2_c){ 0.0f, 0.0f },
        ImGuiChildFlags_Borders,
        0))
  {
    // Once the ring is full its oldest message is the next to be overwritten
    const uint32_t first_index =
      (log->count == DIAGNOSTICS_INTERNAL_LOG_LENGTH) ?
        log->next_index :
        0;

    for (uint32_t i = 0; i < log->count; i++)
    {
      const struct diagnostics_internal_log_entry_t* entry =
        &(log->entries[(first_index + i) % DIAGNOSTICS_INTERNAL_LOG_LENGTH]);

      const bool is_level_valid =
        ((unsigned int)entry->level < (unsigned int)LOG_LEVEL_COUNT);

      if (!is_level_valid ||
          !is_log_level_shown[entry->level] ||
          ((log_filter_text[0] != '\0') &&
           (strstr(entry->text, log_filter_text) == NULL)))
      {
        continue;
      }

      igPushStyleColor_Vec4(
        ImGuiCol_Text,
        level_colors[entry->level]);
      igTextWrapped(
        "%6llu  %-7s  %s",
        (unsigned long long)entry->frame,
        log_get_level_name(entry->level),
        entry->text);
      igPopStyleColor(1);
    }

    // Kept at the bottom while it's there already, so scrolling up to read
    //  isn't undone by the next message
    if (is_log_auto_scrolled &&
        (igGetScrollY() >= igGetScrollMaxY()))
    {
      igSetScrollHereY(1.0f);
    }
  }

  igEndChild();
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_begin_summary_table()
 *
 ******************************************************************************/
static bool _diagnostics_panel_begin_summary_table(
  const char* in_id,
  const char* in_first_heading)
{
  if (!igBeginTable(
        in_id,
        5,
        (ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV),
        (ImVec2_c){ 0.0f, 0.0f },
        0.0f))
  {
    return false;
  }

  igTableSetupColumn(
    in_first_heading,
    ImGuiTableColumnFlags_WidthStretch,
    0.0f,
    0);
  igTableSetupColumn(
    "Latest",
    ImGuiTableColumnFlags_WidthFixed,
    _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
    0);
  igTableSetupColumn(
    "Average",
    ImGuiTableColumnFlags_WidthFixed,
    _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
    0);
  igTableSetupColumn(
    "99th %",
    ImGuiTableColumnFlags_WidthFixed,
    _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
    0);
  igTableSetupColumn(
    "Max",
    ImGuiTableColumnFlags_WidthFixed,
    _DIAGNOSTICS_PANEL_NUMBER_COLUMN_WIDTH_PIXELS,
    0);
  igTableHeadersRow();

  return true;
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_draw_summary_row()
 *
 ******************************************************************************/
static void _diagnostics_panel_draw_summary_row(
  const char* in_label,
  const struct diagnostics_internal_history_t* in_history,
  const int in_decimal_places)
{
  struct diagnostics_internal_summary_t summary;
  diagnostics_internal_history_summarize(
    in_history,
    &(summary));

  igTableNextRow(
    0,
    0.0f);
  igTableNextColumn();
  igText(
    "%s",
    in_label);
  igTableNextColumn();
  igText(
    "%.*f",
    in_decimal_places,
    (double)summary.latest);
  igTableNextColumn();
  igText(
    "%.*f",
    in_decimal_places,
    (double)summary.average);
  igTableNextColumn();
  igText(
    "%.*f",
    in_decimal_places,
    (double)summary.percentile_99);
  igTableNextColumn();
  igText(
    "%.*f",
    in_decimal_places,
    (double)summary.maximum);
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_begin_history_plot()
 *
 ******************************************************************************/
static bool _diagnostics_panel_begin_history_plot(
  const char* in_id,
  const char* in_y_label,
  const float in_y_maximum)
{
  if (!ImPlot_BeginPlot(
        in_id,
        (ImVec2_c){ -1.0f, _DIAGNOSTICS_PANEL_PLOT_HEIGHT_PIXELS },
        (ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect)))
  {
    return false;
  }

  ImPlot_SetupAxes(
    "frames ago",
    in_y_label,
    ImPlotAxisFlags_AutoFit,
    0);

  const double y_limit =
    (in_y_maximum > 0.0f) ?
      ((double)in_y_maximum * 1.25) :
      1.0;

  ImPlot_SetupAxisLimits(
    ImAxis_Y1,
    0.0,
    y_limit,
    ImPlotCond_Always);
  ImPlot_SetupLegend(
    ImPlotLocation_NorthWest,
    0);

  return true;
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_get_history_maximum()
 *
 ******************************************************************************/
static float _diagnostics_panel_get_history_maximum(
  const struct diagnostics_internal_history_t* in_history)
{
  float maximum = 0.0f;

  for (uint32_t i = 0; i < in_history->count; i++)
  {
    if (in_history->values[i] > maximum)
    {
      maximum = in_history->values[i];
    }
  }

  return maximum;
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_plot_history()
 *
 ******************************************************************************/
static void _diagnostics_panel_plot_history(
  const char* in_label,
  const struct diagnostics_internal_history_t* in_history)
{
  if (in_history->count == 0)
  {
    return;
  }

  // Once the ring is full its oldest value is the next to be overwritten, so
  //  the plot starts there
  ImPlotSpec_c spec = _diagnostics_panel_get_default_spec();
  spec.Offset =
    (in_history->count == DIAGNOSTICS_INTERNAL_HISTORY_LENGTH) ?
      (int)in_history->next_index :
      0;

  // The newest value sits at zero frames ago
  ImPlot_PlotLine_FloatPtrInt(
    in_label,
    in_history->values,
    (int)in_history->count,
    1.0,
    -(double)(in_history->count - 1),
    spec);
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_get_default_spec()
 *
 ******************************************************************************/
static ImPlotSpec_c _diagnostics_panel_get_default_spec(void)
{
  static ImPlotSpec_c default_spec;
  static bool is_default_spec_read = false;

  // ImPlot's defaults are set by its C++ constructor, so one is constructed
  //  once to copy them from
  if (!is_default_spec_read)
  {
    ImPlotSpec_c* constructed_spec = ImPlotSpec_ImPlotSpec();
    default_spec = *constructed_spec;
    ImPlotSpec_destroy(constructed_spec);

    is_default_spec_read = true;
  }

  return default_spec;
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_draw_preview_tooltip()
 *
 ******************************************************************************/
static void _diagnostics_panel_draw_preview_tooltip(
  const struct texture_2d_t* in_texture,
  const struct texture_2d_t* in_target_texture)
{
  if (!igBeginItemTooltip())
  {
    return;
  }

  if (in_texture == in_target_texture)
  {
    igTextDisabled("The diagnostics panel is drawn into this");
  }
  else if (!in_texture->is_loaded ||
           (in_texture->width == 0) ||
           (in_texture->height == 0))
  {
    igTextDisabled("Nothing to preview");
  }
  else
  {
    // Fitted within the preview size, keeping the texture's proportions
    const float scale =
      (in_texture->width >= in_texture->height) ?
        (_DIAGNOSTICS_PANEL_PREVIEW_SIZE_PIXELS / (float)in_texture->width) :
        (_DIAGNOSTICS_PANEL_PREVIEW_SIZE_PIXELS / (float)in_texture->height);

    const ImVec2_c preview_size =
      {
        ((float)in_texture->width * scale),
        ((float)in_texture->height * scale)
      };

    // Every ImTextureID in Jotunn is a texture_2d_t*
    const ImTextureRef_c texture_reference =
      {
        ._TexData = NULL,
        ._TexID = (ImTextureID)(uintptr_t)in_texture
      };

    igImage(
      texture_reference,
      preview_size,
      (ImVec2_c){ 0.0f, is_preview_flipped ? 1.0f : 0.0f },
      (ImVec2_c){ 1.0f, is_preview_flipped ? 0.0f : 1.0f });

    igText(
      "%u x %u",
      in_texture->width,
      in_texture->height);
  }

  igEndTooltip();
}

/*******************************************************************************
 *
 *   Function: _diagnostics_panel_format_bytes()
 *
 ******************************************************************************/
static void _diagnostics_panel_format_bytes(
  const size_t in_size_bytes,
  char* out_text,
  const size_t in_text_length)
{
  const double size_bytes = (double)in_size_bytes;

  if (size_bytes >= (1024.0 * 1024.0))
  {
    snprintf(
      out_text,
      in_text_length,
      "%.2f MB",
      (size_bytes / (1024.0 * 1024.0)));
  }
  else if (size_bytes >= 1024.0)
  {
    snprintf(
      out_text,
      in_text_length,
      "%.1f KB",
      (size_bytes / 1024.0));
  }
  else
  {
    snprintf(
      out_text,
      in_text_length,
      "%zu B",
      in_size_bytes);
  }
}
