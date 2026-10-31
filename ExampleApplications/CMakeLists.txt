cmake_minimum_required(VERSION 4.2.3)
project (JotunnExampleApplications)

# Small, self-contained applications, each showing one part of Jotunn. Each
#  lives wholly in its own directory, laid out the same way: main.c runs the
#  application, <app>.c sets it up, and the rest hold what it draws and does,
#  with main.c's opening comment giving the order to read them in. Any of
#  them can be copied out as the start of a new application

# Where the examples' shared resources live. Each example is given it as
#  EXAMPLE_RES_PATH, a string literal, so that resources load wherever the
#  example is run from
set(
  JOTUNN_EXAMPLE_RES_PATH
  ${JotunnExampleApplications_SOURCE_DIR}/res)

add_subdirectory(${JotunnExampleApplications_SOURCE_DIR}/ImGuiApp)
add_subdirectory(${JotunnExampleApplications_SOURCE_DIR}/Renderer2DApp)
