//
// Created by itsuy on 2021/2/20.
//

#ifndef FASTFLOW_TIMER_H
#define FASTFLOW_TIMER_H

#include <atomic>
#include <utility>

#include "fastflow/base/noncopyable.h"
#include "fastflow/base/timestamp.h"
#include "fastflow/net/callbacks.h"

using fastflow::timestamp;

class timer : public fastflow::noncopyable {
public:
    timer(timer_cb cb, timestamp when, double interval)
            : cb_(std::move(cb)),
              expiration_(when),
              interval_(interval),
              repeat_(interval > 0),
              id_(++id_number_) {

    };

//    ~timer(){
//        printf("time dtor...\n");
//    }
    void restart(timestamp now) {
        if (repeat_) {
            expiration_ = now.getTimePointSystem() + std::chrono::milliseconds{(int) (interval_ * 1000)};;
        } else {
            expiration_ = now;
        }
    };

    // 运行回调
    void run() {
        cb_();
    }

    int getIdNumber() const {
        return id_number_.load();
    }

    timestamp getExpiration() const {
        return expiration_;
    }

    bool getRepeat() const {
        return repeat_;
    }

    int getId() const {
        return id_;
    }

private:
    // 过期时间
    timestamp expiration_;
    // 回调
    const timer_cb cb_;
    // 间隔
    const double interval_;
    // 是否重复运行
    const bool repeat_;
    int id_;

    // id号
    std::atomic<int> id_number_ = 0;

};


#endif //FASTFLOW_TIMER_H
