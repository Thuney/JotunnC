cmake_minimum_required(VERSION 4.2.3)
cmake_policy(VERSION 4.2.3)
project (JotunnCProjectRoot)

add_subdirectory(${PROJECT_SOURCE_DIR}/Jotunn ${PROJECT_SOURCE_DIR}/Jotunn/.build)
add_subdirectory(${PROJECT_SOURCE_DIR}/TestApp ${PROJECT_SOURCE_DIR}/TestApp/.build/apps/)
add_subdirectory(${PROJECT_SOURCE_DIR}/BoilerPlateApp ${PROJECT_SOURCE_DIR}/BoilerPlateApp/.build/apps/)
