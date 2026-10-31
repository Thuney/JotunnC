#include "ui_layer_imgui.h"

#include "allocator.h"
#include "event.h"
#include "renderer_2d.h"
#include "texture.h"
#include "timestep.h"
#include "ui_layer.h"
#include "util.h"
#include "window.h"

#include "cimgui.h"
#include "cimnodes.h"
#include "cimplot.h"

#include <float.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/*******************************************************************************
 *
 *   LAYOUT CHECKS
 *
 ******************************************************************************/

// ImGui's vertices are handed to renderer_2d as they are, so the two layouts
//  must agree exactly
static_assert(
  sizeof(ImDrawVert) == sizeof(struct renderable_2d_mesh_vertex_t),
  "ImDrawVert and renderable_2d_mesh_vertex_t differ in size");
static_assert(
  offsetof(ImDrawVert, pos) == offsetof(struct renderable_2d_mesh_vertex_t, position),
  "ImDrawVert::pos and renderable_2d_mesh_vertex_t::position are misaligned");
static_assert(
  offsetof(ImDrawVert, uv) ==
    offsetof(struct renderable_2d_mesh_vertex_t, texture_coordinate),
  "ImDrawVert::uv and renderable_2d_mesh_vertex_t::texture_coordinate are misaligned");
static_assert(
  offsetof(ImDrawVert, col) == offsetof(struct renderable_2d_mesh_vertex_t, color),
  "ImDrawVert::col and renderable_2d_mesh_vertex_t::color are misaligned");
// ImU32 colors are read back as bytes in red, green, blue, alpha order
static_assert(
  __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__,
  "ImGui's packed colors only match u8vector4_t on little-endian targets");
static_assert(
  sizeof(ImDrawIdx) == sizeof(uint16_t),
  "ImDrawIdx is expected to be 16 bits wide");

// Jotunn's key codes follow ImGui's order from Tab through KeypadEqual, so a
//  key translates by offset; check each contiguous group's boundaries
#define _UI_LAYER_IMGUI_KEY_OFFSET_MATCHES(jotunn_key, imgui_key) \
  (((jotunn_key) - EVENT_KEY_CODE_TAB) == ((imgui_key) - ImGuiKey_Tab))

static_assert(
  _UI_LAYER_IMGUI_KEY_OFFSET_MATCHES(EVENT_KEY_CODE_MENU, ImGuiKey_Menu) &&
    _UI_LAYER_IMGUI_KEY_OFFSET_MATCHES(EVENT_KEY_CODE_9, ImGuiKey_9) &&
    _UI_LAYER_IMGUI_KEY_OFFSET_MATCHES(EVENT_KEY_CODE_Z, ImGuiKey_Z) &&
    _UI_LAYER_IMGUI_KEY_OFFSET_MATCHES(EVENT_KEY_CODE_F24, ImGuiKey_F24) &&
    _UI_LAYER_IMGUI_KEY_OFFSET_MATCHES(EVENT_KEY_CODE_PAUSE, ImGuiKey_Pause) &&
    _UI_LAYER_IMGUI_KEY_OFFSET_MATCHES(EVENT_KEY_CODE_KEYPAD_9, ImGuiKey_Keypad9) &&
    _UI_LAYER_IMGUI_KEY_OFFSET_MATCHES(EVENT_KEY_CODE_KEYPAD_EQUAL, ImGuiKey_KeypadEqual),
  "Jotunn's key codes no longer follow ImGui's key order");

// Likewise for cursor shapes, from the arrow through not-allowed
static_assert(
  ((WINDOW_CURSOR_SHAPE_NOT_ALLOWED - WINDOW_CURSOR_SHAPE_ARROW) ==
   (ImGuiMouseCursor_NotAllowed - ImGuiMouseCursor_Arrow)) &&
    ((WINDOW_CURSOR_SHAPE_HAND - WINDOW_CURSOR_SHAPE_ARROW) ==
     (ImGuiMouseCursor_Hand - ImGuiMouseCursor_Arrow)),
  "Jotunn's cursor shapes no longer follow ImGui's cursor order");

/*******************************************************************************
 *
 *   TYPES
 *
 ******************************************************************************/

