cmake_minimum_required(VERSION 3.20)
cmake_policy(VERSION 3.20)

project (JotunnCRoot)

# Build Jotunn
add_subdirectory(${PROJECT_SOURCE_DIR}/Jotunn ${PROJECT_SOURCE_DIR}/Jotunn/.build)

# Build client applications
add_subdirectory(${PROJECT_SOURCE_DIR}/BoilerPlateApp ${PROJECT_SOURCE_DIR}/BoilerPlateApp/.build)
add_subdirectory(${PROJECT_SOURCE_DIR}/TestApp ${PROJECT_SOURCE_DIR}/TestApp/.build)