#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include "window.h"

#include "renderable_2d.h"

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

struct vertex_data_2d_t
{
   fvector2 position;
   fvector4 color_rgba;
};

struct rgba_triangle_2d_t
{
   struct vertex_data_2d_t vertices[3];
   unsigned int indices[3];
};

struct rgba_square_2d_t
{
   struct vertex_data_2d_t vertices[4];
   unsigned int indices[6];
};

static struct rgba_triangle_2d_t triangle_data;
static struct rgba_square_2d_t square_data;

/**************************************************
 * 
 *  Internal GL window functions
 * 
 **************************************************/
static struct renderable_2d_t triangle, square;

void window_gl_do_stuff_init(struct window_t* window)
{
   // V0 Pos
   triangle_data.vertices[0].position.comp.x = -0.75f;
   triangle_data.vertices[0].position.comp.y =  0.0f;
   // V0 Color
   triangle_data.vertices[0].color_rgba.comp.w = 1.0f;
   triangle_data.vertices[0].color_rgba.comp.x = 0.0f;
   triangle_data.vertices[0].color_rgba.comp.y = 0.0f;
   triangle_data.vertices[0].color_rgba.comp.z = 1.0f;
   // V1 Pos
   triangle_data.vertices[1].position.comp.x = -0.5f;
   triangle_data.vertices[1].position.comp.y =  0.5f;
   // V1 Color
   triangle_data.vertices[1].color_rgba.comp.w = 0.0f;
   triangle_data.vertices[1].color_rgba.comp.x = 1.0f;
   triangle_data.vertices[1].color_rgba.comp.y = 0.0f;
   triangle_data.vertices[1].color_rgba.comp.z = 1.0f;
   // V2 Pos
   triangle_data.vertices[2].position.comp.x = -0.25f;
   triangle_data.vertices[2].position.comp.y =  0.0f;
   // V2 Color
   triangle_data.vertices[2].color_rgba.comp.w = 0.0f;
   triangle_data.vertices[2].color_rgba.comp.x = 0.0f;
   triangle_data.vertices[2].color_rgba.comp.y = 1.0f;
   triangle_data.vertices[2].color_rgba.comp.z = 1.0f;

   triangle_data.indices[0] = 0;
   triangle_data.indices[1] = 1;
   triangle_data.indices[2] = 2;

   // V0 Pos
   square_data.vertices[0].position.comp.x = 0.25f;
   square_data.vertices[0].position.comp.y = 0.25f;
   // V0 Color
   square_data.vertices[0].color_rgba.comp.w = 1.0f;
   square_data.vertices[0].color_rgba.comp.x = 0.0f;
   square_data.vertices[0].color_rgba.comp.y = 0.0f;
   square_data.vertices[0].color_rgba.comp.z = 1.0f;
   // V1 Pos
   square_data.vertices[1].position.comp.x = 0.75f;
   square_data.vertices[1].position.comp.y = 0.25f;
   // V1 Color
   square_data.vertices[1].color_rgba.comp.w = 0.0f;
   square_data.vertices[1].color_rgba.comp.x = 1.0f;
   square_data.vertices[1].color_rgba.comp.y = 0.0f;
   square_data.vertices[1].color_rgba.comp.z = 1.0f;
   // V2 Pos
   square_data.vertices[2].position.comp.x = 0.75f;
   square_data.vertices[2].position.comp.y = 0.75f;
   // V2 Color
   square_data.vertices[2].color_rgba.comp.w = 0.0f;
   square_data.vertices[2].color_rgba.comp.x = 0.0f;
   square_data.vertices[2].color_rgba.comp.y = 1.0f;
   square_data.vertices[2].color_rgba.comp.z = 1.0f;
   // V3 Pos
   square_data.vertices[3].position.comp.x = 0.25f;
   square_data.vertices[3].position.comp.y = 0.75f;
   // V3 Color
   square_data.vertices[3].color_rgba.comp.w = 0.0f;
   square_data.vertices[3].color_rgba.comp.x = 0.0f;
   square_data.vertices[3].color_rgba.comp.y = 0.0f;
   square_data.vertices[3].color_rgba.comp.z = 1.0f;

   square_data.indices[0] = 0;
   square_data.indices[1] = 1;
   square_data.indices[2] = 2;
   square_data.indices[3] = 0;
   square_data.indices[4] = 3;
   square_data.indices[5] = 2;

   renderable_2d_init(&triangle, (float*)&triangle_data.vertices, 3, (unsigned int*)&triangle_data.indices, 3);
   renderable_2d_init(&square, (float*)&square_data.vertices, 4, (unsigned int*)&square_data.indices, 6);
}

void window_gl_do_stuff_run(struct window_t* window)
{
   renderable_2d_render(&triangle);
   renderable_2d_render(&square);
}

void window_gl_do_stuff_cleanup(struct window_t* window)
{
   renderable_2d_cleanup(&triangle);
   renderable_2d_cleanup(&square);
}

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

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   window_gl_do_stuff_init(window);

   return !(is_glfw_initialized && is_glew_initialized);
}

void window_gl_run(struct window_t* window)
{
   glClear(GL_COLOR_BUFFER_BIT);

   GLFWwindow* gl_window_ptr;
   gl_window_ptr = (GLFWwindow*)window->context_data.window_handle;

   window_gl_do_stuff_run(window);

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
      window_gl_do_stuff_cleanup(window);

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

void window_graphics_set_background_color(struct window_t* window, float red, float green, float blue, float alpha)
{
   glClearColor(red, green, blue, alpha);
}