// A layer's ImGui state, kept in its ui_layer_data
struct ui_layer_imgui_data_t
{
  ImGuiContext* context;
  ImPlotContext* plot_context;
  ImNodesContext* nodes_context;

  // Measures the time between frames for ImGui's animations and repeat rates
  struct timestep_t frame_timer;

  // Scratch space for one draw list's submeshes, grown as needed
  struct renderer_2d_submesh_t* submeshes;
  uint32_t submesh_capacity;

  // The cursor last shown, so the window is only told when it changes
  enum window_cursor_shape_t cursor_shape;
};

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Makes the data's ImGui, ImPlot and imnodes contexts current
//
//  in:     in_data
static void _ui_layer_imgui_make_current(
  const struct ui_layer_imgui_data_t* in_data);

// Shows the cursor shape ImGui asks for over the window
//
//  in:     in_window
//  in/out: inout_data
static void _ui_layer_imgui_update_cursor(
  const struct window_t* in_window,
  struct ui_layer_imgui_data_t* inout_data);

// Creates, updates or destroys the GPU textures behind ImGui's textures, as
//  each one's status asks
//
//  in/out: inout_draw_data
static void _ui_layer_imgui_update_textures(
  ImDrawData* inout_draw_data);

// Applies one ImGui texture's pending create, update or destroy
//
//  in/out: inout_texture_data
static void _ui_layer_imgui_update_texture(
  ImTextureData* inout_texture_data);

// Destroys the GPU texture behind an ImGui texture
//
//  in/out: inout_texture_data
static void _ui_layer_imgui_destroy_texture(
  ImTextureData* inout_texture_data);

// Draws every draw list of a frame through the layer's renderer_2d
//
//  in:     in_draw_data
//  in/out: inout_ui_layer, inout_data
static void _ui_layer_imgui_draw(
  struct ui_layer_t* inout_ui_layer,
  struct ui_layer_imgui_data_t* inout_data,
  const ImDrawData* in_draw_data);

// Makes room for at least in_count submeshes in the data's scratch space
//
//  in:     in_count
//  in/out: inout_data
static void _ui_layer_imgui_reserve_submeshes(
  struct ui_layer_imgui_data_t* inout_data,
  uint32_t in_count);

// Tells ImGui which modifier keys are held
//
//  in:     in_modifiers
//  in/out: inout_io
static void _ui_layer_imgui_update_modifiers(
  ImGuiIO* inout_io,
  uint8_t in_modifiers);

// Translates a Jotunn key code to ImGui's
//
//  in:     in_key
//  ret:    The ImGui key; ImGuiKey_None when ImGui has no equivalent
static ImGuiKey _ui_layer_imgui_translate_key(
  enum event_key_code_t in_key);

// Clipboard hook for ImGui, reading the clipboard of the window held in the
//  context's clipboard user data
//
//  in:     in_context
//  ret:    The clipboard's text, or NULL when it holds none
static const char* _ui_layer_imgui_get_clipboard_text(
  ImGuiContext* in_context);

// Clipboard hook for ImGui, setting the clipboard of the window held in the
//  context's clipboard user data
//
//  in:     in_context, in_text
static void _ui_layer_imgui_set_clipboard_text(
  ImGuiContext* in_context,
  const char* in_text);

// Stands in for ImGui's reset-render-state draw callback. Each mesh is drawn
//  with its render state set afresh, so there is nothing to reset
//
//  in:     in_draw_list, in_command
static void _ui_layer_imgui_reset_render_state(
  const ImDrawList* in_draw_list,
  const ImDrawCmd* in_command);

// Allocator hook for ImGui, which ImPlot and imnodes allocate through too.
//  The memory is counted against ALLOCATOR_TAG_IMGUI; in_user_data is unused
//
//  in:     in_size_bytes, in_user_data
//  ret:    The memory; NULL when it can't be allocated
static void* _ui_layer_imgui_allocate(
  size_t in_size_bytes,
  void* in_user_data);

