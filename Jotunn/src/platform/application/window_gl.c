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

/**************************************************
 *
 *  Variables
 *
 **************************************************/

static uint8_t is_glfw_initialized = 0;
static uint8_t is_glew_initialized = 0;

// Standard cursors are created on first use and shared between windows; GLFW
//  destroys them itself when it terminates
static GLFWcursor* gl_cursors[WINDOW_CURSOR_SHAPE_COUNT];
static bool is_gl_cursor_created[WINDOW_CURSOR_SHAPE_COUNT];

/**************************************************
 *
 *  GLFW callback functions
 *
 **************************************************/

static void gl_window_size_callback(
  GLFWwindow* window,
  int width,
  int height);
static void gl_framebuffer_size_callback(
  GLFWwindow* window,
  int width,
  int height);
static void gl_window_close_callback(
  GLFWwindow* window);
static void gl_window_key_callback(
  GLFWwindow* window,
  int key,
  int scancode,
  int action,
  int mods);
static void gl_window_char_callback(
  GLFWwindow* window,
  unsigned int codepoint);
static void gl_window_mouse_button_callback(
  GLFWwindow* window,
  int button,
  int action,
  int mods);
static void gl_window_scroll_callback(
  GLFWwindow* window,
  double x_offset,
  double y_offset);
static void gl_window_cursor_position_callback(
  GLFWwindow* window,
  double x_pos,
  double y_pos);
static void gl_window_focus_callback(
  GLFWwindow* window,
  int focused);

static void gl_window_error_callback(
  int error_code,
  const char* description);
static void _gl_window_cursor_enter_callback(
  GLFWwindow* in_window,
  int in_is_entered);

static enum event_key_code_t _window_gl_translate_key(
  const int in_glfw_key);
static uint8_t _window_gl_translate_modifiers(
  const int in_glfw_modifiers);
static GLFWcursor* _window_gl_get_cursor(
  const enum window_cursor_shape_t in_cursor_shape);

/**************************************************
 *
 *  Internal GL window functions
 *
 **************************************************/
static void window_gl_set_callbacks(
  GLFWwindow* glfw_window_ptr);

static uint8_t window_gl_init(
  struct window_t* window);

void window_gl_set_context(
  struct window_t* window);
void window_gl_release_context(
  struct window_t* window);

static void window_gl_poll_events(
  struct window_t* window);

static void window_gl_run(
  struct window_t* window);

static void window_gl_cleanup(
  struct window_t* window);

/**************************************************
 *
 *  Extern window function definitions
 *
 **************************************************/
uint8_t window_graphics_init(
  struct window_t* window)
{
#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Found OpenGL Backend");
#endif

  return window_gl_init(window);
}

void window_graphics_poll_events(
  struct window_t* window)
{
  window_gl_poll_events(window);
}

void window_graphics_run(
  struct window_t* window)
{
  window_gl_run(window);
}

void window_graphics_cleanup(
  struct window_t* window)
{
  window_gl_cleanup(window);
}

void window_graphics_set_context(
  struct window_t* window)
{
  window_gl_set_context(window);
}

void window_graphics_release_context(
  struct window_t* window)
{
  window_gl_release_context(window);
}

void window_graphics_set_viewport(
  uint32_t x,
  uint32_t y,
  uint32_t width,
  uint32_t height)
{
  glViewport(x, y, width, height);
}

void window_graphics_set_background_color(
  struct window_t* window,
  const fvector4 color)
{
  render_api_set_clear_color(
    &(window->window_layer_renderer.base.render_api),
    color);
}

/*******************************************************************************
 *
 *   Function: window_graphics_set_cursor_shape()
 *
 ******************************************************************************/
