#include "window.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

const GLchar* vertex_shader =
    "#version 150"										//Defines the GLSL version of this shader to be 1.50
    "\n"
    "in vec2 position;"									//Defines an input to the shader which is a 2-dimensional vector
    "\n"
    "void main()"
    "{"
    "	gl_Position = vec4(position, 0.0, 1.0);"		//Set the homogenous coordinates of the vertex given our 2D vector input
    "};";

const GLchar* fragment_shader =
    "#version 150"										//Defines the GLSL version of this shader to be 1.50
    "\n"
    "out vec4 outColor;"								//Defines an output to the shader which is a 4-dimensional vector
    "\n"
    "void main()"
    "{"
    "	outColor = vec4(1.0, 1.0, 1.0, 1.0);"			//Set the value of the (in this case constant and white) color output
    "}";

static int is_glfw_initialized = 0;
static int is_glew_initialized = 0;

// Window Callbacks

void window_size_callback(GLFWwindow* window, int width, int height)
{

}

void window_close_callback(GLFWwindow* window)
{
    
}

void window_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void window_char_callback(GLFWwindow* window, unsigned int codepoint)
{

}

void window_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

}

void window_scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{

}

void window_cursor_position_callback(GLFWwindow* window, double x_pos, double y_pos)
{

}

void window_error_callback(int error_code, const char* description)
{
    
}

// Helpers

void window_set_metadata(struct window_data_t* metadata, int width, int height, char* tag)
{
    // Store our window tag
    int tag_length = strlen(tag);
    metadata->tag = (char*) malloc(tag_length*sizeof(char));    
    strcpy(metadata->tag, tag);

    // Assign window properties
    metadata->width  = width;
    metadata->height = height;
}

int window_gl_init(struct window_t* window)
{
    //Handle GLFW initialization
    if (!is_glfw_initialized)
    {
        is_glfw_initialized = glfwInit();
        glfwSetErrorCallback((GLFWerrorfun)&window_error_callback);
    }

    // Create window via GLFW
    window->glfw_window = glfwCreateWindow(window->metadata.width, window->metadata.height, window->metadata.tag, NULL, NULL);
    // Set a pointer in GLFW to our window data
    glfwSetWindowUserPointer(window->glfw_window, (void*)&(window->metadata));

    // Window action callback assignment
    glfwSetWindowSizeCallback(window->glfw_window,  (GLFWwindowsizefun)&window_size_callback);
    glfwSetWindowCloseCallback(window->glfw_window, (GLFWwindowclosefun)&window_close_callback);
    glfwSetKeyCallback(window->glfw_window,         (GLFWkeyfun)&window_key_callback);
    glfwSetCharCallback(window->glfw_window,        (GLFWcharfun)&window_char_callback);
    glfwSetMouseButtonCallback(window->glfw_window, (GLFWmousebuttonfun)&window_mouse_button_callback);
    glfwSetScrollCallback(window->glfw_window,      (GLFWscrollfun)&window_scroll_callback);
    glfwSetCursorPosCallback(window->glfw_window,   (GLFWcursorposfun)&window_cursor_position_callback);

    // Set the GL viewport to the size of the window
    glViewport(0, 0, window->metadata.width, window->metadata.height);

    glfwMakeContextCurrent(window->glfw_window);

    if(!is_glew_initialized)
    {
        is_glew_initialized = glewInit();
    }

    return !(is_glfw_initialized && is_glew_initialized);
}

void window_gl_cleanup(struct window_t* window)
{
    if(is_glfw_initialized)
    {
        glfwDestroyWindow(window->glfw_window);
        glfwTerminate();

        is_glfw_initialized = 0;
    }

    if(is_glew_initialized)
    {
        is_glew_initialized = 0;
    }
}

// Exposed functions

int window_init(struct window_t* window, int width, int height, char* tag)
{
    window_set_metadata(&window->metadata, width, height, tag);
    window_gl_init(window);

    return 0;
}

int window_cleanup(struct window_t* window)
{
    free(window->metadata.tag);
    window->metadata.tag = 0;

    window_gl_cleanup(window);

    return 0;
}