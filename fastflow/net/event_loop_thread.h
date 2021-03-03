//
// Created by itsuy on 2021/3/3.
//

#ifndef FASTFLOW_EVENT_LOOP_THREAD_H
#define FASTFLOW_EVENT_LOOP_THREAD_H

#include <condition_variable>
#include <mutex>
#include <thread>

#include "fastflow/base/noncopyable.h"

class event_loop;

class event_loop_thread : fastflow::noncopyable {
public:
    event_loop_thread();

    ~event_loop_thread();

    event_loop *start_loop();

private:
    void thread_func();

    event_loop *loop_;

    std::mutex mutex_;
    std::condition_variable cond_;
    std::thread thread_;
};

#endif //FASTFLOW_EVENT_LOOP_THREAD_H
