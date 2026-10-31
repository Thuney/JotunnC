cmake_minimum_required(VERSION 4.2.3)
project (ImGuiApp)

add_executable(
  ImGuiApp
  ${ImGuiApp_SOURCE_DIR}/main.c
  ${ImGuiApp_SOURCE_DIR}/imgui_app.c
  ${ImGuiApp_SOURCE_DIR}/imgui_app_ui.c)

target_compile_definitions(
  ImGuiApp
  PRIVATE
  EXAMPLE_RES_PATH="${JOTUNN_EXAMPLE_RES_PATH}")

target_compile_options(
  ImGuiApp
  PRIVATE
  -Wall
  -Wextra)

target_link_libraries(
  ImGuiApp
  m
  JotunnC)
