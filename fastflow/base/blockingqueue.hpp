//
// Created by itsuy on 2021/2/13.
//

#ifndef FASTFLOW_BLOCKQUEUE_H
#define FASTFLOW_BLOCKQUEUE_H

#include "fastflow/base/noncopyable.h"

#include <condition_variable>
#include <mutex>
#include <queue>

//blocking queue实现
namespace fastflow {
    template<typename T>
    class blockingqueue : fastflow::noncopyable {
    public:
        blockingqueue()
                : mutex_(),
                  queue_() {

        }

    public:
        void push(const T &value) {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push(value);
            not_empty_.notify_one();
        };

        void push(T &value) {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push(value);
            not_empty_.notify_one();
        };

        void emplace(const T &&value) {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.emplace(value);
            not_empty_.notify_one();
        };

        void emplace(T &&value) {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.emplace(value);
            not_empty_.notify_one();
        };

        T take() {
            std::unique_lock<std::mutex> lock(mutex_);
            while (queue_.empty()) {
                not_empty_.wait(lock);
            }
            T front = queue_.front();
            queue_.pop();
            return front;
        };

        unsigned long get_size() const {
            std::unique_lock<std::mutex> lock(mutex_);
            return queue_.size();
        }

    private:
        mutable std::mutex mutex_;
        std::condition_variable not_empty_;
        std::queue<T> queue_;
    };
}
#endif //FASTFLOW_BLOCKQUEUE_H