// Allocator hook for ImGui, releasing memory from _ui_layer_imgui_allocate();
//  NULL is ignored and in_user_data is unused
//
//  in:     in_user_data
//  in/out: inout_memory
static void _ui_layer_imgui_free(
  void* inout_memory,
  void* in_user_data);

/*******************************************************************************
 *
 *   Function: ui_layer_internal_imgui_init()
 *
 ******************************************************************************/
void ui_layer_internal_imgui_init(
  struct ui_layer_t* inout_ui_layer)
{
  struct ui_layer_imgui_data_t* data =
    (struct ui_layer_imgui_data_t*)allocator_allocate_zeroed(
      1,
      sizeof(struct ui_layer_imgui_data_t),
      ALLOCATOR_TAG_UI);

  inout_ui_layer->ui_layer_data = data;

  // ImGui's allocator is shared by every context, and each block must be
  //  freed by the allocator that made it, so it's set before ImGui first
  //  allocates; setting it again for a later layer changes nothing
  igSetAllocatorFunctions(
    &(_ui_layer_imgui_allocate),
    &(_ui_layer_imgui_free),
    NULL);

  // Creating a context only makes it current when none is, so each one is
  //  made current explicitly before the next is created from it
  data->context = igCreateContext(NULL);
  igSetCurrentContext(data->context);
  data->plot_context = ImPlot_CreateContext();
  ImPlot_SetCurrentContext(data->plot_context);
  data->nodes_context = imnodes_CreateContext();
  imnodes_SetCurrentContext(data->nodes_context);

  ImGuiIO* io = igGetIO_ContextPtr(data->context);

  io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  io->BackendPlatformName = "jotunn";
  io->BackendRendererName = "jotunn_renderer_2d";
  io->BackendFlags |=
    (ImGuiBackendFlags_HasMouseCursors |
     ImGuiBackendFlags_RendererHasVtxOffset |
     ImGuiBackendFlags_RendererHasTextures);

  ImGuiPlatformIO* platform_io = igGetPlatformIO_ContextPtr(data->context);

  platform_io->Platform_GetClipboardTextFn = &(_ui_layer_imgui_get_clipboard_text);
  platform_io->Platform_SetClipboardTextFn = &(_ui_layer_imgui_set_clipboard_text);
  platform_io->Platform_ClipboardUserData = inout_ui_layer->ui_window_layer.parent_window;
  platform_io->DrawCallback_ResetRenderState = &(_ui_layer_imgui_reset_render_state);

  igStyleColorsDark(NULL);

  timestep_init(&(data->frame_timer));

  // Not a shape ImGui can ask for, so the first frame always sets the cursor
  data->cursor_shape = WINDOW_CURSOR_SHAPE_COUNT;
}

/*******************************************************************************
 *
 *   Function: ui_layer_internal_imgui_cleanup()
 *
 ******************************************************************************/
void ui_layer_internal_imgui_cleanup(
  struct ui_layer_t* inout_ui_layer)
{
  struct ui_layer_imgui_data_t* data = (struct ui_layer_imgui_data_t*)inout_ui_layer->ui_layer_data;

  if (data == NULL) {
    return;
  }

  _ui_layer_imgui_make_current(data);

  // Textures still referenced only by ImGui itself are ours to release
  ImGuiPlatformIO* platform_io = igGetPlatformIO_ContextPtr(data->context);

  for (int i = 0; i < platform_io->Textures.Size; i++) {
    ImTextureData* texture_data = platform_io->Textures.Data[i];

    if (texture_data->RefCount == 1) {
      _ui_layer_imgui_destroy_texture(texture_data);
    }
  }

  ImGuiIO* io = igGetIO_ContextPtr(data->context);

  io->BackendPlatformName = NULL;
  io->BackendRendererName = NULL;
  io->BackendFlags &=
    ~(ImGuiBackendFlags_HasMouseCursors |
      ImGuiBackendFlags_RendererHasVtxOffset |
      ImGuiBackendFlags_RendererHasTextures);

  imnodes_DestroyContext(data->nodes_context);
  ImPlot_DestroyContext(data->plot_context);
  igDestroyContext(data->context);

  allocator_free(data->submeshes);
  allocator_free(data);

  inout_ui_layer->ui_layer_data = NULL;
}

