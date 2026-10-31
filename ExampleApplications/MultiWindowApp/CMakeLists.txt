cmake_minimum_required(VERSION 4.2.3)
project (MultiWindowApp)

add_executable(
  MultiWindowApp
  ${MultiWindowApp_SOURCE_DIR}/main.c
  ${MultiWindowApp_SOURCE_DIR}/multi_window_app.c
  ${MultiWindowApp_SOURCE_DIR}/multi_window_app_scene_window.c
  ${MultiWindowApp_SOURCE_DIR}/multi_window_app_controls_window.c)

target_compile_definitions(
  MultiWindowApp
  PRIVATE
  EXAMPLE_RES_PATH="${JOTUNN_EXAMPLE_RES_PATH}")

target_compile_options(
  MultiWindowApp
  PRIVATE
  -Wall
  -Wextra)

target_link_libraries(
  MultiWindowApp
  m
  JotunnC)
