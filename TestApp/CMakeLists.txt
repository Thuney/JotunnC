cmake_minimum_required(VERSION 4.2.3)
project (TestApp)

include_directories(${TestApp_SOURCE_DIR})
set(
  JOTUNN_APPLICATION_SOURCES

  ${TestApp_SOURCE_DIR}/main.c
)

add_executable(TestApp ${JOTUNN_APPLICATION_SOURCES})

target_link_libraries(TestApp JotunnC)
