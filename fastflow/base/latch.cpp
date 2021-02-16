//
// Created by itsuy on 2021/2/11.
//

#include "latch.h"

fastflow::latch::latch(int i) :
        mutex_(),
        count_(i) {
}

void fastflow::latch::count_down() {
    std::unique_lock<std::mutex> lock(mutex_);
    --count_;
    if (count_ <= 0) {
        cv_.notify_all();
    }
}

void fastflow::latch::wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (count_ > 0) {
        cv_.wait(lock);
    }
}

unsigned int fastflow::latch::get_count() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return count_;
}

