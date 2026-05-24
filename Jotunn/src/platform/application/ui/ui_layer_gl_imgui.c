#include "ui_layer.h"

#include <memory.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cimgui.h"
#include "cimgui_impl.h"

#include "cimplot.h"

struct ui_layer_gl_imgui_data_t
{
  ImGuiContext *ui_context;
  ImPlotContext *ui_plot_context;
  ImGuiIO *ui_io;

  ImGuiViewport* ui_viewport;
};

static void ui_layer_gl_imgui_init(
  struct ui_layer_t* ui_layer);

static void ui_layer_gl_imgui_cleanup(
  struct ui_layer_t* ui_layer);

static void ui_layer_gl_imgui_render_begin(
  struct ui_layer_t* ui_layer);

static void ui_layer_gl_imgui_render_end(
  struct ui_layer_t* ui_layer);

//
void ui_layer_graphics_init(
  struct ui_layer_t* ui_layer)
{
  ui_layer_gl_imgui_init(ui_layer);
}

void ui_layer_graphics_cleanup(
  struct ui_layer_t* ui_layer)
{
  ui_layer_gl_imgui_cleanup(ui_layer);
}

void ui_layer_graphics_render_begin(
  struct ui_layer_t* ui_layer)
{
  ui_layer_gl_imgui_render_begin(ui_layer);
}

void ui_layer_graphics_render_end(
  struct ui_layer_t* ui_layer)
{
  ui_layer_gl_imgui_render_end(ui_layer);
}

//
static void ui_layer_gl_imgui_init(
  struct ui_layer_t* ui_layer)
{
  GLFWwindow* glfw_window_handle =
    (GLFWwindow*)ui_layer->ui_window_layer.parent_window->context_data.window_handle;

  ui_layer->ui_layer_data =
    malloc(sizeof(struct ui_layer_gl_imgui_data_t));

  struct ui_layer_gl_imgui_data_t *imgui_data =
    (struct ui_layer_gl_imgui_data_t*)ui_layer->ui_layer_data;

  imgui_data->ui_context      = igCreateContext(NULL);
  imgui_data->ui_plot_context = ImPlot_CreateContext();
  imgui_data->ui_io           = igGetIO_ContextPtr(imgui_data->ui_context);
  imgui_data->ui_viewport     = igGetMainViewport();

  imgui_data->ui_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  const char* glsl_version = "#version 330 core";
  ImGui_ImplGlfw_InitForOpenGL(glfw_window_handle, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Setup style
  igStyleColorsDark(NULL);
}

static void ui_layer_gl_imgui_cleanup(
  struct ui_layer_t* ui_layer)
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  struct ui_layer_gl_imgui_data_t *imgui_data =
    (struct ui_layer_gl_imgui_data_t*)ui_layer->ui_layer_data;

  ImPlot_DestroyContext(imgui_data->ui_plot_context);
  igDestroyContext(imgui_data->ui_context);

  imgui_data->ui_io = NULL;
}

static void ui_layer_gl_imgui_render_begin(
  struct ui_layer_t* ui_layer)
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  igNewFrame();
}

static void ui_layer_gl_imgui_render_end(
  struct ui_layer_t* ui_layer)
{
  igRender();
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}
