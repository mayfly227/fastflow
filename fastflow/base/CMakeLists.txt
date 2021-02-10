cmake_minimum_required(VERSION 3.16)
project(FastFlow_Base)

set(CMAKE_CXX_STANDARD 17)

set(base_src
        ThreadPool.cpp
        )

add_library(fastflow_base ${base_src})
target_link_libraries(fastflow_base pthread)

install(TARGETS fastflow_base DESTINATION lib)