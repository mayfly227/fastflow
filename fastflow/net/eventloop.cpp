//
// Created by itsuy on 2021/2/17.
//

#include <cassert>

#include "fastflow/net/channel.h"
#include "fastflow/net/eventloop.h"
#include "fastflow/net/poller.h"
#include "fastflow/net/syscall.h"
#include "fastflow/net/timer_queue.h"

#ifdef WIN32

#include <WinSock2.h>

#pragma comment (lib, "ws2_32.lib")
#elif __linux__

//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <unistd.h>
#endif

//检查当前线程是否已经有event_loop
thread_local event_loop *t_loop_in_this_thread = nullptr;


event_loop::event_loop() :
        looping_(false),
        quit_(false),
        calling_pending_func_(false),
        current_id_(std::this_thread::get_id()),
        poller_(std::make_unique<poller>(this)),
        timer_queue_(std::make_unique<timer_queue>(this)),
        wakeup_fd_(create_eventfd()),
#ifdef WIN32
        wakeup_channel_(std::make_unique<channel>(this, wakeup_fd_.pipe_read))
#elif __linux__
wakeup_channel_(std::make_unique<channel>(this, wakeup_fd_))
#endif
{
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    fastflow::logging::trace("loop [%p] create in thread [%ld]", this, current_id_);
    if (t_loop_in_this_thread) {
        fastflow::logging::error("Another EventLoop in this thread [%ld]", std::this_thread::get_id());
    } else {
        t_loop_in_this_thread = this;
    }
    wakeup_channel_->set_read_cb([this] { handle_read(); });
    wakeup_channel_->enable_reading();
}

event_loop::~event_loop() {
    // 确保loop 运行
    assert(!looping_);
    t_loop_in_this_thread = nullptr;
#ifdef WIN32
    ::WSACleanup();
#endif
}

event_loop *event_loop::get_current_thread_loop() {
    return t_loop_in_this_thread;
}

void event_loop::abort_not_in_thread() {
    fastflow::logging::error("current loop id[%ld],current thread loop id[%ld]", current_id_,
                             std::this_thread::get_id());
}

void event_loop::loop() {
    assert(!looping_);
    assert_in_loop_thread();
    looping_ = true;
    while (!quit_) {
        activate_channels_.clear();
        poller_->poll_channels(-1, &activate_channels_);
        for (auto &c:activate_channels_) {
            c->handle_event();
        }
        // 运行用户回调
        do_pending_funcs();
    }
    fastflow::logging::trace("loop [%p] in thread [%ld] stop looping", this, std::this_thread::get_id());
    looping_ = false;
}

void event_loop::update_channel(channel *c) {
    //必需在IO线程执行
    assert(c->owner_loop() == this);
    assert_in_loop_thread();
    poller_->update_channel(c);
}

void event_loop::quit() {
    quit_ = true;
    if (!is_in_loop_thread()) {
        wakeup();
    }
}

timer_id event_loop::call_at(const fastflow::timestamp &when, const timer_cb &cb) {
    return timer_queue_->add_timer(cb, when, -1);
}

timer_id event_loop::call_later(double delay, const timer_cb &cb) {
    auto now = timestamp::now_system();
    timestamp when{now + std::chrono::milliseconds{(int) (delay * 1000)}};
    return call_at(when, cb);
}

timer_id event_loop::call_every(double interval, const timer_cb &cb) {
    auto now = timestamp::now_system();
    timestamp when{now + std::chrono::milliseconds{(int) (interval * 1000)}};
    return timer_queue_->add_timer(cb, when, interval);
}

void event_loop::run_in_loop(const func &cb) {
    // 如果在当前IO线程调用本函数，那么回调会同步进行
    // 如果在其它线程调用调用，那么回调会加入IO线程的任务队列,IO线程会被唤醒来调用回调
    if (is_in_loop_thread()) {
        cb();
    } else {
        queue_in_loop(cb);
    }
}

void event_loop::queue_in_loop(const event_loop::func &cb) {
    // 将cb放入未决队列,并在必要时唤醒loop
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_funcs_.emplace_back(cb);
    }
    // 其它线程调用
    if (!is_in_loop_thread() || calling_pending_func_) {
        wakeup();
    }
}

void event_loop::do_pending_funcs() {
    std::vector<func> funcs;
    calling_pending_func_ = true;
    //减少临界区长度,因为func有可能再次调用queue_in_loop
    {
        std::lock_guard<std::mutex> lock(mutex_);
        funcs.swap(pending_funcs_);
    }
    fastflow::logging::trace("do_pending_funcs now len(funcs)=[%d]", funcs.size());
    for (const auto &f:funcs) {
        f();
    }
    calling_pending_func_ = false;
}

void event_loop::handle_read() const {
#ifdef WIN32
    char buffer[1]{'0'};
    auto n = ::recv(wakeup_fd_.pipe_read, buffer, sizeof(buffer), 0);
    if (n != sizeof(buffer)) {
        fastflow::logging::error("EventLoop::wakeup() reads [%d] bytes instead of 8");
    }
#elif __linux__
    uint64_t one = 1;
    auto n = ::read(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        fastflow::logging::error("EventLoop::wakeup() reads [%d] bytes instead of 8");
    }
#endif
}

void event_loop::wakeup() const {
    fastflow::logging::trace("wake up");
#ifdef WIN32
    char buffer[1]{'0'};
    auto n = ::send(wakeup_fd_.pipe_write, buffer, sizeof(buffer), 0);
    if (n != sizeof(buffer)) {
        fastflow::logging::error("EventLoop::wakeup() write [%d] bytes instead of 1");
    }
#elif __linux__
    uint64_t one = 1;
    auto n = ::write(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        fastflow::logging::error("EventLoop::wakeup() writes [%d] bytes instead of 8");
    }
#endif
}
