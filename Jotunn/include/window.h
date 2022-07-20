#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct window_data_t
{
    char* tag;

    int width;
    int height;
};

struct window_t
{
    struct window_data_t metadata;
    GLFWwindow* glfw_window;
};

int window_init(struct window_t* window, int width, int height, char* tag);
int window_cleanup(struct window_t* window);