/*******************************************************************************
 *
 *   Function: ui_layer_internal_imgui_frame_begin()
 *
 ******************************************************************************/
void ui_layer_internal_imgui_frame_begin(
  struct ui_layer_t* inout_ui_layer)
{
  struct ui_layer_imgui_data_t* data = (struct ui_layer_imgui_data_t*)inout_ui_layer->ui_layer_data;
  const struct window_t* window = inout_ui_layer->ui_window_layer.parent_window;

  _ui_layer_imgui_make_current(data);

  ImGuiIO* io = igGetIO_ContextPtr(data->context);

  // The layer's framebuffer matches the window, one pixel per unit
  io->DisplaySize =
    (ImVec2_c){
      (float)window->metadata.width,
      (float)window->metadata.height};
  io->DisplayFramebufferScale = (ImVec2_c){1.0f, 1.0f};

  // ImGui requires time to have passed between frames
  timestep_step(&(data->frame_timer));
  const float delta_time_seconds = (float)data->frame_timer.delta_time_seconds;
  io->DeltaTime =
    (delta_time_seconds > 0.0f) ?
      delta_time_seconds :
      (1.0f / 1000000.0f);

  _ui_layer_imgui_update_cursor(
    window,
    data);

  igNewFrame();
}

/*******************************************************************************
 *
 *   Function: ui_layer_internal_imgui_frame_end()
 *
 ******************************************************************************/
void ui_layer_internal_imgui_frame_end(
  struct ui_layer_t* inout_ui_layer)
{
  struct ui_layer_imgui_data_t* data = (struct ui_layer_imgui_data_t*)inout_ui_layer->ui_layer_data;

  _ui_layer_imgui_make_current(data);

  igRender();

  ImDrawData* draw_data = igGetDrawData();

  _ui_layer_imgui_update_textures(draw_data);

  _ui_layer_imgui_draw(
    inout_ui_layer,
    data,
    draw_data);
}

/*******************************************************************************
 *
 *   Function: ui_layer_internal_imgui_event_react()
 *
 ******************************************************************************/
