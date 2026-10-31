#include "allocation_storage_app_ui.h"
#include "allocation_storage_app.h"

#include "allocator.h"
#include "ui_imgui.h"
#include "util.h"

#include <stdbool.h>

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Says whether Jotunn was built with the heap, which the application doesn't
//  use either way
static void allocation_storage_app_ui_draw_build_kind(void);

// Shows how much of the pool is in use, and the most that's been in use at
//  once
//
//  in:     in_pool
static void allocation_storage_app_ui_draw_pool_summary(
  const struct allocator_pool_t* in_pool);

// Shows a table with a row for each allocator tag
static void allocation_storage_app_ui_draw_tag_table(void);

// Fills a row of the tag table: the tag's name, its policy, and the memory
//  counted against it now and at most. Counts are kept only in DEBUG builds
//
//  in:     in_tag
static void allocation_storage_app_ui_draw_tag_row(
  enum allocator_tag_t in_tag);

// Makes the buttons allocating and freeing blocks, and trying a forbidden
//  tag, with what the last of them did
//
//  in/out: inout_blocks
static void allocation_storage_app_ui_draw_block_buttons(
  struct allocation_storage_app_blocks_t* inout_blocks);

/*******************************************************************************
 *
 *   Function: allocation_storage_app_ui_render()
 *
 ******************************************************************************/
void allocation_storage_app_ui_render(
  struct ui_layer_t* inout_ui_layer)
{
  struct allocation_storage_app_t* app =
    container_of(
      inout_ui_layer,
      struct allocation_storage_app_t,
      ui_layer);

  // Placed the first time it's shown, and as tall as its contents
  if (ui_imgui_begin_panel(
        "Allocation storage",
        (ImVec2_c){20.0f, 20.0f},
        560.0f))
  {
    allocation_storage_app_ui_draw_build_kind();

    igSeparatorText("Pool");
    allocation_storage_app_ui_draw_pool_summary(&(app->memory.pool));

    igSeparatorText("Tags");
    allocation_storage_app_ui_draw_tag_table();

    igSeparatorText("Try it");
    allocation_storage_app_ui_draw_block_buttons(&(app->blocks));

    igSeparator();
    igTextWrapped("Press F3 for Jotunn's diagnostics overlay, which allocates from the pool too");
  }
  igEnd();
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_ui_draw_build_kind()
 *
 ******************************************************************************/
static void allocation_storage_app_ui_draw_build_kind(void)
{
  // Defined for the application too, through JotunnC's public definitions
#ifdef JOTUNN_NO_HEAP
  igText("Jotunn is built without the heap (JOTUNN_ENABLE_HEAP off)");
#else
  igText("Jotunn is built with the heap, but this application doesn't use it");
#endif
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_ui_draw_pool_summary()
 *
 ******************************************************************************/
static void allocation_storage_app_ui_draw_pool_summary(
  const struct allocator_pool_t* in_pool)
{
  char used_text[UTIL_BYTES_TEXT_LENGTH];
  char size_text[UTIL_BYTES_TEXT_LENGTH];
  char peak_text[UTIL_BYTES_TEXT_LENGTH];

  util_format_bytes(
    in_pool->used_bytes,
    used_text,
    sizeof(used_text));
  util_format_bytes(
    in_pool->size_bytes,
    size_text,
    sizeof(size_text));
  util_format_bytes(
    in_pool->peak_used_bytes,
    peak_text,
    sizeof(peak_text));

  igText(
    "%s of %s in use, at most %s",
    used_text,
    size_text,
    peak_text);
  igTextDisabled("Also drawn along the bottom of the window, by the 2D layer");
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_ui_draw_tag_table()
 *
 ******************************************************************************/
static void allocation_storage_app_ui_draw_tag_table(void)
{
  const int column_count = 4;
  const ImGuiTableFlags table_flags = (ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);

  if (!igBeginTable(
        "tags",
        column_count,
        table_flags,
        (ImVec2_c){0.0f, 0.0f},
        0.0f))
  {
    return;
  }

  const char* column_names[] = {"Tag", "Policy", "Live", "Peak"};

  for (int i = 0; i < column_count; i++) {
    igTableSetupColumn(
      column_names[i],
      0,
      0.0f,
      0);
  }
  igTableHeadersRow();

  for (unsigned int i = 0; i < ALLOCATOR_TAG_COUNT; i++) {
    allocation_storage_app_ui_draw_tag_row((enum allocator_tag_t)i);
  }

  igEndTable();
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_ui_draw_tag_row()
 *
 ******************************************************************************/
static void allocation_storage_app_ui_draw_tag_row(
  enum allocator_tag_t in_tag)
{
  struct allocator_tag_stats_t stats;
  const bool has_stats =
    allocator_get_tag_stats(
      in_tag,
      &(stats));

  igTableNextRow(
    0,
    0.0f);

  igTableSetColumnIndex(0);
  igText(
    "%s",
    allocator_get_tag_name(in_tag));

  igTableSetColumnIndex(1);
  igText(
    "%s",
    allocator_get_policy_name(allocator_get_tag_policy(in_tag)));

  if (has_stats) {
    char live_text[UTIL_BYTES_TEXT_LENGTH];
    char peak_text[UTIL_BYTES_TEXT_LENGTH];

    util_format_bytes(
      stats.live_bytes,
      live_text,
      sizeof(live_text));
    util_format_bytes(
      stats.peak_live_bytes,
      peak_text,
      sizeof(peak_text));

    igTableSetColumnIndex(2);
    igText(
      "%s",
      live_text);

    igTableSetColumnIndex(3);
    igText(
      "%s",
      peak_text);
  }
  else {
    igTableSetColumnIndex(2);
    igTextDisabled("-");

    igTableSetColumnIndex(3);
    igTextDisabled("-");
  }
}

/*******************************************************************************
 *
 *   Function: allocation_storage_app_ui_draw_block_buttons()
 *
 ******************************************************************************/
static void allocation_storage_app_ui_draw_block_buttons(
  struct allocation_storage_app_blocks_t* inout_blocks)
{
  const ImVec2_c fit_to_label = {0.0f, 0.0f};

  if (igButton(
        "Allocate 1 MB (GENERAL)",
        fit_to_label))
  {
    allocation_storage_app_blocks_allocate(inout_blocks);
  }
  igSameLine(
    0.0f,
    -1.0f);

  if (igButton(
        "Free a block",
        fit_to_label))
  {
    allocation_storage_app_blocks_free_last(inout_blocks);
  }
  igSameLine(
    0.0f,
    -1.0f);

  if (igButton(
        "Allocate under RENDERER",
        fit_to_label))
  {
    allocation_storage_app_blocks_try_forbidden(inout_blocks);
  }

  igText(
    "%u of %u blocks allocated",
    inout_blocks->count,
    (unsigned int)ALLOCATION_STORAGE_APP_BLOCKS_MAX_COUNT);
  igTextWrapped(
    "%s",
    inout_blocks->status_message);
}
