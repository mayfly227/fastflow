//
// Created by itsuy on 2021/3/3.
//

#include "fastflow/net/eventloop.h"
#include "fastflow/net/event_loop_thread.h"

event_loop_thread::event_loop_thread() :
        loop_(nullptr),
        mutex_(),
        thread_(),
        cond_() {

}

event_loop_thread::~event_loop_thread() {
    loop_->quit();
    thread_.join();
}

event_loop *event_loop_thread::start_loop() {
    std::thread t([this] { thread_func(); });
    thread_ = std::move(t);
    // 加锁是为了保证线程运行了
    {
        std::unique_lock<std::mutex> lock_(mutex_);
        while (loop_ == nullptr) {
            cond_.wait(lock_);
        }
    }
    return loop_;
}

void event_loop_thread::thread_func() {
    event_loop loop;
    {
        std::unique_lock<std::mutex> lock_(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }
    loop.loop();
}
