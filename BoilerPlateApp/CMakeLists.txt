cmake_minimum_required(VERSION 4.2.3)
project (BoilerPlateApp)

include_directories(${BoilerPlateApp_SOURCE_DIR})
set(
  JOTUNN_APPLICATION_SOURCES

  ${BoilerPlateApp_SOURCE_DIR}/main.c
  ${BoilerPlateApp_SOURCE_DIR}/ball_ui.c
  ${BoilerPlateApp_SOURCE_DIR}/ball_window.c
)

add_executable(BoilerPlateApp ${JOTUNN_APPLICATION_SOURCES})

target_link_libraries(BoilerPlateApp m)

target_link_libraries(BoilerPlateApp JotunnC)
