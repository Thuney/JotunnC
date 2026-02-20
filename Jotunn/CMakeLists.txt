project (JotunnC)
cmake_minimum_required(VERSION 4.2.3)

set(GRAPHICS_BACKEND "OPENGL")

set(
  CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${JotunnC_SOURCE_DIR}/.build/lib/)

find_package(Freetype REQUIRED)

#
set(APP_SRCS
  ${JotunnC_SOURCE_DIR}/src/core/application/application.c
  ${JotunnC_SOURCE_DIR}/src/core/application/timestep.c
  ${JotunnC_SOURCE_DIR}/src/core/application/window.c
  ${JotunnC_SOURCE_DIR}/src/core/application/window_layer.c
  ${JotunnC_SOURCE_DIR}/src/core/application/ui_element.c
  ${JotunnC_SOURCE_DIR}/src/core/application/ui_layer.c

  ${JotunnC_SOURCE_DIR}/src/core/utils/util.c

  ${JotunnC_SOURCE_DIR}/src/core/graphics/api/render_api.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/renderer.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/renderer_2d.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/renderer_window_layer.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/renderable_2d_primitive.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/camera.c

  ${JotunnC_SOURCE_DIR}/src/core/graphics/datatypes/font.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/datatypes/framebuffer.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/datatypes/shader.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/datatypes/texture.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/datatypes/vertexarray.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/datatypes/vertexattribute.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/datatypes/vertexbuffer.c

  ${JotunnC_SOURCE_DIR}/src/math/fvector.c
  ${JotunnC_SOURCE_DIR}/src/math/fmatrix.c
  ${JotunnC_SOURCE_DIR}/src/math/fquaternion.c)

if (GRAPHICS_BACKEND STREQUAL "OPENGL")
  message("Graphics backend is OpenGL")

  set(
    PLATFORM_SRCS
    ${JotunnC_SOURCE_DIR}/src/platform/application/window_gl.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/api/render_api_opengl.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/datatypes/framebuffer_gl.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/datatypes/shader_gl.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/datatypes/texture_gl.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/datatypes/vertexarray_gl.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/datatypes/vertexattribute_gl.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/datatypes/vertexbuffer_gl.c
  )
else()
  message("Unknown graphics backend; using no backend implementation")

  set(
    PLATFORM_SRCS
    ${JotunnC_SOURCE_DIR}/src/platform/application/window_no_backend.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/api/render_api_no_backend.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/datatypes/framebuffer_no_backend.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/datatypes/shader_no_backend.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/datatypes/texture_no_backend.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/datatypes/vertexarray_no_backend.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/datatypes/vertexattribute_no_backend.c
    ${JotunnC_SOURCE_DIR}/src/platform/graphics/datatypes/vertexbuffer_no_backend.c
  )
endif()

add_library(
  JotunnC
  SHARED
  ${APP_SRCS}
  ${PLATFORM_SRCS})

target_compile_definitions(
  JotunnC
  PUBLIC
  JOTUNN_ROOT_PATH=${JotunnC_SOURCE_DIR}
  DEBUG
)

target_include_directories(
  JotunnC
  PUBLIC
  ${JotunnC_SOURCE_DIR}/include/)

target_link_libraries(JotunnC PUBLIC ${FREETYPE_LIBRARIES})
include_directories(JotunnC PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_include_directories(JotunnC PUBLIC ${FREETYPE_INCLUDE_DIRS})

#
if (GRAPHICS_BACKEND STREQUAL "OPENGL")
  find_package(glfw3 REQUIRED)
  find_package(GLEW REQUIRED)

  target_link_libraries(JotunnC PRIVATE glfw)
  target_link_libraries(JotunnC PRIVATE GLEW::GLEW)
else()

endif()
