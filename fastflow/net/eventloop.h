//
// Created by itsuy on 2021/2/17.
//

#ifndef FASTFLOW_EVENTLOOP_H
#define FASTFLOW_EVENTLOOP_H

#include <atomic>
#include <memory>
#include <thread>

#include "fastflow/base/logging.h"
#include "fastflow/base/noncopyable.h"
#include "fastflow/base/timestamp.h"
#include "fastflow/net/callbacks.h"
#include "fastflow/net/syscall.h"
#include "fastflow/net/timer_id.h"

class channel;

class poller;

class timer_queue;

// 核心事件循环
class event_loop : fastflow::noncopyable {
public:

    // 一个线程只能有一个event_loop对象
    event_loop();

    ~event_loop();

    typedef std::function<void()> func;

    void loop();

    void quit();

    // 获取当前线程的loop
    static event_loop *get_current_thread_loop();

    // 确保当前对象在当前线程(不能跨线程调用)
    void assert_in_loop_thread() {
        if (!is_in_loop_thread()) {
            abort_not_in_thread();
        }
    };

    [[nodiscard]] bool is_in_loop_thread() const {
        return current_id_ == std::this_thread::get_id();
    };

    void run_in_loop(const func &cb);

    void queue_in_loop(const func &cb);

    void update_channel(channel *c);

    timer_id call_at(const fastflow::timestamp &when, const timer_cb &cb);

    timer_id call_later(double delay, const timer_cb &cb);

    timer_id call_every(double interval, const timer_cb &cb);

    // 唤醒IO线程
    void wakeup() const;

private:
    void abort_not_in_thread();

    // 唤醒loop
    void handle_read() const;

    void do_pending_funcs();

    typedef std::vector<channel *> channel_list;
    // 当前线程id
    std::thread::id current_id_;
    //运行标置位
    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    std::atomic<bool> calling_pending_func_;

#ifdef WIN32
    wakeup_fd_pipe wakeup_fd_;
#elif __linux__
    int wakeup_fd_;
#endif
    // 处理wakeup_fd_上的readable事件，分发给handle_read
    std::unique_ptr<channel> wakeup_channel_;
    std::mutex mutex_;
    std::vector<func> pending_funcs_;
    std::unique_ptr<poller> poller_;
    channel_list activate_channels_;

    // 定时器队列
    std::unique_ptr<timer_queue> timer_queue_;
};

#endif //FASTFLOW_EVENTLOOP_H
