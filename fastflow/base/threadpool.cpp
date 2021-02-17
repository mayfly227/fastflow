//
// Created by itsuy on 2021/2/8.
//

#include <exception>
#include <iostream>
#include "threadpool.h"


fastflow::threadpool::threadpool(std::string name)
        : mutex_(),
          poolName_(std::move(name)),
          isRunning_(false) {
}

void fastflow::threadpool::start(int numThreads) {
    isRunning_ = true;
    threads_.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i) {
        threads_.emplace_back(
                std::make_unique<std::thread>(std::thread([this] { runInThread(); }))
        );
    }
}

void fastflow::threadpool::run(const fastflow::threadpool::Task &task) {
    if (threads_.empty()) {
        task();
    } else {
        std::unique_lock<std::mutex> uniqueLock(mutex_);
        if (!isRunning_) {
            return;
        }
        tasks.emplace(task);
        task_count++;
        // 唤醒某个线程
        notEmpty_.notify_one();
    }
}

void fastflow::threadpool::stop(bool wait_all_task_complete) {
    if (wait_all_task_complete) {
        {
            std::unique_lock<std::mutex> uniqueLock(mutex_);
            // 标志位为false,runInThread会跳出while，线程结束
            //如果不唤醒所有的线程,那么子线程就会卡在wait
            notEmpty_.notify_all();
            while (task_count > 0) {
                shutdown.wait(uniqueLock);
                isRunning_ = false;
                notEmpty_.notify_all();
            }
        }
    } else {
        {
            std::unique_lock<std::mutex> uniqueLock(mutex_);
            // 标志位为false,runInThread会跳出while，线程结束
            //如果不唤醒所有的线程,那么子线程就会卡在wait
            notEmpty_.notify_all();
            isRunning_ = false;
        }
    }
    for (auto &t:threads_) {
        // 等待子线程退出
        t->join();
    }
}

void fastflow::threadpool::runInThread() {
    try {
        while (isRunning_) {
            // 如果任务列表没有任务，那么就会阻塞在这里
            Task task{take()};
            if (task) {
                task();
                task_count--;
                if (task_count == 0) {
                    shutdown.notify_all();
                }
            } else {
            }
        }
    } catch (const std::exception &ex) {
        std::cerr << "exception caught in ThreadPool" << std::endl;
        std::cerr << "reason :" << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception caught in ThreadPool" << std::endl;
        throw;
    }

}

fastflow::threadpool::Task fastflow::threadpool::take() {
    std::unique_lock<std::mutex> uniqueLock(mutex_);
    // 如果队列为空才等待
    while (tasks.empty() && isRunning_) {
        notEmpty_.wait(uniqueLock);
    }
    Task task;
    // 有可能是虚假唤醒 所以这里需要判断一下
    if (!tasks.empty()) {
        task = tasks.front();
        tasks.pop();
    }
    return task;
}

fastflow::threadpool::~threadpool() {
    if (isRunning_) {
        stop(false);
    }
    printf("threadpool is stop,remain task task_count == [%d]\n", (int) fastflow::threadpool::task_count);
}

const std::string &fastflow::threadpool::getPoolName() const {
    return poolName_;
};

std::atomic<int> fastflow::threadpool::task_count = 0;