bool ui_layer_internal_imgui_event_react(
  struct ui_layer_t* inout_ui_layer,
  const struct event_base_t* in_event)
{
  struct ui_layer_imgui_data_t* data = (struct ui_layer_imgui_data_t*)inout_ui_layer->ui_layer_data;
  const struct window_t* window = inout_ui_layer->ui_window_layer.parent_window;
  const void* window_handle = window->context_data.window_handle;

  _ui_layer_imgui_make_current(data);

  ImGuiIO* io = igGetIO_ContextPtr(data->context);

  bool is_captured = false;

  // Events reach every window, so input from other windows is ignored
  switch (in_event->event_type)
  {
    case EVENT_TYPE_MOUSE_MOVED:
    {
      const struct event_mouse_moved_t* event =
        container_of(
          in_event,
          struct event_mouse_moved_t,
          base);

      if (event->window_handle == window_handle) {
        // Jotunn's mouse coordinates grow upward from the bottom of the
        //  window; ImGui's grow downward from the top
        ImGuiIO_AddMousePosEvent(
          io,
          event->x,
          ((float)window->metadata.height - event->y));

        is_captured = io->WantCaptureMouse;
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

      if ((event->window_handle == window_handle) &&
          ((int)event->button >= 0) &&
          ((int)event->button < ImGuiMouseButton_COUNT))
      {
        _ui_layer_imgui_update_modifiers(
          io,
          (uint8_t)event->mods);
        ImGuiIO_AddMouseButtonEvent(
          io,
          (int)event->button,
          (event->action == EVENT_MOUSE_ACTION_PRESS));

        is_captured = io->WantCaptureMouse;
      }
    }
    break;

    case EVENT_TYPE_MOUSE_SCROLLED:
    {
      const struct event_mouse_scrolled_t* event =
        container_of(
          in_event,
          struct event_mouse_scrolled_t,
          base);

      if (event->window_handle == window_handle) {
        ImGuiIO_AddMouseWheelEvent(
          io,
          event->x_offset,
          event->y_offset);

        is_captured = io->WantCaptureMouse;
      }
    }
    break;

    case EVENT_TYPE_MOUSE_ENTERED:
    {
      const struct event_mouse_entered_t* event =
        container_of(
          in_event,
          struct event_mouse_entered_t,
          base);

      if ((event->window_handle == window_handle) &&
          !event->is_entered)
      {
        // ImGui's way of saying the mouse is nowhere over the window
        ImGuiIO_AddMousePosEvent(
          io,
          -FLT_MAX,
          -FLT_MAX);
      }
    }
    break;

    case EVENT_TYPE_KEY_PRESSED:
    {
      const struct event_key_pressed_t* event =
        container_of(
          in_event,
          struct event_key_pressed_t,
          base);

      if (event->window_handle == window_handle) {
        _ui_layer_imgui_update_modifiers(
          io,
          event->modifiers);

        const ImGuiKey key = _ui_layer_imgui_translate_key(event->key);
        if (key != ImGuiKey_None) {
          ImGuiIO_AddKeyEvent(
            io,
            key,
            true);
        }

        is_captured = io->WantCaptureKeyboard;
      }
    }
    break;

    case EVENT_TYPE_KEY_RELEASED:
    {
      const struct event_key_released_t* event =
        container_of(
          in_event,
          struct event_key_released_t,
          base);

      if (event->window_handle == window_handle) {
        _ui_layer_imgui_update_modifiers(
          io,
          event->modifiers);

        const ImGuiKey key = _ui_layer_imgui_translate_key(event->key);
        if (key != ImGuiKey_None) {
          ImGuiIO_AddKeyEvent(
            io,
            key,
            false);
        }

        is_captured = io->WantCaptureKeyboard;
      }
    }
    break;

    case EVENT_TYPE_KEY_TYPED:
    {
      const struct event_key_typed_t* event =
        container_of(
          in_event,
          struct event_key_typed_t,
          base);

      if (event->window_handle == window_handle) {
        ImGuiIO_AddInputCharacter(
          io,
          event->codepoint);

        is_captured = io->WantCaptureKeyboard;
      }
    }
    break;

    case EVENT_TYPE_WINDOW_FOCUS:
    {
      const struct event_window_focus_t* event =
        container_of(
          in_event,
          struct event_window_focus_t,
          base);

      if (event->window_handle == window_handle) {
        ImGuiIO_AddFocusEvent(
          io,
          (event->is_focused != 0));
      }
    }
    break;

    default:
    {
    }
    break;
  }

  return is_captured;
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_make_current()
 *
 ******************************************************************************/
static void _ui_layer_imgui_make_current(
  const struct ui_layer_imgui_data_t* in_data)
{
  igSetCurrentContext(in_data->context);
  ImPlot_SetCurrentContext(in_data->plot_context);
  imnodes_SetCurrentContext(in_data->nodes_context);
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_update_cursor()
 *
 ******************************************************************************/
static void _ui_layer_imgui_update_cursor(
  const struct window_t* in_window,
  struct ui_layer_imgui_data_t* inout_data)
{
  const ImGuiIO* io = igGetIO_ContextPtr(inout_data->context);

  if (io->ConfigFlags &
      ImGuiConfigFlags_NoMouseCursorChange)
  {
    return;
  }

  const ImGuiMouseCursor imgui_cursor = igGetMouseCursor();

  // ImGui draws the cursor itself when asked to, so the window hides its own
  const enum window_cursor_shape_t cursor_shape =
    ((imgui_cursor == ImGuiMouseCursor_None) ||
     io->MouseDrawCursor) ?
      WINDOW_CURSOR_SHAPE_HIDDEN :
      (WINDOW_CURSOR_SHAPE_ARROW + (imgui_cursor - ImGuiMouseCursor_Arrow));

  if (cursor_shape != inout_data->cursor_shape) {
    window_set_cursor_shape(
      in_window,
      cursor_shape);

    inout_data->cursor_shape = cursor_shape;
  }
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_update_textures()
 *
 ******************************************************************************/
static void _ui_layer_imgui_update_textures(
  ImDrawData* inout_draw_data)
{
  if (inout_draw_data->Textures == NULL) {
    return;
  }

  for (int i = 0; i < inout_draw_data->Textures->Size; i++) {
    ImTextureData* texture_data = inout_draw_data->Textures->Data[i];

    if (texture_data->Status != ImTextureStatus_OK) {
      _ui_layer_imgui_update_texture(texture_data);
    }
  }
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_update_texture()
 *
 ******************************************************************************/
static void _ui_layer_imgui_update_texture(
  ImTextureData* inout_texture_data)
{
  switch (inout_texture_data->Status)
  {
    case ImTextureStatus_WantCreate:
    {
      // ImGui's atlas defaults to RGBA32, the only format handled here
      struct texture_2d_t* texture =
        (struct texture_2d_t*)allocator_allocate_zeroed(
          1,
          sizeof(struct texture_2d_t),
          ALLOCATOR_TAG_UI);

      texture_2d_init(
        texture,
        (unsigned int)inout_texture_data->Width,
        (unsigned int)inout_texture_data->Height,
        TEXTURE_2D_INTERNAL_FORMAT_RGBA,
        TEXTURE_2D_DATA_FORMAT_RGBA,
        false);
      texture_2d_set_data(
        texture,
        (unsigned char*)ImTextureData_GetPixels(inout_texture_data),
        (unsigned int)ImTextureData_GetSizeInBytes(inout_texture_data));
      resource_set_name(
        &(texture->resource),
        "imgui");

      // Every ImTextureID in Jotunn is a texture_2d_t*
      ImTextureData_SetTexID(
        inout_texture_data,
        (ImTextureID)(uintptr_t)texture);
      ImTextureData_SetStatus(
        inout_texture_data,
        ImTextureStatus_OK);
    }
    break;

    case ImTextureStatus_WantUpdates:
    {
      const struct texture_2d_t* texture =
        (const struct texture_2d_t*)(uintptr_t)inout_texture_data->TexID;

      for (int i = 0; i < inout_texture_data->Updates.Size; i++) {
        const ImTextureRect* update_rect = &(inout_texture_data->Updates.Data[i]);

        texture_2d_set_sub_data(
          texture,
          update_rect->x,
          update_rect->y,
          update_rect->w,
          update_rect->h,
          (uint32_t)inout_texture_data->Width,
          ImTextureData_GetPixelsAt(
            inout_texture_data,
            update_rect->x,
            update_rect->y));
      }

      ImTextureData_SetStatus(
        inout_texture_data,
        ImTextureStatus_OK);
    }
    break;

    case ImTextureStatus_WantDestroy:
    {
      // A texture is only let go once a whole frame has passed without it
      if (inout_texture_data->UnusedFrames > 0) {
        _ui_layer_imgui_destroy_texture(inout_texture_data);
      }
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
 *   Function: _ui_layer_imgui_destroy_texture()
 *
 ******************************************************************************/
static void _ui_layer_imgui_destroy_texture(
  ImTextureData* inout_texture_data)
{
  struct texture_2d_t* texture = (struct texture_2d_t*)(uintptr_t)inout_texture_data->TexID;

  if (texture != NULL) {
    texture_2d_cleanup(texture);
    allocator_free(texture);
  }

  ImTextureData_SetTexID(
    inout_texture_data,
    ImTextureID_Invalid);
  ImTextureData_SetStatus(
    inout_texture_data,
    ImTextureStatus_Destroyed);
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_draw()
 *
 ******************************************************************************/
static void _ui_layer_imgui_draw(
  struct ui_layer_t* inout_ui_layer,
  struct ui_layer_imgui_data_t* inout_data,
  const ImDrawData* in_draw_data)
{
  const ImVec2_c display_position = in_draw_data->DisplayPos;
  const ImVec2_c display_size = in_draw_data->DisplaySize;
  const ImVec2_c framebuffer_scale = in_draw_data->FramebufferScale;

  const float framebuffer_width_pixels = (display_size.x * framebuffer_scale.x);
  const float framebuffer_height_pixels = (display_size.y * framebuffer_scale.y);

  if ((framebuffer_width_pixels <= 0.0f) ||
      (framebuffer_height_pixels <= 0.0f))
  {
    return;
  }

  // ImGui's coordinates start at the top-left of the display and grow
  //  downward, so the camera is flipped to match for the duration of the frame
  struct camera_orthographic_t* camera = &(inout_ui_layer->ui_camera_ortho);

  camera_orthographic_set_projection(
    camera,
    display_position.x,
    (display_position.x + display_size.x),
    display_position.y,
    (display_position.y + display_size.y),
    camera->base.near_plane,
    camera->base.far_plane);

  const ImGuiPlatformIO* platform_io = igGetPlatformIO_ContextPtr(inout_data->context);

  for (int list_index = 0; list_index < in_draw_data->CmdLists.Size; list_index++) {
    const ImDrawList* draw_list = in_draw_data->CmdLists.Data[list_index];

    _ui_layer_imgui_reserve_submeshes(
      inout_data,
      (uint32_t)draw_list->CmdBuffer.Size);

    struct renderer_2d_mesh_t mesh =
      (struct renderer_2d_mesh_t){
        .vertices = (const struct renderable_2d_mesh_vertex_t*)draw_list->VtxBuffer.Data,
        .vertex_count = (uint32_t)draw_list->VtxBuffer.Size,
        .index_type = RENDER_API_ELEMENT_DATA_TYPE_UNSIGNED_SHORT,
        .indices = draw_list->IdxBuffer.Data,
        .index_count = (uint32_t)draw_list->IdxBuffer.Size,
        .submeshes = inout_data->submeshes,
        .submesh_count = 0};

    for (int command_index = 0; command_index < draw_list->CmdBuffer.Size; command_index++) {
      ImDrawCmd* command = &(draw_list->CmdBuffer.Data[command_index]);

      if (command->UserCallback != NULL) {
        // Everything before the callback is drawn first, so that whatever
        //  the callback draws lands in order
        renderer_2d_draw_mesh(
          &(inout_ui_layer->ui_renderer_2d),
          &(mesh));
        mesh.submesh_count = 0;

        if (command->UserCallback != platform_io->DrawCallback_ResetRenderState) {
          command->UserCallback(
            draw_list,
            command);
        }

        continue;
      }

      // Bring the clip rectangle into framebuffer pixels, within the
      //  framebuffer
      float clip_min_x = ((command->ClipRect.x - display_position.x) * framebuffer_scale.x);
      float clip_min_y = ((command->ClipRect.y - display_position.y) * framebuffer_scale.y);
      float clip_max_x = ((command->ClipRect.z - display_position.x) * framebuffer_scale.x);
      float clip_max_y = ((command->ClipRect.w - display_position.y) * framebuffer_scale.y);

      clip_min_x = (clip_min_x < 0.0f) ? 0.0f : clip_min_x;
      clip_min_y = (clip_min_y < 0.0f) ? 0.0f : clip_min_y;
      clip_max_x =
        (clip_max_x > framebuffer_width_pixels) ?
          framebuffer_width_pixels :
          clip_max_x;
      clip_max_y =
        (clip_max_y > framebuffer_height_pixels) ?
          framebuffer_height_pixels :
          clip_max_y;

      if ((clip_max_x <= clip_min_x) ||
          (clip_max_y <= clip_min_y))
      {
        continue;
      }

      // The scissor's origin is the bottom-left of the framebuffer
      inout_data->submeshes[mesh.submesh_count] =
        (struct renderer_2d_submesh_t){
          .texture = (const struct texture_2d_t*)(uintptr_t)ImDrawCmd_GetTexID(command),
          .is_clipped = true,
          .clip_rect =
            (struct renderer_2d_clip_rect_t){
              .x_pixels = (int32_t)clip_min_x,
              .y_pixels = (int32_t)(framebuffer_height_pixels - clip_max_y),
              .width_pixels = (int32_t)(clip_max_x - clip_min_x),
              .height_pixels = (int32_t)(clip_max_y - clip_min_y)},
          .index_offset = command->IdxOffset,
          .index_count = command->ElemCount,
          .vertex_offset = command->VtxOffset};
      mesh.submesh_count++;
    }

    renderer_2d_draw_mesh(
      &(inout_ui_layer->ui_renderer_2d),
      &(mesh));
  }
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_reserve_submeshes()
 *
 ******************************************************************************/
static void _ui_layer_imgui_reserve_submeshes(
  struct ui_layer_imgui_data_t* inout_data,
  uint32_t in_count)
{
  if (in_count <= inout_data->submesh_capacity) {
    return;
  }

  // Grow geometrically so a busy frame doesn't reallocate per draw list
  uint32_t capacity = (inout_data->submesh_capacity > 0) ? inout_data->submesh_capacity : 64;
  while (capacity < in_count) {
    capacity *= 2;
  }

  inout_data->submeshes =
    (struct renderer_2d_submesh_t*)allocator_reallocate(
      inout_data->submeshes,
      (capacity * sizeof(struct renderer_2d_submesh_t)),
      ALLOCATOR_TAG_UI);
  inout_data->submesh_capacity = capacity;
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_update_modifiers()
 *
 ******************************************************************************/
static void _ui_layer_imgui_update_modifiers(
  ImGuiIO* inout_io,
  uint8_t in_modifiers)
{
  ImGuiIO_AddKeyEvent(
    inout_io,
    ImGuiMod_Ctrl,
    ((in_modifiers & EVENT_KEY_MODIFIER_CONTROL) != 0));
  ImGuiIO_AddKeyEvent(
    inout_io,
    ImGuiMod_Shift,
    ((in_modifiers & EVENT_KEY_MODIFIER_SHIFT) != 0));
  ImGuiIO_AddKeyEvent(
    inout_io,
    ImGuiMod_Alt,
    ((in_modifiers & EVENT_KEY_MODIFIER_ALT) != 0));
  ImGuiIO_AddKeyEvent(
    inout_io,
    ImGuiMod_Super,
    ((in_modifiers & EVENT_KEY_MODIFIER_SUPER) != 0));
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_translate_key()
 *
 ******************************************************************************/
static ImGuiKey _ui_layer_imgui_translate_key(
  enum event_key_code_t in_key)
{
  if ((in_key >= EVENT_KEY_CODE_TAB) &&
      (in_key <= EVENT_KEY_CODE_KEYPAD_EQUAL))
  {
    return (ImGuiKey)(ImGuiKey_Tab + (in_key - EVENT_KEY_CODE_TAB));
  }

  if (in_key == EVENT_KEY_CODE_OEM_102) {
    return ImGuiKey_Oem102;
  }

  return ImGuiKey_None;
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_get_clipboard_text()
 *
 ******************************************************************************/
static const char* _ui_layer_imgui_get_clipboard_text(
  ImGuiContext* in_context)
{
  const ImGuiPlatformIO* platform_io = igGetPlatformIO_ContextPtr(in_context);

  return window_get_clipboard_text(
    (const struct window_t*)platform_io->Platform_ClipboardUserData);
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_set_clipboard_text()
 *
 ******************************************************************************/
static void _ui_layer_imgui_set_clipboard_text(
  ImGuiContext* in_context,
  const char* in_text)
{
  const ImGuiPlatformIO* platform_io = igGetPlatformIO_ContextPtr(in_context);

  window_set_clipboard_text(
    (const struct window_t*)platform_io->Platform_ClipboardUserData,
    in_text);
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_reset_render_state()
 *
 ******************************************************************************/
static void _ui_layer_imgui_reset_render_state(
  const ImDrawList* in_draw_list,
  const ImDrawCmd* in_command)
{
  (void)in_draw_list;
  (void)in_command;
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_allocate()
 *
 ******************************************************************************/
static void* _ui_layer_imgui_allocate(
  size_t in_size_bytes,
  void* in_user_data)
{
  (void)in_user_data;

  return allocator_allocate(
    in_size_bytes,
    ALLOCATOR_TAG_IMGUI);
}

/*******************************************************************************
 *
 *   Function: _ui_layer_imgui_free()
 *
 ******************************************************************************/
static void _ui_layer_imgui_free(
  void* inout_memory,
  void* in_user_data)
{
  (void)in_user_data;

  allocator_free(inout_memory);
}
