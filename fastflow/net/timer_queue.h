//
// Created by itsuy on 2021/2/20.
//

#ifndef FASTFLOW_TIMER_QUEUE_H
#define FASTFLOW_TIMER_QUEUE_H

#include "fastflow/base/noncopyable.h"
#include "fastflow/base/timestamp.h"
#include "fastflow/net/callbacks.h"
#include "fastflow/net/channel.h"

#include <set>
#include <tuple>
#include <vector>

// 定时器队列
class channel;

class event_loop;

class timer;

class timer_id;

using fastflow::timestamp;

class timer_queue : fastflow::noncopyable {
public:
    explicit timer_queue(event_loop *loop);

    ~timer_queue();

    // 保证线程安全,会从其它线程调用(内部会使用run_in_loop)
    timer_id add_timer(const timer_cb &cb, timestamp when, double interval);


    void cancel(const timer_id &timer_id_);

    // timer 列表
    typedef std::pair<timestamp, timer_ptr> entry;
    typedef std::set<entry> timer_list;
private:
    //当timer_fd可读
    void handle_read();

    void add_timer_in_loop(const timer_ptr &t);

    //移除所有过期的timer
    std::vector<entry> get_expired(timestamp now);

    void reset(const std::vector<entry> &expired, timestamp now);

    // 添加定时器到队列
    bool insert(const timer_ptr &t);

    const int timer_fd_;
    event_loop *loop_;
    channel timer_fd_channel_;
    // 根据过期时间排序
    timer_list timer_list_;
};


#endif //FASTFLOW_TIMER_QUEUE_H
