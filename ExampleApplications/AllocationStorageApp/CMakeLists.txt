cmake_minimum_required(VERSION 4.2.3)
project (AllocationStorageApp)

add_executable(
  AllocationStorageApp
  ${AllocationStorageApp_SOURCE_DIR}/main.c
  ${AllocationStorageApp_SOURCE_DIR}/allocation_storage_app.c
  ${AllocationStorageApp_SOURCE_DIR}/allocation_storage_app_memory.c
  ${AllocationStorageApp_SOURCE_DIR}/allocation_storage_app_blocks.c
  ${AllocationStorageApp_SOURCE_DIR}/allocation_storage_app_scene.c
  ${AllocationStorageApp_SOURCE_DIR}/allocation_storage_app_ui.c)

target_compile_definitions(
  AllocationStorageApp
  PRIVATE
  EXAMPLE_RES_PATH="${JOTUNN_EXAMPLE_RES_PATH}")

target_compile_options(
  AllocationStorageApp
  PRIVATE
  -Wall
  -Wextra)

target_link_libraries(
  AllocationStorageApp
  m
  JotunnC)
