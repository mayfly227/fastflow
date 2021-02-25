cmake_minimum_required(VERSION 3.16)
project(FastFlow_Base)

set(CMAKE_CXX_STANDARD 17)

set(base_src
        threadpool.cpp threadpool.h
        latch.cpp latch.h
        copyable.h
        noncopyable.h
        blockingqueue.hpp
        logging.cpp logging.h
        timestamp.cpp timestamp.h
        syscall.h syscall.cpp)

IF (WIN32)
    MESSAGE(STATUS "Build with windows")
    add_library(fastflow_base ${base_src})
ELSEIF (APPLE)
    MESSAGE(STATUS "Build with Apple systems.")
ELSEIF (UNIX)
    MESSAGE(STATUS "Build with UNIX-like OS's.")
    add_library(fastflow_base ${base_src})
    target_link_libraries(fastflow_base pthread)
ENDIF ()


install(TARGETS fastflow_base DESTINATION lib)