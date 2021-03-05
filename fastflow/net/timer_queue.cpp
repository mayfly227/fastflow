//
// Created by itsuy on 2021/2/20.
//

#include <cassert>
#include <iterator>

#include "fastflow/base/logging.h"
#include "fastflow/net/eventloop.h"
#include "fastflow/net/timer.h"
#include "fastflow/net/timer_id.h"
#include "fastflow/net/timer_queue.h"

std::vector<timer_queue::entry> timer_queue::get_expired(timestamp now) {
    // 从Timer list中移除过期的timer,并通过vector返回
    std::vector<entry> expired;
    // auto sentry = std::make_pair(now, reinterpret_cast<timer *>(UINTPTR_MAX));
    auto sentry = std::make_pair(now, nullptr);
    auto end = timer_list_.lower_bound(sentry);
    // 没有找到(也就是比所有的都大) || 现在时间<找到的时间
    assert(end == timer_list_.end() || now < end->first);
    std::copy(timer_list_.begin(), end, std::back_inserter(expired));

    // 移除所有过期的timer
    for (const auto &e : expired) {
        timer_list_.erase(e);
    }
    return expired;
}

timer_queue::timer_queue(event_loop *loop) :
        loop_(loop),
        timer_fd_(create_timerfd()),
        timer_fd_channel_(loop, timer_fd_),
        timer_list_() {
    // 加入事件循环
    timer_fd_channel_.set_read_cb([this] { handle_read(); });
    timer_fd_channel_.enable_reading();
}

timer_id timer_queue::add_timer(const timer_cb &cb, timestamp when, double interval) {
    // 添加一个定时器
    // 其中when就是过期时间
    auto t = std::make_shared<timer>(cb, when, interval);
    loop_->run_in_loop([this, t] { add_timer_in_loop(t); });
    return timer_id(t);
}

void timer_queue::cancel(const timer_id &timer_id_) {

}

void timer_queue::add_timer_in_loop(const timer_ptr &t) {
    loop_->assert_in_loop_thread();
    bool status = insert(t);
    if (status) {
        reset_timerfd(timer_fd_, t->getExpiration());
        fastflow::logging::trace("add_timer_in_loop expired at:%s",
                                 timestamp::to_fmt_string(t->getExpiration().getTimePointSystem()).c_str());
    }
}

void timer_queue::handle_read() {
    loop_->assert_in_loop_thread();
    timestamp now_time{timestamp::now_system()};
    read_timerfd(timer_fd_, now_time);

    //获取过期的定时器,并运行超时回调
    std::vector<entry> expired = get_expired(now_time);
    for (const auto &it:expired) {
        it.second->run();
    }
    // 重置过期时间
    reset(expired, now_time);
}

void timer_queue::reset(const std::vector<entry> &expired, timestamp now) {
    // 获取下一个超时的时间
    timestamp next_expired;

    // 查询过期的是否是重复执行
    for (const auto &t:expired) {
        if (t.second->getRepeat()) {
            t.second->restart(now);
            // 重新加入定时器队列,这里是在IO线程完成的
            insert(t.second);
        }
    }
    // 还有定时器没有执行
    if (!timer_list_.empty()) {
        next_expired = timer_list_.begin()->second->getExpiration();
        reset_timerfd(timer_fd_, next_expired);
    }
}

bool timer_queue::insert(const timer_ptr &t) {
    bool earliestChanged = false;
    timestamp when = t->getExpiration();
    // fastflow::logging::trace("insert expiration: %s", when.to_string().c_str());
    auto it = timer_list_.begin();
    // 过期时间按从小到大排序
    // 如果新的定时器小于原来最小的,那么就重置定时器
    if (it == timer_list_.end() || when < it->first) {
        earliestChanged = true;
    }
    // std::pair<timer_list::iterator, bool>
    auto result = timer_list_.insert(std::make_pair(when, t));
    assert(result.second);
    return earliestChanged;
}

timer_queue::~timer_queue() {
#ifdef __linux__
    ::close(timer_fd_);
#endif
}
