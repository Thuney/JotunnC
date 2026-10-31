cmake_minimum_required(VERSION 4.2.3)
project (Renderer2DApp)

add_executable(
  Renderer2DApp
  ${Renderer2DApp_SOURCE_DIR}/main.c
  ${Renderer2DApp_SOURCE_DIR}/renderer_2d_app.c
  ${Renderer2DApp_SOURCE_DIR}/renderer_2d_app_resources.c
  ${Renderer2DApp_SOURCE_DIR}/renderer_2d_app_scene.c)

target_compile_definitions(
  Renderer2DApp
  PRIVATE
  EXAMPLE_RES_PATH="${JOTUNN_EXAMPLE_RES_PATH}")

target_compile_options(
  Renderer2DApp
  PRIVATE
  -Wall
  -Wextra)

target_link_libraries(
  Renderer2DApp
  m
  JotunnC)
