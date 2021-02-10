cmake_minimum_required(VERSION 3.16)
project(FastFlow)

set(CMAKE_CXX_STANDARD 17)

#include_directories(../fastflow)
include_directories(${PROJECT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(fastflow/base)
add_subdirectory(fastflow/tests)