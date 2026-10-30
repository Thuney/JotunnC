#pragma once

// Private to Jotunn: the Dear ImGui implementation behind ui_layer_t. It takes
//  its input from Jotunn events and draws through the layer's renderer_2d,
//  so it carries no dependency on any windowing or graphics backend

#include <stdbool.h>

struct event_base_t;
struct ui_layer_t;

/*******************************************************************************
 *
 *   LIFECYCLE
 *
 ******************************************************************************/

// Creates the layer's ImGui, ImPlot and imnodes contexts, storing them in the
//  layer's ui_layer_data
//
//  in/out: inout_ui_layer
void ui_layer_internal_imgui_init(
  struct ui_layer_t* inout_ui_layer);

// Releases the layer's ImGui textures and destroys its contexts. The parent
//  window's graphics context must be current
//
//  in/out: inout_ui_layer
void ui_layer_internal_imgui_cleanup(
  struct ui_layer_t* inout_ui_layer);

/*******************************************************************************
 *
 *   FRAMES
 *
 ******************************************************************************/

// Starts an ImGui frame; ig*, ImPlot_* and imnodes_* calls may follow until
//  ui_layer_internal_imgui_frame_end()
//
//  in/out: inout_ui_layer
void ui_layer_internal_imgui_frame_begin(
  struct ui_layer_t* inout_ui_layer);

// Ends the ImGui frame and draws it through the layer's renderer_2d, which
//  must be within a scene
//
//  in/out: inout_ui_layer
void ui_layer_internal_imgui_frame_end(
  struct ui_layer_t* inout_ui_layer);

/*******************************************************************************
 *
 *   EVENTS
 *
 ******************************************************************************/

// Passes an event from the layer's parent window to ImGui
//
//  in:     in_event
//  in/out: inout_ui_layer
//  ret:    true when ImGui wants exclusive use of the event's input device,
//            the mouse or the keyboard; otherwise false
bool ui_layer_internal_imgui_event_react(
  struct ui_layer_t* inout_ui_layer,
  const struct event_base_t* in_event);
