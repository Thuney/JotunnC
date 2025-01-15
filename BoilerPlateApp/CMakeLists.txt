cmake_minimum_required(VERSION 3.20)
project (BoilerPlateApp)

file(GLOB_RECURSE SRCS *.c)
file(GLOB_RECURSE HDRS *.h)

include_directories(${BoilerPlateApp_SOURCE_DIR})

add_executable(BoilerPlateApp ${SRCS} ${HDRS})

target_link_libraries(BoilerPlateApp JotunnC)