void window_graphics_set_cursor_shape(
  const struct window_t* in_window,
  const enum window_cursor_shape_t in_cursor_shape)
{
  GLFWwindow* gl_window_ptr = (GLFWwindow*)in_window->context_data.window_handle;

  if (in_cursor_shape == WINDOW_CURSOR_SHAPE_HIDDEN)
  {
    glfwSetInputMode(
      gl_window_ptr,
      GLFW_CURSOR,
      GLFW_CURSOR_HIDDEN);
  }
  else
  {
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
 *   Function: window_graphics_get_clipboard_text()
 *
 ******************************************************************************/
const char* window_graphics_get_clipboard_text(
  const struct window_t* in_window)
{
  return glfwGetClipboardString((GLFWwindow*)in_window->context_data.window_handle);
}

/*******************************************************************************
 *
 *   Function: window_graphics_set_clipboard_text()
 *
 ******************************************************************************/
void window_graphics_set_clipboard_text(
  const struct window_t* in_window,
  const char* in_text)
{
  glfwSetClipboardString(
    (GLFWwindow*)in_window->context_data.window_handle,
    in_text);
}

/**************************************************
 *
 *  GLFW callback functions
 *
 **************************************************/

static void gl_window_size_callback(
  GLFWwindow* window,
  int width,
  int height)
{
  #ifdef DEBUG
  fprintf(stdout, "Event - Window Resize - W: %d H: %d\n", width, height);
  #endif

  struct window_data_t* metadata =
    (struct window_data_t*)glfwGetWindowUserPointer(window);
  metadata->width = width;
  metadata->height = height;
  metadata->resized = 1;

  struct event_window_resize_t resize_event = (struct event_window_resize_t){
    .base =
      (struct event_base_t){.event_type = EVENT_WINDOW_RESIZE, .handled = 0},
    .window_handle = (void*)window,
    .width = width,
    .height = height};

  metadata->function_event_notify(
    metadata->parent_application, &(resize_event.base));
}

static void gl_framebuffer_size_callback(
  GLFWwindow* window,
  int width,
  int height)
{
  #ifdef DEBUG
  fprintf(stdout, "Event - Framebuffer Resize - W: %d H: %d\n", width, height);
  #endif

  struct window_data_t* metadata =
    (struct window_data_t*)glfwGetWindowUserPointer(window);
  metadata->width = width;
  metadata->height = height;

  // Set the GL viewport to the size of the window
  // glViewport(0, 0, metadata->width, metadata->height);
}

static void gl_window_close_callback(
  GLFWwindow* window)
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Event - Window Close\n");
  // #endif

  struct window_data_t* metadata =
    (struct window_data_t*)glfwGetWindowUserPointer(window);

  metadata->signaled_close = 1;

  struct event_window_close_t window_close_event =
    (struct event_window_close_t){
      .base =
        (struct event_base_t){.event_type = EVENT_WINDOW_CLOSE, .handled = 0},
      .window_handle = (void*)window};

  metadata->function_event_notify(
    metadata->parent_application, &(window_close_event.base));
}

static void gl_window_key_callback(
  GLFWwindow* window,
  int key,
  int scancode,
  int action,
  int mods)
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

  struct window_data_t* metadata =
    (struct window_data_t*)glfwGetWindowUserPointer(window);

  const enum event_key_code_t key_code = _window_gl_translate_key(key);
  const uint8_t modifiers = _window_gl_translate_modifiers(mods);

  if (action == GLFW_RELEASE)
  {
    struct event_key_released_t key_released_event =
      (struct event_key_released_t){
        .base =
          (struct event_base_t){.event_type = EVENT_KEY_RELEASED, .handled = 0},
        .window_handle = (void*)window,
        .key = key_code,
        .modifiers = modifiers};

    metadata->function_event_notify(
      metadata->parent_application,
      &(key_released_event.base));
  }
  else
  {
    struct event_key_pressed_t key_pressed_event =
      (struct event_key_pressed_t){
        .base =
          (struct event_base_t){.event_type = EVENT_KEY_PRESSED, .handled = 0},
        .window_handle = (void*)window,
        .key = key_code,
        .modifiers = modifiers,
        .is_repeat = (action == GLFW_REPEAT)};

    metadata->function_event_notify(
      metadata->parent_application,
      &(key_pressed_event.base));
  }
}

static void gl_window_char_callback(
  GLFWwindow* window,
  unsigned int codepoint)
{
  // #ifdef DEBUG
  // fprintf(stdout, "Event - Char Received - Char: %c\n", codepoint);
  // #endif

  struct window_data_t* metadata =
    (struct window_data_t*)glfwGetWindowUserPointer(window);

  struct event_key_typed_t key_typed_event =
    (struct event_key_typed_t){
      .base =
        (struct event_base_t){.event_type = EVENT_KEY_TYPED, .handled = 0},
      .window_handle = (void*)window,
      .codepoint = codepoint};

  metadata->function_event_notify(
    metadata->parent_application,
    &(key_typed_event.base));
}

