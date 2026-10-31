#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "application.h"
#include "log.h"
#include "event.h"
#include "fvector.h"
#include "render_api.h"
#include "renderer_2d.h"
#include "window.h"

#include "../../core/application/window_internal.h"

/*******************************************************************************
 *
 *   STATE
 *
 ******************************************************************************/

static uint8_t is_glfw_initialized = 0;
static uint8_t is_glew_initialized = 0;

// Windows opened and not yet cleaned up; GLFW is terminated once the last one
//  closes
static uint32_t open_window_count = 0;

// Standard cursors are created on first use and shared between windows; GLFW
//  destroys them itself when it terminates
static GLFWcursor* gl_cursors[WINDOW_CURSOR_SHAPE_COUNT];
static bool is_gl_cursor_created[WINDOW_CURSOR_SHAPE_COUNT];

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// GLFW's window size callback: records the window's new size in its
//  metadata, marks it resized and sends a window resize event
//
//  in:     in_window, in_width, in_height
static void window_gl_size_callback(
  GLFWwindow* in_window,
  const int in_width,
  const int in_height);

// GLFW's framebuffer size callback: records the framebuffer's new size as
//  the window's size
//
//  in:     in_window, in_width_pixels, in_height_pixels
static void window_gl_framebuffer_size_callback(
  GLFWwindow* in_window,
  const int in_width_pixels,
  const int in_height_pixels);

// GLFW's window close callback: flags the window to close and sends a window
//  close event
//
//  in:     in_window
static void window_gl_close_callback(
  GLFWwindow* in_window);

// GLFW's key callback: sends a key released event for a release, and a key
//  pressed event, marked as a repeat while the key is held, otherwise
//
//  in:     in_window, in_key, in_scancode, in_action, in_mods
static void window_gl_key_callback(
  GLFWwindow* in_window,
  const int in_key,
  const int in_scancode,
  const int in_action,
  const int in_mods);

// GLFW's character callback: sends a key typed event for the Unicode code
//  point typed
//
//  in:     in_window, in_codepoint
static void window_gl_char_callback(
  GLFWwindow* in_window,
  const unsigned int in_codepoint);

// GLFW's mouse button callback: sends a mouse button event
//
//  in:     in_window, in_button, in_action, in_mods
static void window_gl_mouse_button_callback(
  GLFWwindow* in_window,
  const int in_button,
  const int in_action,
  const int in_mods);

// GLFW's scroll callback: sends a mouse scrolled event
//
//  in:     in_window, in_x_offset, in_y_offset
static void window_gl_scroll_callback(
  GLFWwindow* in_window,
  const double in_x_offset,
  const double in_y_offset);

// GLFW's cursor position callback: sends a mouse moved event, with the
//  position measured up from the bottom of the window rather than down from
//  the top
//
//  in:     in_window, in_x_pos, in_y_pos
static void window_gl_cursor_position_callback(
  GLFWwindow* in_window,
  const double in_x_pos,
  const double in_y_pos);

// GLFW's window focus callback: sends a window focus event, whether the
//  window gained focus or lost it
//
//  in:     in_window, in_is_focused
static void window_gl_focus_callback(
  GLFWwindow* in_window,
  const int in_is_focused);

// GLFW's error callback: logs the error in debug builds
//
//  in:     in_error_code, in_description
static void window_gl_error_callback(
  const int in_error_code,
  const char* in_description);

// GLFW's cursor enter callback: sends a mouse entered event as the cursor
//  enters or leaves the window's content area
//
//  in:     in_window, in_is_entered
static void _window_gl_cursor_enter_callback(
  GLFWwindow* in_window,
  const int in_is_entered);

// Translates a GLFW key code to Jotunn's
//
//  in:     in_glfw_key
//  ret:    The key code; EVENT_KEY_CODE_UNKNOWN for a key Jotunn doesn't have
static enum event_key_code_t _window_gl_translate_key(
  const int in_glfw_key);

// Translates GLFW's modifier key bits to Jotunn's
//
//  in:     in_glfw_modifiers
//  ret:    A bit set of enum event_key_modifier_t
static uint8_t _window_gl_translate_modifiers(
  const int in_glfw_modifiers);

// Finds GLFW's standard cursor for a shape, creating it the first time
//
//  in:     in_cursor_shape
//  ret:    The cursor; NULL for a shape out of range or one the platform
//            lacks, either of which GLFW shows as the arrow
static GLFWcursor* _window_gl_get_cursor(
  const enum window_cursor_shape_t in_cursor_shape);

