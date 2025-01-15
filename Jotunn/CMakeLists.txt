cmake_minimum_required(VERSION 3.20)
project (JotunnC VERSION 1.0 LANGUAGES C)

set(GRAPHICS_BACKEND "OPENGL")

set(CMAKE_C_STANDARD_REQUIRED ON)

#add_subdirectory(${PROJECT_SOURCE_DIR}/vendor/clay)

find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)

include_directories(${JotunnC_SOURCE_DIR}/vendor/clay)
include_directories(${JotunnC_SOURCE_DIR}/include)

file(GLOB_RECURSE SRCS src/*.c)

add_library(JotunnC SHARED)

target_sources(
  JotunnC
  PUBLIC
  ./src/core/application/application.c
  ./src/core/application/timestep.c
  ./src/core/application/window.c
  ./src/core/application/window_layer.c
  ./src/core/application/ui_element.c
  ./src/core/application/ui_layer.c
  ./src/core/application/ui_layer_clay.c

  ./src/core/utils/util.c

  ./src/core/graphics/api/render_api.c
  ./src/core/graphics/renderer.c
  ./src/core/graphics/renderer_2d.c
  ./src/core/graphics/renderer_window_layer.c
  ./src/core/graphics/renderable_2d_primitive.c
  ./src/core/graphics/camera.c

  ./src/core/graphics/datatypes/font.c
  ./src/core/graphics/datatypes/framebuffer.c
  ./src/core/graphics/datatypes/shader.c
  ./src/core/graphics/datatypes/texture.c
  ./src/core/graphics/datatypes/vertexarray.c
  ./src/core/graphics/datatypes/vertexattribute.c
  ./src/core/graphics/datatypes/vertexbuffer.c

  ./src/math/fvector.c
  ./src/math/fmatrix.c
  ./src/math/fquaternion.c
)

if (${GRAPHICS_BACKEND} STREQUAL "OPENGL")
  target_sources(
    JotunnC
    PRIVATE
    ./src/platform/application/window_gl.c
    ./src/platform/graphics/api/render_api_opengl.c
    ./src/platform/graphics/datatypes/framebuffer_gl.c
    ./src/platform/graphics/datatypes/shader_gl.c
    ./src/platform/graphics/datatypes/texture_gl.c
    ./src/platform/graphics/datatypes/vertexarray_gl.c
    ./src/platform/graphics/datatypes/vertexattribute_gl.c
    ./src/platform/graphics/datatypes/vertexbuffer_gl.c
  )
else()
  target_sources(
    JotunnC
    PRIVATE
    ./src/platform/application/window_no_backend.c
    ./src/platform/graphics/api/render_api_no_backend.c
    ./src/platform/graphics/datatypes/framebuffer_no_backend.c
    ./src/platform/graphics/datatypes/shader_no_backend.c
    ./src/platform/graphics/datatypes/shader_no_backend.c
    ./src/platform/graphics/datatypes/vertexarray_no_backend.c
    ./src/platform/graphics/datatypes/vertexattribute_no_backend.c
    ./src/platform/graphics/datatypes/vertexbuffer_no_backend.c
  )
endif()

target_include_directories(JotunnC PUBLIC ${JotunnC_SOURCE_DIR}/include)
target_include_directories(JotunnC PUBLIC ${JotunnC_SOURCE_DIR}/vendor/clay)

target_link_libraries(JotunnC m)
target_link_libraries(JotunnC glfw)
target_link_libraries(JotunnC GLEW::GLEW)
target_link_libraries(JotunnC Freetype::Freetype)