static void gl_window_mouse_button_callback(
  GLFWwindow* window,
  int button,
  int action,
  int mods)
{
  // #ifdef DEBUG
  // fprintf(
  //   stdout,
  //   "Event - Mouse Button - Button %d Action %d Mods %d\n",
  //   button,
  //   action,
  //   mods);
  // #endif

  struct window_data_t* metadata =
    (struct window_data_t*)glfwGetWindowUserPointer(window);

  struct event_mouse_button_t mouse_button_event =
    (struct event_mouse_button_t){
      .base =
        (struct event_base_t){.event_type = EVENT_MOUSE_BUTTON, .handled = 0},
      .window_handle = (void*)window,
      .button = button,
      .action = action,
      .mods = _window_gl_translate_modifiers(mods)};

  metadata->function_event_notify(
    metadata->parent_application, &(mouse_button_event.base));
}

static void gl_window_scroll_callback(
  GLFWwindow* window,
  double x_offset,
  double y_offset)
{
  #ifdef DEBUG
  fprintf(stdout, "Event - Scroll Wheel - X: %lf Y: %lf\n", x_offset, y_offset);
  #endif

  struct window_data_t* metadata =
    (struct window_data_t*)glfwGetWindowUserPointer(window);

  struct event_mouse_scrolled_t mouse_scrolled_event =
    (struct event_mouse_scrolled_t){
      .base =
        (struct event_base_t){.event_type = EVENT_MOUSE_SCROLLED, .handled = 0},
      .window_handle = (void*)window,
      .x_offset = (float)x_offset,
      .y_offset = (float)y_offset};

  metadata->function_event_notify(
    metadata->parent_application, &(mouse_scrolled_event.base));
}

static void gl_window_cursor_position_callback(
  GLFWwindow* window,
  double x_pos,
  double y_pos)
{
  // #ifdef DEBUG
  //    fprintf(stdout, "Event - Cursor Position - X: %lf Y: %lf\n", x_pos, y_pos);
  // #endif

  struct window_data_t* metadata =
    (struct window_data_t*)glfwGetWindowUserPointer(window);

  struct event_mouse_moved_t mouse_moved_event = (struct event_mouse_moved_t){
    .base =
      (struct event_base_t){.event_type = EVENT_MOUSE_MOVED, .handled = 0},
    .window_handle = (void*)window,
    .x = (float)x_pos,
    .y = (float)(metadata->height - y_pos)};

  metadata->function_event_notify(
    metadata->parent_application, &(mouse_moved_event.base));
}

static void gl_window_focus_callback(
  GLFWwindow* window,
  int focused)
{
  struct window_data_t* metadata =
    (struct window_data_t*)glfwGetWindowUserPointer(window);

  // #ifdef DEBUG
  // if (focused)
  // {
  //   fprintf(stdout, "Window focus changed to %s\n", metadata->tag);
  // }
  // #endif

  struct event_window_focus_t window_focus_event =
    (struct event_window_focus_t){
      .base =
        (struct event_base_t){.event_type = EVENT_WINDOW_FOCUS, .handled = 0},
      .window_handle = (void*)window,
      .focused = focused};

  metadata->function_event_notify(
    metadata->parent_application, &(window_focus_event.base));
}

static void gl_window_error_callback(
  int error_code,
  const char* description)
{
  #ifdef DEBUG
  log_write(
    LOG_LEVEL_ERROR,
    "Error Callback - Code: %d, Description: %s",
    error_code,
    description);
  #endif
}

/**************************************************
 *
 *  Internal GL window functions
 *
 **************************************************/
static void window_gl_set_callbacks(
  GLFWwindow* glfw_window_ptr)
{
  // Window action callback assignment
  glfwSetWindowSizeCallback(
    glfw_window_ptr, (GLFWwindowsizefun)&gl_window_size_callback);
  glfwSetFramebufferSizeCallback(
    glfw_window_ptr, (GLFWframebuffersizefun)&gl_framebuffer_size_callback);
  glfwSetWindowCloseCallback(
    glfw_window_ptr, (GLFWwindowclosefun)&gl_window_close_callback);
  glfwSetKeyCallback(glfw_window_ptr, (GLFWkeyfun)&gl_window_key_callback);
  glfwSetCharCallback(glfw_window_ptr, (GLFWcharfun)&gl_window_char_callback);
  glfwSetMouseButtonCallback(
    glfw_window_ptr, (GLFWmousebuttonfun)&gl_window_mouse_button_callback);
  glfwSetScrollCallback(
    glfw_window_ptr, (GLFWscrollfun)&gl_window_scroll_callback);
  glfwSetCursorPosCallback(
    glfw_window_ptr, (GLFWcursorposfun)&gl_window_cursor_position_callback);
  glfwSetWindowFocusCallback(
    glfw_window_ptr, (GLFWwindowfocusfun)&gl_window_focus_callback);
  glfwSetCursorEnterCallback(
    glfw_window_ptr,
    (GLFWcursorenterfun)&(_gl_window_cursor_enter_callback));
}

