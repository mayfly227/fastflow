cmake_minimum_required(VERSION 3.16)
project(FastFlow)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
IF (WIN32)
    #    option(__WIN32 "select to win32" ON)
ELSEIF (UNIX)

ENDIF ()
add_subdirectory(fastflow/base)
add_subdirectory(fastflow/net)
add_subdirectory(fastflow/tests)