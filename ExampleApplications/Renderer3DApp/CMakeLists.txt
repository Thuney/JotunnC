cmake_minimum_required(VERSION 4.2.3)
project (Renderer3DApp)

add_executable(
  Renderer3DApp
  ${Renderer3DApp_SOURCE_DIR}/main.c
  ${Renderer3DApp_SOURCE_DIR}/renderer_3d_app.c
  ${Renderer3DApp_SOURCE_DIR}/renderer_3d_app_scene.c)

target_compile_definitions(
  Renderer3DApp
  PRIVATE
  EXAMPLE_RES_PATH="${JOTUNN_EXAMPLE_RES_PATH}")

target_compile_options(
  Renderer3DApp
  PRIVATE
  -Wall
  -Wextra)

target_link_libraries(
  Renderer3DApp
  m
  JotunnC)