static uint8_t window_gl_init(
  struct window_t* window)
{
#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Initializing OpenGL Window");
#endif

  // Handle GLFW initialization
  if (!is_glfw_initialized)
  {
    is_glfw_initialized = glfwInit();
  }

  if (is_glfw_initialized)
  {
    glfwSetErrorCallback((GLFWerrorfun)gl_window_error_callback);

    // Set some GLFW settings such as GL context version, modern core profile for the context, etc.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
  }
#ifdef DEBUG
  else
  {
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
    window->metadata.width,
    window->metadata.height,
    window->metadata.tag,
    NULL,
    NULL);

  // Assign to our window handle as void*
  window->context_data.window_handle = (void*)gl_window_ptr;

  // Set a pointer in GLFW to our window data
  glfwSetWindowUserPointer(gl_window_ptr, (void*)&(window->metadata));

  window_gl_set_callbacks(gl_window_ptr);

  window_gl_set_context(window);

  // printOpenGLInfo(gl_window_ptr);

  // Set the GL viewport to the size of the window
  glViewport(0, 0, window->metadata.width, window->metadata.height);

  if (!is_glew_initialized)
  {
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

void window_gl_set_context(
  struct window_t* window)
{
// #ifdef DEBUG
//   fprintf(stdout, "Setting context to window '%s'\n", window->metadata.tag);
// #endif

  GLFWwindow* gl_window_handle =
    (GLFWwindow*)(window->context_data.window_handle);

  glfwMakeContextCurrent(gl_window_handle);
}

void window_gl_release_context(
  struct window_t* window)
{
// #ifdef DEBUG
//   fprintf(stdout, "Releasing context from window '%s'\n", window->metadata.tag);
// #endif
  glfwMakeContextCurrent(NULL);
}

static void window_gl_poll_events(
  struct window_t* window)
{
  glfwPollEvents();
}

static void window_gl_run(
  struct window_t* window)
{
  GLFWwindow* gl_window_ptr;
  gl_window_ptr = (GLFWwindow*)window->context_data.window_handle;

  glfwSwapBuffers(gl_window_ptr);
}

static void window_gl_cleanup(
  struct window_t* window)
{
#ifdef DEBUG
  log_write(
    LOG_LEVEL_INFO,
    "Cleaning up OpenGL Window");
#endif

  if (is_glfw_initialized)
  {
    GLFWwindow* gl_window_ptr = (GLFWwindow*)window->context_data.window_handle;

    glfwDestroyWindow(gl_window_ptr);
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

  if (is_glew_initialized)
  {
    is_glew_initialized = 0;
  }
}

/*******************************************************************************
 *
 *   Function: _gl_window_cursor_enter_callback()
 *
 ******************************************************************************/
static void _gl_window_cursor_enter_callback(
  GLFWwindow* in_window,
  int in_is_entered)
{
  struct window_data_t* metadata =
    (struct window_data_t*)glfwGetWindowUserPointer(in_window);

  struct event_mouse_entered_t mouse_entered_event =
    (struct event_mouse_entered_t){
      .base =
        (struct event_base_t){.event_type = EVENT_MOUSE_ENTERED, .handled = 0},
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

  if (in_glfw_modifiers & GLFW_MOD_SHIFT)
  {
    modifiers |= EVENT_KEY_MODIFIER_SHIFT;
  }
  if (in_glfw_modifiers & GLFW_MOD_CONTROL)
  {
    modifiers |= EVENT_KEY_MODIFIER_CONTROL;
  }
  if (in_glfw_modifiers & GLFW_MOD_ALT)
  {
    modifiers |= EVENT_KEY_MODIFIER_ALT;
  }
  if (in_glfw_modifiers & GLFW_MOD_SUPER)
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

  if (!is_gl_cursor_created[in_cursor_shape])
  {
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
