#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include "fvector.h"
#include "render_api.h"
#include "renderer_2d.h"
#include "window.h"

/**************************************************
 * 
 *  Variables
 * 
 **************************************************/

static int is_glfw_initialized = 0;
static int is_glew_initialized = 0;

/**************************************************
 * 
 *  GLFW callback functions
 * 
 **************************************************/

void gl_window_size_callback(GLFWwindow* window, int width, int height)
{
   #ifdef DEBUG
      fprintf(stdout, "Event - Window Resize - W: %d H: %d\n", width, height);
   #endif

   struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(window);
   metadata->width  = width;
   metadata->height = height;

   // Set the GL viewport to the size of the window
   glViewport(0, 0, metadata->width, metadata->height);
}

void gl_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   #ifdef DEBUG
      fprintf(stdout, "Event - Framebuffer Resize - W: %d H: %d\n", width, height);
   #endif

   // struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(window);
   // metadata->width  = width;
   // metadata->height = height;

   // Set the GL viewport to the size of the window
   // glViewport(0, 0, metadata->width, metadata->height);
}

void gl_window_close_callback(GLFWwindow* window)
{
   #ifdef DEBUG
      fprintf(stdout, "Event - Window Close\n");
   #endif

   struct window_data_t* metadata = (struct window_data_t*)glfwGetWindowUserPointer(window);
   metadata->signaled_close = 1;
}

void gl_window_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   #ifdef DEBUG
      fprintf(stdout, "Event - Key - Key: %c Scancode: %d Action: %d Mods: %d\n", key, scancode, action, mods);
   #endif
}

void gl_window_char_callback(GLFWwindow* window, unsigned int codepoint)
{
   #ifdef DEBUG
      fprintf(stdout, "Event - Char Received - Char: %c\n", codepoint);
   #endif
}

void gl_window_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
   #ifdef DEBUG
      fprintf(stdout, "Event - Mouse Button - Button %d Action %d Mods %d\n", button, action, mods);
   #endif
}

void gl_window_scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
   #ifdef DEBUG
      fprintf(stdout, "Event - Scroll Wheel - X: %lf Y: %lf\n", x_offset, y_offset);
   #endif
}

void gl_window_cursor_position_callback(GLFWwindow* window, double x_pos, double y_pos)
{
   #ifdef DEBUG
      fprintf(stdout, "Event - Cursor Position - X: %lf Y: %lf\n", x_pos, y_pos);
   #endif
}

void gl_window_error_callback(int error_code, const char* description)
{
   #ifdef DEBUG
      fprintf(stdout, "Error Callback - Code: %d, Description: %s\n", error_code, description);
   #endif
}

/**************************************************
 * 
 *  Internal GL window functions
 * 
 **************************************************/
void window_gl_set_callbacks(GLFWwindow* glfw_window_ptr)
{
   // Window action callback assignment
   glfwSetWindowSizeCallback(glfw_window_ptr,  (GLFWwindowsizefun)&gl_window_size_callback);
   glfwSetFramebufferSizeCallback(glfw_window_ptr, (GLFWframebuffersizefun)&gl_framebuffer_size_callback);
   glfwSetWindowCloseCallback(glfw_window_ptr, (GLFWwindowclosefun)&gl_window_close_callback);
   glfwSetKeyCallback(glfw_window_ptr,         (GLFWkeyfun)&gl_window_key_callback);
   glfwSetCharCallback(glfw_window_ptr,        (GLFWcharfun)&gl_window_char_callback);
   glfwSetMouseButtonCallback(glfw_window_ptr, (GLFWmousebuttonfun)&gl_window_mouse_button_callback);
   glfwSetScrollCallback(glfw_window_ptr,      (GLFWscrollfun)&gl_window_scroll_callback);
   glfwSetCursorPosCallback(glfw_window_ptr,   (GLFWcursorposfun)&gl_window_cursor_position_callback);
}

int window_gl_init(struct window_t* window)
{
   #ifdef DEBUG
      fprintf(stdout, "Initializing OpenGL Window\n");
   #endif

   //Handle GLFW initialization
   if (!is_glfw_initialized)
   {
      is_glfw_initialized = glfwInit();
      if (is_glfw_initialized)
      {
         glfwSetErrorCallback((GLFWerrorfun)gl_window_error_callback);

         //Set some GLFW settings such as GL context version, modern core profile for the context, etc.
         glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
         glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
         glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
         glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
         glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
      }
   }

   // Create window via GLFW
   GLFWwindow* gl_window_ptr = glfwCreateWindow(window->metadata.width, window->metadata.height, window->metadata.tag, NULL, NULL);
   // Assign to our window handle as void*
   window->context_data.window_handle = (void*)gl_window_ptr;

   // Set a pointer in GLFW to our window data
   glfwSetWindowUserPointer(gl_window_ptr, (void*)&(window->metadata));

   window_gl_set_callbacks(gl_window_ptr);

   glfwMakeContextCurrent(gl_window_ptr);

   if (!is_glew_initialized)
   {
      glewExperimental = GL_TRUE;
      is_glew_initialized = (glewInit() == GLEW_OK);
   }

   return !(is_glfw_initialized && is_glew_initialized);
}

void window_gl_run(struct window_t* window)
{
   GLFWwindow* gl_window_ptr;
   gl_window_ptr = (GLFWwindow*)window->context_data.window_handle;

   glfwPollEvents();
   glfwSwapBuffers(gl_window_ptr);
}

void window_gl_cleanup(struct window_t* window)
{
   #ifdef DEBUG
      fprintf(stdout, "Cleaning up OpenGL Window\n");
   #endif

   if (is_glfw_initialized)
   {
      GLFWwindow* gl_window_ptr = (GLFWwindow*)window->context_data.window_handle;

      glfwDestroyWindow(gl_window_ptr);
      glfwTerminate();

      is_glfw_initialized = 0;
   }

   if (is_glew_initialized)
   {
      is_glew_initialized = 0;
   }
}

/**************************************************
 * 
 *  Extern window function definitions
 * 
 **************************************************/
int window_graphics_init(struct window_t* window)
{
   #ifdef DEBUG
      fprintf(stdout, "Found OpenGL Backend\n");
   #endif

   return window_gl_init(window);
}

void window_graphics_run(struct window_t* window)
{
   window_gl_run(window);
}

void window_graphics_cleanup(struct window_t* window)
{
   window_gl_cleanup(window);
}

void window_graphics_set_background_color(struct window_t* window, const fvector4 color)
{
   render_api_set_clear_color(color);
}