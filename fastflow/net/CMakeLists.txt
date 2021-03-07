cmake_minimum_required(VERSION 3.16)
project(FastFlow_Net)

set(CMAKE_CXX_STANDARD 17)

set(net_src
        callbacks.h
        channel.cpp channel.h
        eventloop.cpp eventloop.h
        event_loop_thread.cpp event_loop_thread.h
        poller.cpp poller.h
        syscall.cpp syscall.h
        timer_queue.cpp timer_queue.h
        timer.h
        timer_id.h
        )

add_library(fastflow_net ${net_src})
target_link_libraries(fastflow_net fastflow_base)

install(TARGETS fastflow_net DESTINATION lib)