// Points each of a GLFW window's callbacks at this file's
//
//  in/out: inout_glfw_window_ptr
static void window_gl_set_callbacks(
  GLFWwindow* inout_glfw_window_ptr);

// Initializes GLFW the first time, opens a window sized and titled from the
//  window's metadata, with an OpenGL 3.2 core context made current and the
//  viewport covering it, then initializes GLEW the first time. Any OpenGL
//  errors left over are cleared
//
//  in/out: inout_window
//  ret:    0 on success; 1 when GLFW or GLEW couldn't be initialized
static uint8_t window_gl_init(
  struct window_t* inout_window);

// Makes the window's OpenGL context current on the calling thread
//
//  in:     in_window
static void window_gl_set_context(
  const struct window_t* in_window);

// Leaves the calling thread with no current OpenGL context
//
//  in:     in_window
static void window_gl_release_context(
  const struct window_t* in_window);

// Handles every window's pending events through GLFW's callbacks
//
//  in/out: inout_window
static void window_gl_poll_events(
  struct window_t* inout_window);

// Swaps the window's buffers, presenting the frame drawn
//
//  in:     in_window
static void window_gl_run(
  const struct window_t* in_window);

// Closes the window. Once the last open window is closed, also terminates
//  GLFW, which destroys the standard cursors, so the next window opened
//  initializes GLFW and GLEW again
//
//  in:     in_window
static void window_gl_cleanup(
  const struct window_t* in_window);

/*******************************************************************************
 *
 *   BACKEND HOOKS
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *   Function: window_internal_platform_init()
 *
 ******************************************************************************/
uint8_t window_internal_platform_init(
  struct window_t* inout_window)
{
#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Found OpenGL Backend");
#endif

