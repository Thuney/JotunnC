cmake_minimum_required(VERSION 4.2.3)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

project (JotunnC LANGUAGES C CXX)

set(GRAPHICS_BACKEND "OPENGL")

set(
  CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${JotunnC_SOURCE_DIR}/.build/lib/)

find_package(Freetype REQUIRED)

# Dear ImGui and its extensions don't depend on a graphics API; Jotunn feeds
#  them input from its own events and draws them through renderer_2d
add_subdirectory(${JotunnC_SOURCE_DIR}/vendor/cimgui)
add_subdirectory(${JotunnC_SOURCE_DIR}/vendor/cimplot)

# cimnodes ships without a CMakeLists.txt, so its target is defined here,
#  building imnodes against cimgui's Dear ImGui rather than a copy of its own
add_library(
  cimnodes
  SHARED
  ${JotunnC_SOURCE_DIR}/vendor/cimnodes/cimnodes.cpp
  ${JotunnC_SOURCE_DIR}/vendor/cimnodes/imnodes/imnodes.cpp)

target_compile_definitions(
  cimnodes
  PRIVATE
  # The namespace cimnodes' generated wrappers call into
  IMNODES_NAMESPACE=imnodes
  # Matches the configuration cimgui compiles imgui with
  IMGUI_USER_CONFIG="../cimconfig.h")

target_include_directories(
  cimnodes
  PUBLIC
  ${JotunnC_SOURCE_DIR}/vendor/cimnodes
  PRIVATE
  ${JotunnC_SOURCE_DIR}/vendor/cimnodes/imnodes)

target_link_libraries(
  cimnodes
  PUBLIC
  cimgui)

set_target_properties(
  cimnodes
  PROPERTIES
  PREFIX "")

if (GRAPHICS_BACKEND STREQUAL "OPENGL")
  find_package(glfw3 REQUIRED)
  find_package(GLEW REQUIRED)
else()

endif()

#
set(APP_SRCS
  ${JotunnC_SOURCE_DIR}/src/core/application/application.c
  ${JotunnC_SOURCE_DIR}/src/core/application/timestep.c
  ${JotunnC_SOURCE_DIR}/src/core/application/window.c
  ${JotunnC_SOURCE_DIR}/src/core/application/window_layer.c
  # ${JotunnC_SOURCE_DIR}/src/core/application/ui_element.c
  ${JotunnC_SOURCE_DIR}/src/core/application/ui_layer.c
  ${JotunnC_SOURCE_DIR}/src/core/application/ui/ui_layer_imgui.c

  ${JotunnC_SOURCE_DIR}/src/core/utils/allocator.c
  ${JotunnC_SOURCE_DIR}/src/core/utils/log.c
  ${JotunnC_SOURCE_DIR}/src/core/utils/util.c

  ${JotunnC_SOURCE_DIR}/src/core/graphics/api/render_api.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/renderer.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/renderer_2d.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/renderer_window_layer.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/renderable_2d_primitive.c
  ${JotunnC_SOURCE_DIR}/src/core/graphics/resource.c
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
  ${JotunnC_SOURCE_DIR}/src/math/fquaternion.c
  ${JotunnC_SOURCE_DIR}/src/math/u8vector.c)

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

# The diagnostics overlay, toggled with F3 in each window: frame timing, heap
#  memory and graphics resources. Switched off, its sources aren't built and
#  its hooks compile to nothing
option(
  JOTUNN_ENABLE_DIAGNOSTICS
  "Build Jotunn's diagnostics overlay"
  ON)

if (JOTUNN_ENABLE_DIAGNOSTICS)
  set(
    DIAGNOSTICS_SRCS
    ${JotunnC_SOURCE_DIR}/src/core/diagnostics/diagnostics.c
    ${JotunnC_SOURCE_DIR}/src/core/diagnostics/diagnostics_overlay.c
    ${JotunnC_SOURCE_DIR}/src/core/diagnostics/diagnostics_panel.c)
else()
  set(
    DIAGNOSTICS_SRCS)
endif()

add_library(
  JotunnC
  SHARED
  ${APP_SRCS}
  ${PLATFORM_SRCS}
  ${DIAGNOSTICS_SRCS})

if (JOTUNN_ENABLE_DIAGNOSTICS)
  # Public, as it decides what diagnostics.h declares
  target_compile_definitions(
    JotunnC
    PUBLIC
    JOTUNN_DIAGNOSTICS)
endif()

# Whether Jotunn's allocator may use the C library's heap. Switched off, it
#  never does: every allocator tag starts out forbidden, and the application
#  must give each tag it needs a pool of memory it provides
option(
  JOTUNN_ENABLE_HEAP
  "Let Jotunn's allocator use the C library's heap"
  ON)

if (NOT JOTUNN_ENABLE_HEAP)
  # Public, so the application can tell which kind of build it's linked to
  target_compile_definitions(
    JotunnC
    PUBLIC
    JOTUNN_NO_HEAP)
endif()

# How many of each kind of primitive a renderer_2d batch holds. It sizes the
#  public struct renderer_2d_storage_t, so it's a public definition, which
#  keeps Jotunn and the applications built against it agreeing on it
set(
  JOTUNN_RENDERER_2D_MAX_PRIMITIVES_PER_BATCH
  1000
  CACHE STRING
  "How many of each kind of primitive a renderer_2d batch holds")

target_compile_definitions(
  JotunnC
  PUBLIC
  RENDERER_2D_MAX_PRIMITIVES_PER_BATCH=${JOTUNN_RENDERER_2D_MAX_PRIMITIVES_PER_BATCH})

target_compile_definitions(
  JotunnC
  #
  PUBLIC
  #
  JOTUNN_ROOT_PATH=${JotunnC_SOURCE_DIR}

  DEBUG

  CIMGUI_DEFINE_ENUMS_AND_STRUCTS
)

target_include_directories(
  JotunnC
  PUBLIC
  ${JotunnC_SOURCE_DIR}/include/
  ${cimplot_SOURCE_DIR}/)

target_link_libraries(
  JotunnC 
  PUBLIC 
  cimgui 
  cimplot
  cimnodes)

target_include_directories(
  JotunnC 
  PUBLIC 
  cimgui 
  cimplot
  cimnodes)

target_link_libraries(
  JotunnC 
  PUBLIC 
  ${FREETYPE_LIBRARIES})
include_directories(
  JotunnC 
  PUBLIC 
  ${FREETYPE_INCLUDE_DIRS})
target_include_directories(
  JotunnC 
  PUBLIC 
  ${FREETYPE_INCLUDE_DIRS})

#
if (GRAPHICS_BACKEND STREQUAL "OPENGL")
  target_link_libraries(
    JotunnC 
    PUBLIC 
    glfw)
  target_link_libraries(
    JotunnC 
    PUBLIC 
    GLEW::GLEW)
else()

endif()
