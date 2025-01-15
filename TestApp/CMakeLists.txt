cmake_minimum_required(VERSION 3.20)
project (TestApp)

file(GLOB_RECURSE SRCS *.c)
file(GLOB_RECURSE HDRS *.h)

include_directories(${TestApp_SOURCE_DIR})

add_executable(TestApp ${SRCS} ${HDRS})

target_link_libraries(TestApp JotunnC)