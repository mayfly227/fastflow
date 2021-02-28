//
// Created by itsuy on 2021/2/18.
//

#ifndef FASTFLOW_CHANNEL_H
#define FASTFLOW_CHANNEL_H

#include <functional>

#include "fastflow/base/noncopyable.h"
#include "fastflow/net/eventloop.h"

//负责处理fd,但是不拥有
//把不同的IO事件分发给不同的回调
//所有成员函数只能在IO线程调用,这里不需要加锁
class channel : fastflow::noncopyable {
public:
    typedef std::function<void()> event_cb;

    channel(event_loop *loop, int fd);

    // 核心处理事件,根据revents的值调用不同的回调
    void handle_event();

    void set_read_cb(const event_cb &cb) {
        read_cb_ = cb;
    }

    void set_write_cb(const event_cb &cb) {
        write_cb_ = cb;
    }

    void set_error_cb(const event_cb &cb) {
        error_cb_ = cb;
    }

    int get_fd() const {
        return fd_;
    }

    int get_events() const {
        return events_;
    }

    void set_revents(int revents) {
        revents_ = revents;
    }

    //for poller
    int get_index() const {
        return index_;
    }

    void set_index(int index) {
        index_ = index;
    }

    // 控制IO行为
    void enable_reading() {
        events_ |= k_read_event;
        update();
    };

    void enable_writing() {
        events_ |= k_write_event;
        update();
    };

    void disable_writing();

    void disable_all() {
        events_ |= k_none_event;
    };

    bool is_set_none_event() const {
        return k_none_event == events_;
    };

    //返回所属的loop
    event_loop *owner_loop() const {
        return loop_;
    };

private:
    // 更新channel
    void update();

    //封装事件
    static const int k_none_event;
    static const int k_read_event;
    static const int k_write_event;
    event_loop *loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_; // used by poller

    event_cb read_cb_;
    event_cb write_cb_;
    event_cb error_cb_;
};

#endif //FASTFLOW_CHANNEL_H
