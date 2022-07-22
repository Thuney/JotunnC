#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include "window.h"

#include "shader.h"
#include "vertexarray.h"
#include "vertexattribute.h"
#include "vertexbuffer.h"

/**************************************************
 * 
 *  Variables
 * 
 **************************************************/

const GLchar* vertex_shader_src =
   "#version 150"										//Defines the GLSL version of this shader to be 1.50
   "\n"
   "in vec2 position;"									//Defines an input to the shader which is a 2-dimensional vector
   "\n"
   "void main()"
   "{"
   "	gl_Position = vec4(position, 0.0, 1.0);"		//Set the homogenous coordinates of the vertex given our 2D vector input
   "};";

const GLchar* fragment_shader_src =
   "#version 150"										//Defines the GLSL version of this shader to be 1.50
   "\n"
   "out vec4 outColor;"								//Defines an output to the shader which is a 4-dimensional vector
   "\n"
   "void main()"
   "{"
   "	outColor = vec4(1.0, 1.0, 1.0, 1.0);"			//Set the value of the (in this case constant and white) color output
   "}";

//Hardcoded array of our triangle vertices in (X, Y) pairs
//Note that these values are in the range [-1.0, 1.0] to fit in
//OpenGL's unprojected coordinate system
float triangle_vertices[] =
{
   -0.5f, 0.5f,  //Vertex 1 (X, Y), top point
   0.0f, -0.5f,  //Vertex 2 (X, Y), bottom right point
   -1.0f, -0.5f, //Vertex 3 (X, Y), bottom left point
};

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
static struct shader_program_t shader_program;
static struct vertex_attribute_t pos_attribute;
static struct vertex_array_t vao;
static struct vertex_buffer_t vbo;

void window_gl_do_stuff_init(struct window_t* window)
{
   struct vertex_shader_t vertex_shader;
   struct fragment_shader_t fragment_shader;

   vertex_shader_init(&vertex_shader, vertex_shader_src);
   fragment_shader_init(&fragment_shader, fragment_shader_src);

   shader_program_init(&shader_program, &vertex_shader, &fragment_shader);
   shader_program_bind_fragment_data_location(&shader_program, 0, "outColor");
   shader_program_link(&shader_program);

   vertex_shader_destroy(&vertex_shader);
   fragment_shader_destroy(&fragment_shader);

   vertex_array_init(&vao, 1);
   vertex_buffer_init(&vbo, 1);

   vertex_array_bind(&vao);
   vertex_buffer_bind(&vbo);

   vertex_buffer_buffer_data(&vbo, triangle_vertices, sizeof(triangle_vertices), STATIC_DRAW);

   int triangle_pos_attrib_index = shader_program_get_attribute_location(&shader_program, "position");

   vertex_attribute_init(&pos_attribute, triangle_pos_attrib_index, 2, V_FLOAT, GL_FALSE, 0, 0);
   vertex_array_set_attribute(&vao, &pos_attribute);

	// //Specify how to interpret the vertex data for our position attribute
	// glVertexAttribPointer(triangle_pos_attrib_index, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// glEnableVertexAttribArray(triangle_pos_attrib_index);

	glBindVertexArray(0);
}

void window_gl_do_stuff_run(struct window_t* window)
{
   vertex_array_bind(&vao);
   shader_program_use(&shader_program);
   glDrawArrays(GL_TRIANGLES, 0, 3);
}

void window_gl_do_stuff_cleanup(struct window_t* window)
{
   vertex_array_destroy(&vao, 1);
   vertex_buffer_destroy(&vbo, 1);
   shader_program_destroy(&shader_program);
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