  return window_gl_init(inout_window);
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_poll_events()
 *
 ******************************************************************************/
void window_internal_platform_poll_events(
  struct window_t* inout_window)
{
  window_gl_poll_events(inout_window);
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_run()
 *
 ******************************************************************************/
void window_internal_platform_run(
  const struct window_t* in_window)
{
  window_gl_run(in_window);
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_cleanup()
 *
 ******************************************************************************/
void window_internal_platform_cleanup(
  const struct window_t* in_window)
{
  window_gl_cleanup(in_window);
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_set_context()
 *
 ******************************************************************************/
void window_internal_platform_set_context(
  const struct window_t* in_window)
{
  window_gl_set_context(in_window);
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_release_context()
 *
 ******************************************************************************/
void window_internal_platform_release_context(
  const struct window_t* in_window)
{
  window_gl_release_context(in_window);
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_set_viewport()
 *
 ******************************************************************************/
void window_internal_platform_set_viewport(
  const uint32_t in_x_pixels,
  const uint32_t in_y_pixels,
  const uint32_t in_width_pixels,
  const uint32_t in_height_pixels)
{
  glViewport(
    in_x_pixels,
    in_y_pixels,
    in_width_pixels,
    in_height_pixels);
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_set_background_color()
 *
 ******************************************************************************/
void window_internal_platform_set_background_color(
  struct window_t* inout_window,
  const union fvector4_t in_color)
{
  render_api_set_clear_color(
    &(inout_window->window_layer_renderer.base.render_api),
    in_color);
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_set_cursor_shape()
 *
 ******************************************************************************/
void window_internal_platform_set_cursor_shape(
  const struct window_t* in_window,
  const enum window_cursor_shape_t in_cursor_shape)
{
  GLFWwindow* gl_window_ptr = (GLFWwindow*)in_window->context_data.window_handle;

  if (in_cursor_shape == WINDOW_CURSOR_SHAPE_HIDDEN) {
    glfwSetInputMode(
      gl_window_ptr,
      GLFW_CURSOR,
      GLFW_CURSOR_HIDDEN);
  }
  else {
    // A NULL cursor falls back to the default arrow
    glfwSetCursor(
      gl_window_ptr,
      _window_gl_get_cursor(in_cursor_shape));
    glfwSetInputMode(
      gl_window_ptr,
      GLFW_CURSOR,
      GLFW_CURSOR_NORMAL);
  }
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_get_clipboard_text()
 *
 ******************************************************************************/
const char* window_internal_platform_get_clipboard_text(
  const struct window_t* in_window)
{
  return glfwGetClipboardString((GLFWwindow*)in_window->context_data.window_handle);
}

/*******************************************************************************
 *
 *   Function: window_internal_platform_set_clipboard_text()
 *
 ******************************************************************************/
void window_internal_platform_set_clipboard_text(
  const struct window_t* in_window,
  const char* in_text)
{
  glfwSetClipboardString(
    (GLFWwindow*)in_window->context_data.window_handle,
    in_text);
}

/*******************************************************************************
 *
 *   GLFW CALLBACKS
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *   Function: window_gl_size_callback()
 *
 ******************************************************************************/
static void window_gl_size_callback(
  GLFWwindow* in_window,
  const int in_width,
  const int in_height)
{
  #ifdef DEBUG
  fprintf(
    stdout,
    "Event - Window Resize - W: %d H: %d\n",
    in_width,
    in_height);
  #endif

  struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(in_window);
  metadata->width = in_width;
  metadata->height = in_height;
  metadata->is_resized = 1;

  struct event_window_resize_t resize_event = (struct event_window_resize_t){
    .base = (struct event_base_t){.event_type = EVENT_TYPE_WINDOW_RESIZE, .is_handled = 0},
    .window_handle = (void*)in_window,
    .width = in_width,
    .height = in_height};

  metadata->function_event_notify(
    metadata->parent_application,
    &(resize_event.base));
}

/*******************************************************************************
 *
 *   Function: window_gl_framebuffer_size_callback()
 *
 ******************************************************************************/
static void window_gl_framebuffer_size_callback(
  GLFWwindow* in_window,
  const int in_width_pixels,
  const int in_height_pixels)
{
  #ifdef DEBUG
  fprintf(
    stdout,
    "Event - Framebuffer Resize - W: %d H: %d\n",
    in_width_pixels,
    in_height_pixels);
  #endif

  struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(in_window);
  metadata->width = in_width_pixels;
  metadata->height = in_height_pixels;

  // Set the GL viewport to the size of the window
  // glViewport(0, 0, metadata->width, metadata->height);
}

/*******************************************************************************
 *
 *   Function: window_gl_close_callback()
 *
 ******************************************************************************/
static void window_gl_close_callback(
  GLFWwindow* in_window)
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Event - Window Close\n");
  // #endif

  struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(in_window);

  metadata->should_close = 1;

  struct event_window_close_t window_close_event =
    (struct event_window_close_t){
      .base = (struct event_base_t){.event_type = EVENT_TYPE_WINDOW_CLOSE, .is_handled = 0},
      .window_handle = (void*)in_window};

  metadata->function_event_notify(
    metadata->parent_application,
    &(window_close_event.base));
}

/*******************************************************************************
 *
 *   Function: window_gl_key_callback()
 *
 ******************************************************************************/
static void window_gl_key_callback(
  GLFWwindow* in_window,
  const int in_key,
  const int in_scancode,
  const int in_action,
  const int in_mods)
{
  // #ifdef DEBUG
  // fprintf(
  //   stdout,
  //   "Event - Key - Key: %c Scancode: %d Action: %d Mods: %d\n",
  //   key,
  //   scancode,
  //   action,
  //   mods);
  // #endif

  struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(in_window);

  const enum event_key_code_t key_code = _window_gl_translate_key(in_key);
  const uint8_t modifiers = _window_gl_translate_modifiers(in_mods);

  if (in_action == GLFW_RELEASE) {
    struct event_key_released_t key_released_event =
      (struct event_key_released_t){
        .base = (struct event_base_t){.event_type = EVENT_TYPE_KEY_RELEASED, .is_handled = 0},
        .window_handle = (void*)in_window,
        .key = key_code,
        .modifiers = modifiers};

    metadata->function_event_notify(
      metadata->parent_application,
      &(key_released_event.base));
  }
  else {
    struct event_key_pressed_t key_pressed_event =
      (struct event_key_pressed_t){
        .base = (struct event_base_t){.event_type = EVENT_TYPE_KEY_PRESSED, .is_handled = 0},
        .window_handle = (void*)in_window,
        .key = key_code,
        .modifiers = modifiers,
        .is_repeat = (in_action == GLFW_REPEAT)};

    metadata->function_event_notify(
      metadata->parent_application,
      &(key_pressed_event.base));
  }
}

/*******************************************************************************
 *
 *   Function: window_gl_char_callback()
 *
 ******************************************************************************/
static void window_gl_char_callback(
  GLFWwindow* in_window,
  const unsigned int in_codepoint)
{
  // #ifdef DEBUG
  // fprintf(stdout, "Event - Char Received - Char: %c\n", codepoint);
  // #endif

  struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(in_window);

  struct event_key_typed_t key_typed_event =
    (struct event_key_typed_t){
      .base = (struct event_base_t){.event_type = EVENT_TYPE_KEY_TYPED, .is_handled = 0},
      .window_handle = (void*)in_window,
      .codepoint = in_codepoint};

  metadata->function_event_notify(
    metadata->parent_application,
    &(key_typed_event.base));
}

/*******************************************************************************
 *
 *   Function: window_gl_mouse_button_callback()
 *
 ******************************************************************************/
static void window_gl_mouse_button_callback(
  GLFWwindow* in_window,
  const int in_button,
  const int in_action,
  const int in_mods)
{
  // #ifdef DEBUG
  // fprintf(
  //   stdout,
  //   "Event - Mouse Button - Button %d Action %d Mods %d\n",
  //   button,
  //   action,
  //   mods);
  // #endif

  struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(in_window);

  struct event_mouse_button_t mouse_button_event =
    (struct event_mouse_button_t){
      .base = (struct event_base_t){.event_type = EVENT_TYPE_MOUSE_BUTTON, .is_handled = 0},
      .window_handle = (void*)in_window,
      .button = in_button,
      .action = in_action,
      .mods = _window_gl_translate_modifiers(in_mods)};

  metadata->function_event_notify(
    metadata->parent_application,
    &(mouse_button_event.base));
}

/*******************************************************************************
 *
 *   Function: window_gl_scroll_callback()
 *
 ******************************************************************************/
static void window_gl_scroll_callback(
  GLFWwindow* in_window,
  const double in_x_offset,
  const double in_y_offset)
{
  #ifdef DEBUG
  fprintf(
    stdout,
    "Event - Scroll Wheel - X: %lf Y: %lf\n",
    in_x_offset,
    in_y_offset);
  #endif

  struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(in_window);

  struct event_mouse_scrolled_t mouse_scrolled_event =
    (struct event_mouse_scrolled_t){
      .base = (struct event_base_t){.event_type = EVENT_TYPE_MOUSE_SCROLLED, .is_handled = 0},
      .window_handle = (void*)in_window,
      .x_offset = (float)in_x_offset,
      .y_offset = (float)in_y_offset};

  metadata->function_event_notify(
    metadata->parent_application,
    &(mouse_scrolled_event.base));
}

/*******************************************************************************
 *
 *   Function: window_gl_cursor_position_callback()
 *
 ******************************************************************************/
static void window_gl_cursor_position_callback(
  GLFWwindow* in_window,
  const double in_x_pos,
  const double in_y_pos)
{
  // #ifdef DEBUG
  //  fprintf(stdout, "Event - Cursor Position - X: %lf Y: %lf\n", x_pos,
  //  y_pos);
  // #endif

  struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(in_window);

  struct event_mouse_moved_t mouse_moved_event = (struct event_mouse_moved_t){
    .base = (struct event_base_t){.event_type = EVENT_TYPE_MOUSE_MOVED, .is_handled = 0},
    .window_handle = (void*)in_window,
    .x = (float)in_x_pos,
    .y = (float)(metadata->height - in_y_pos)};

  metadata->function_event_notify(
    metadata->parent_application,
    &(mouse_moved_event.base));
}

/*******************************************************************************
 *
 *   Function: window_gl_focus_callback()
 *
 ******************************************************************************/
static void window_gl_focus_callback(
  GLFWwindow* in_window,
  const int in_is_focused)
{
  struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(in_window);

  // #ifdef DEBUG
  // if (focused)
  // {
  //   fprintf(stdout, "Window focus changed to %s\n", metadata->tag);
  // }
  // #endif

  struct event_window_focus_t window_focus_event =
    (struct event_window_focus_t){
      .base = (struct event_base_t){.event_type = EVENT_TYPE_WINDOW_FOCUS, .is_handled = 0},
      .window_handle = (void*)in_window,
      .is_focused = in_is_focused};

  metadata->function_event_notify(
    metadata->parent_application,
    &(window_focus_event.base));
}

/*******************************************************************************
 *
 *   Function: window_gl_error_callback()
 *
 ******************************************************************************/
static void window_gl_error_callback(
  const int in_error_code,
  const char* in_description)
{
  #ifdef DEBUG
  log_write(
    LOG_LEVEL_ERROR,
    "Error Callback - Code: %d, Description: %s",
    in_error_code,
    in_description);
  #endif
}

/*******************************************************************************
 *
 *   GL WINDOW FUNCTIONS
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *   Function: window_gl_set_callbacks()
 *
 ******************************************************************************/
static void window_gl_set_callbacks(
  GLFWwindow* inout_glfw_window_ptr)
{
  // Window action callback assignment
  glfwSetWindowSizeCallback(
    inout_glfw_window_ptr,
    (GLFWwindowsizefun)&(window_gl_size_callback));
  glfwSetFramebufferSizeCallback(
    inout_glfw_window_ptr,
    (GLFWframebuffersizefun)&(window_gl_framebuffer_size_callback));
  glfwSetWindowCloseCallback(
    inout_glfw_window_ptr,
    (GLFWwindowclosefun)&(window_gl_close_callback));
  glfwSetKeyCallback(
    inout_glfw_window_ptr,
    (GLFWkeyfun)&(window_gl_key_callback));
  glfwSetCharCallback(
    inout_glfw_window_ptr,
    (GLFWcharfun)&(window_gl_char_callback));
  glfwSetMouseButtonCallback(
    inout_glfw_window_ptr,
    (GLFWmousebuttonfun)&(window_gl_mouse_button_callback));
  glfwSetScrollCallback(
    inout_glfw_window_ptr,
    (GLFWscrollfun)&(window_gl_scroll_callback));
  glfwSetCursorPosCallback(
    inout_glfw_window_ptr,
    (GLFWcursorposfun)&(window_gl_cursor_position_callback));
  glfwSetWindowFocusCallback(
    inout_glfw_window_ptr,
    (GLFWwindowfocusfun)&(window_gl_focus_callback));
  glfwSetCursorEnterCallback(
    inout_glfw_window_ptr,
    (GLFWcursorenterfun)&(_window_gl_cursor_enter_callback));
}

/*******************************************************************************
 *
 *   Function: window_gl_init()
 *
 ******************************************************************************/
static uint8_t window_gl_init(
  struct window_t* inout_window)
{
#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Initializing OpenGL Window");
#endif

  // Handle GLFW initialization
  if (!is_glfw_initialized) {
    is_glfw_initialized = glfwInit();
  }

  if (is_glfw_initialized) {
    glfwSetErrorCallback((GLFWerrorfun)window_gl_error_callback);

    // Set some GLFW settings such as GL context version, modern core profile
    //  for the context, etc.
    glfwWindowHint(
      GLFW_CONTEXT_VERSION_MAJOR,
      3);
    glfwWindowHint(
      GLFW_CONTEXT_VERSION_MINOR,
      2);
    glfwWindowHint(
      GLFW_OPENGL_PROFILE,
      GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(
      GLFW_OPENGL_FORWARD_COMPAT,
      GL_TRUE);
    glfwWindowHint(
      GLFW_RESIZABLE,
      GL_TRUE);
  }
#ifdef DEBUG
  else {
    log_write(
      LOG_LEVEL_ERROR,
      "GLFW initialization failed");
  }

  log_write(
    LOG_LEVEL_INFO,
    "Creating Window");
#endif

  // Create window via GLFW
  GLFWwindow* gl_window_ptr = glfwCreateWindow(
    inout_window->metadata.width,
    inout_window->metadata.height,
    inout_window->metadata.tag,
    NULL,
    NULL);

  // Assign to our window handle as void*
  inout_window->context_data.window_handle = (void*)gl_window_ptr;

  if (gl_window_ptr != NULL) {
    open_window_count++;
  }

  // Set a pointer in GLFW to our window data
  glfwSetWindowUserPointer(
    gl_window_ptr,
    (void*)&(inout_window->metadata));

  window_gl_set_callbacks(gl_window_ptr);

  window_gl_set_context(inout_window);

  // printOpenGLInfo(gl_window_ptr);

  // Set the GL viewport to the size of the window
  glViewport(
    0,
    0,
    inout_window->metadata.width,
    inout_window->metadata.height);

  if (!is_glew_initialized) {
    glewExperimental = GL_TRUE;
    is_glew_initialized = (glewInit() == GLEW_OK);
  }

  // window_gl_release_context();

  GLenum gl_error = GL_NO_ERROR;

#ifdef DEBUG
  log_write(
    LOG_LEVEL_DEBUG,
    "End of Window Graphics Init - Clearing error queue");
#endif

  do
  {
    gl_error = glGetError();

  #ifdef DEBUG
    // The queue always ends with GL_NO_ERROR, which isn't worth a warning
    log_write(
      (gl_error == GL_NO_ERROR) ? LOG_LEVEL_DEBUG : LOG_LEVEL_WARNING,
      "Error Code: %d",
      gl_error);
  #endif

  } while (gl_error != GL_NO_ERROR);

#ifdef DEBUG
  log_write(
    LOG_LEVEL_DEBUG,
    "Done clearing error queue");
#endif

  return !(is_glfw_initialized && is_glew_initialized);
}

/*******************************************************************************
 *
 *   Function: window_gl_set_context()
 *
 ******************************************************************************/
static void window_gl_set_context(
  const struct window_t* in_window)
{
// #ifdef DEBUG
//   fprintf(stdout, "Setting context to window '%s'\n", window->metadata.tag);
// #endif

  GLFWwindow* gl_window_handle = (GLFWwindow*)(in_window->context_data.window_handle);

  glfwMakeContextCurrent(gl_window_handle);
}

/*******************************************************************************
 *
 *   Function: window_gl_release_context()
 *
 ******************************************************************************/
static void window_gl_release_context(
  const struct window_t* in_window)
{
// #ifdef DEBUG
//  fprintf(stdout, "Releasing context from window '%s'\n",
//  window->metadata.tag);
// #endif
  glfwMakeContextCurrent(NULL);
}

/*******************************************************************************
 *
 *   Function: window_gl_poll_events()
 *
 ******************************************************************************/
static void window_gl_poll_events(
  struct window_t* inout_window)
{
  glfwPollEvents();
}

/*******************************************************************************
 *
 *   Function: window_gl_run()
 *
 ******************************************************************************/
static void window_gl_run(
  const struct window_t* in_window)
{
  GLFWwindow* gl_window_ptr;
  gl_window_ptr = (GLFWwindow*)in_window->context_data.window_handle;

  glfwSwapBuffers(gl_window_ptr);
}

/*******************************************************************************
 *
 *   Function: window_gl_cleanup()
 *
 ******************************************************************************/
static void window_gl_cleanup(
  const struct window_t* in_window)
{
#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Cleaning up OpenGL Window");
#endif

  GLFWwindow* gl_window_ptr = (GLFWwindow*)in_window->context_data.window_handle;

  if (is_glfw_initialized &&
      (gl_window_ptr != NULL))
  {
    glfwDestroyWindow(gl_window_ptr);
    open_window_count--;
  }

  // Other windows still need GLFW, and its standard cursors
  if (open_window_count > 0) {
    return;
  }

  if (is_glfw_initialized) {
    glfwTerminate();

    // Terminating destroyed any standard cursors that were created
    memset(
      gl_cursors,
      0,
      sizeof(gl_cursors));
    memset(
      is_gl_cursor_created,
      0,
      sizeof(is_gl_cursor_created));

    is_glfw_initialized = 0;
  }

  if (is_glew_initialized) {
    is_glew_initialized = 0;
  }
}

/*******************************************************************************
 *
 *   Function: _window_gl_cursor_enter_callback()
 *
 ******************************************************************************/
static void _window_gl_cursor_enter_callback(
  GLFWwindow* in_window,
  const int in_is_entered)
{
  struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(in_window);

  struct event_mouse_entered_t mouse_entered_event =
    (struct event_mouse_entered_t){
      .base = (struct event_base_t){.event_type = EVENT_TYPE_MOUSE_ENTERED, .is_handled = 0},
      .window_handle = (void*)in_window,
      .is_entered = (in_is_entered == GLFW_TRUE)};

  metadata->function_event_notify(
    metadata->parent_application,
    &(mouse_entered_event.base));
}

/*******************************************************************************
 *
 *   Function: _window_gl_translate_key()
 *
 ******************************************************************************/
static enum event_key_code_t _window_gl_translate_key(
  const int in_glfw_key)
{
  // Digits, letters, function keys and keypad digits are contiguous in both
  //  GLFW's key codes and Jotunn's
  if ((in_glfw_key >= GLFW_KEY_0) &&
      (in_glfw_key <= GLFW_KEY_9))
  {
    return (EVENT_KEY_CODE_0 + (in_glfw_key - GLFW_KEY_0));
  }
  if ((in_glfw_key >= GLFW_KEY_A) &&
      (in_glfw_key <= GLFW_KEY_Z))
  {
    return (EVENT_KEY_CODE_A + (in_glfw_key - GLFW_KEY_A));
  }
  if ((in_glfw_key >= GLFW_KEY_F1) &&
      (in_glfw_key <= GLFW_KEY_F24))
  {
    return (EVENT_KEY_CODE_F1 + (in_glfw_key - GLFW_KEY_F1));
  }
  if ((in_glfw_key >= GLFW_KEY_KP_0) &&
      (in_glfw_key <= GLFW_KEY_KP_9))
  {
    return (EVENT_KEY_CODE_KEYPAD_0 + (in_glfw_key - GLFW_KEY_KP_0));
  }

  enum event_key_code_t key_code;

  switch (in_glfw_key)
  {
    case GLFW_KEY_TAB:           key_code = EVENT_KEY_CODE_TAB;             break;
    case GLFW_KEY_LEFT:          key_code = EVENT_KEY_CODE_LEFT_ARROW;      break;
    case GLFW_KEY_RIGHT:         key_code = EVENT_KEY_CODE_RIGHT_ARROW;     break;
    case GLFW_KEY_UP:            key_code = EVENT_KEY_CODE_UP_ARROW;        break;
    case GLFW_KEY_DOWN:          key_code = EVENT_KEY_CODE_DOWN_ARROW;      break;
    case GLFW_KEY_PAGE_UP:       key_code = EVENT_KEY_CODE_PAGE_UP;         break;
    case GLFW_KEY_PAGE_DOWN:     key_code = EVENT_KEY_CODE_PAGE_DOWN;       break;
    case GLFW_KEY_HOME:          key_code = EVENT_KEY_CODE_HOME;            break;
    case GLFW_KEY_END:           key_code = EVENT_KEY_CODE_END;             break;
    case GLFW_KEY_INSERT:        key_code = EVENT_KEY_CODE_INSERT;          break;
    case GLFW_KEY_DELETE:        key_code = EVENT_KEY_CODE_DELETE;          break;
    case GLFW_KEY_BACKSPACE:     key_code = EVENT_KEY_CODE_BACKSPACE;       break;
    case GLFW_KEY_SPACE:         key_code = EVENT_KEY_CODE_SPACE;           break;
    case GLFW_KEY_ENTER:         key_code = EVENT_KEY_CODE_ENTER;           break;
    case GLFW_KEY_ESCAPE:        key_code = EVENT_KEY_CODE_ESCAPE;          break;
    case GLFW_KEY_LEFT_CONTROL:  key_code = EVENT_KEY_CODE_LEFT_CONTROL;    break;
    case GLFW_KEY_LEFT_SHIFT:    key_code = EVENT_KEY_CODE_LEFT_SHIFT;      break;
    case GLFW_KEY_LEFT_ALT:      key_code = EVENT_KEY_CODE_LEFT_ALT;        break;
    case GLFW_KEY_LEFT_SUPER:    key_code = EVENT_KEY_CODE_LEFT_SUPER;      break;
    case GLFW_KEY_RIGHT_CONTROL: key_code = EVENT_KEY_CODE_RIGHT_CONTROL;   break;
    case GLFW_KEY_RIGHT_SHIFT:   key_code = EVENT_KEY_CODE_RIGHT_SHIFT;     break;
    case GLFW_KEY_RIGHT_ALT:     key_code = EVENT_KEY_CODE_RIGHT_ALT;       break;
    case GLFW_KEY_RIGHT_SUPER:   key_code = EVENT_KEY_CODE_RIGHT_SUPER;     break;
    case GLFW_KEY_MENU:          key_code = EVENT_KEY_CODE_MENU;            break;
    case GLFW_KEY_APOSTROPHE:    key_code = EVENT_KEY_CODE_APOSTROPHE;      break;
    case GLFW_KEY_COMMA:         key_code = EVENT_KEY_CODE_COMMA;           break;
    case GLFW_KEY_MINUS:         key_code = EVENT_KEY_CODE_MINUS;           break;
    case GLFW_KEY_PERIOD:        key_code = EVENT_KEY_CODE_PERIOD;          break;
    case GLFW_KEY_SLASH:         key_code = EVENT_KEY_CODE_SLASH;           break;
    case GLFW_KEY_SEMICOLON:     key_code = EVENT_KEY_CODE_SEMICOLON;       break;
    case GLFW_KEY_EQUAL:         key_code = EVENT_KEY_CODE_EQUAL;           break;
    case GLFW_KEY_LEFT_BRACKET:  key_code = EVENT_KEY_CODE_LEFT_BRACKET;    break;
    case GLFW_KEY_BACKSLASH:     key_code = EVENT_KEY_CODE_BACKSLASH;       break;
    case GLFW_KEY_RIGHT_BRACKET: key_code = EVENT_KEY_CODE_RIGHT_BRACKET;   break;
    case GLFW_KEY_GRAVE_ACCENT:  key_code = EVENT_KEY_CODE_GRAVE_ACCENT;    break;
    case GLFW_KEY_CAPS_LOCK:     key_code = EVENT_KEY_CODE_CAPS_LOCK;       break;
    case GLFW_KEY_SCROLL_LOCK:   key_code = EVENT_KEY_CODE_SCROLL_LOCK;     break;
    case GLFW_KEY_NUM_LOCK:      key_code = EVENT_KEY_CODE_NUM_LOCK;        break;
    case GLFW_KEY_PRINT_SCREEN:  key_code = EVENT_KEY_CODE_PRINT_SCREEN;    break;
    case GLFW_KEY_PAUSE:         key_code = EVENT_KEY_CODE_PAUSE;           break;
    case GLFW_KEY_KP_DECIMAL:    key_code = EVENT_KEY_CODE_KEYPAD_DECIMAL;  break;
    case GLFW_KEY_KP_DIVIDE:     key_code = EVENT_KEY_CODE_KEYPAD_DIVIDE;   break;
    case GLFW_KEY_KP_MULTIPLY:   key_code = EVENT_KEY_CODE_KEYPAD_MULTIPLY; break;
    case GLFW_KEY_KP_SUBTRACT:   key_code = EVENT_KEY_CODE_KEYPAD_SUBTRACT; break;
    case GLFW_KEY_KP_ADD:        key_code = EVENT_KEY_CODE_KEYPAD_ADD;      break;
    case GLFW_KEY_KP_ENTER:      key_code = EVENT_KEY_CODE_KEYPAD_ENTER;    break;
    case GLFW_KEY_KP_EQUAL:      key_code = EVENT_KEY_CODE_KEYPAD_EQUAL;    break;
    case GLFW_KEY_WORLD_1:       key_code = EVENT_KEY_CODE_OEM_102;         break;
    default:                     key_code = EVENT_KEY_CODE_UNKNOWN;         break;
  }

  return key_code;
}

/*******************************************************************************
 *
 *   Function: _window_gl_translate_modifiers()
 *
 ******************************************************************************/
static uint8_t _window_gl_translate_modifiers(
  const int in_glfw_modifiers)
{
  uint8_t modifiers = EVENT_KEY_MODIFIER_NONE;

  if (in_glfw_modifiers &
      GLFW_MOD_SHIFT)
  {
    modifiers |= EVENT_KEY_MODIFIER_SHIFT;
  }
  if (in_glfw_modifiers &
      GLFW_MOD_CONTROL)
  {
    modifiers |= EVENT_KEY_MODIFIER_CONTROL;
  }
  if (in_glfw_modifiers &
      GLFW_MOD_ALT)
  {
    modifiers |= EVENT_KEY_MODIFIER_ALT;
  }
  if (in_glfw_modifiers &
      GLFW_MOD_SUPER)
  {
    modifiers |= EVENT_KEY_MODIFIER_SUPER;
  }

  return modifiers;
}

/*******************************************************************************
 *
 *   Function: _window_gl_get_cursor()
 *
 ******************************************************************************/
static GLFWcursor* _window_gl_get_cursor(
  const enum window_cursor_shape_t in_cursor_shape)
{
  if ((in_cursor_shape < 0) ||
      (in_cursor_shape >= WINDOW_CURSOR_SHAPE_COUNT))
  {
    return NULL;
  }

  if (!is_gl_cursor_created[in_cursor_shape]) {
    int glfw_shape;

    switch (in_cursor_shape)
    {
      case WINDOW_CURSOR_SHAPE_TEXT_INPUT:  glfw_shape = GLFW_IBEAM_CURSOR;         break;
      case WINDOW_CURSOR_SHAPE_RESIZE_ALL:  glfw_shape = GLFW_RESIZE_ALL_CURSOR;    break;
      case WINDOW_CURSOR_SHAPE_RESIZE_NS:   glfw_shape = GLFW_RESIZE_NS_CURSOR;     break;
      case WINDOW_CURSOR_SHAPE_RESIZE_EW:   glfw_shape = GLFW_RESIZE_EW_CURSOR;     break;
      case WINDOW_CURSOR_SHAPE_RESIZE_NESW: glfw_shape = GLFW_RESIZE_NESW_CURSOR;   break;
      case WINDOW_CURSOR_SHAPE_RESIZE_NWSE: glfw_shape = GLFW_RESIZE_NWSE_CURSOR;   break;
      case WINDOW_CURSOR_SHAPE_HAND:        glfw_shape = GLFW_POINTING_HAND_CURSOR; break;
      case WINDOW_CURSOR_SHAPE_NOT_ALLOWED: glfw_shape = GLFW_NOT_ALLOWED_CURSOR;   break;
      // GLFW has no busy cursors, so those fall back to the arrow
      case WINDOW_CURSOR_SHAPE_ARROW:
      case WINDOW_CURSOR_SHAPE_WAIT:
      case WINDOW_CURSOR_SHAPE_PROGRESS:
      default:                              glfw_shape = GLFW_ARROW_CURSOR;         break;
    }

    // Some platforms lack some shapes, leaving NULL and so the default arrow
    gl_cursors[in_cursor_shape] = glfwCreateStandardCursor(glfw_shape);
    is_gl_cursor_created[in_cursor_shape] = true;
  }

  return gl_cursors[in_cursor_shape];
}
