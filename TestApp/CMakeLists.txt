cmake_minimum_required(VERSION 4.2.3)
project (TestApp)

include_directories(${TestApp_SOURCE_DIR})
set(
  JOTUNN_APPLICATION_SOURCES

  ${TestApp_SOURCE_DIR}/main.c
  ${TestApp_SOURCE_DIR}/demo_3d_window.c
)

add_executable(TestApp ${JOTUNN_APPLICATION_SOURCES})

target_compile_definitions(
  TestApp
  PRIVATE
  TESTAPP_ROOT_PATH="${TestApp_SOURCE_DIR}")

target_link_libraries(TestApp m)

target_link_libraries(TestApp JotunnC)
