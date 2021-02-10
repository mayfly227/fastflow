//
// Created by itsuy on 2021/2/8.
//

#include <iostream>
#include "ThreadPool.h"


FastFlow::ThreadPool::ThreadPool(std::string name)
        : mutex_(),
          poolName_(std::move(name)),
          isRunning_(false) {
}

void FastFlow::ThreadPool::start(int numThreads) {
    isRunning_ = true;
    threads_.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i) {
        threads_.emplace_back(
                std::make_unique<std::thread>(std::thread([this] { runInThread(); }))
        );
    }
}

void FastFlow::ThreadPool::run(FastFlow::ThreadPool::Task task) {
    if (threads_.empty()) {
        task();
    } else {
        std::unique_lock<std::mutex> uniqueLock(mutex_);
        count++;
        if (!isRunning_) {
            printf("not running...\n");
            return;
        }
        tasks.emplace(task);
        // 唤醒某个线程
        notEmpty_.notify_one();
    }
}

void FastFlow::ThreadPool::stop() {
    {
        std::cout << "stop thread pool\n";
        std::unique_lock<std::mutex> uniqueLock(mutex_);
        // 标志位为false,runInThread会跳出while，线程结束
        //如果不唤醒所有的线程,那么子线程就会卡在wait
        notEmpty_.notify_all();
        while (count > 0) {
            printf("count被唤醒了前。。。");
            shutDown.wait(uniqueLock);
            printf("count被唤醒了后。。。");
            isRunning_ = false;
            notEmpty_.notify_all();
        }
    }
    std::cout << "EXIT\n";
    printf("当前queue的大小: %lu\n", tasks.size());
    printf("当前thread的大小: %lu\n", threads_.size());
    for (auto &t:threads_) {
// 等待子线程退出
        t->join();
    }
}

void FastFlow::ThreadPool::runInThread() {
//    std::cout << "runInThread\n";
//    std::flush(std::cout);
    while (isRunning_ == true) {
        // 如果任务列表没有任务，那么就会阻塞在这里
        Task task{take()};
        if (task) {
            task();
            count--;
            if (count == 0) {
                shutDown.notify_all();
            }
        } else {
        }
    }
}

FastFlow::ThreadPool::Task FastFlow::ThreadPool::take() {
    std::unique_lock<std::mutex> uniqueLock(mutex_);
    // 如果队列为空才等待
    while (tasks.empty() && isRunning_) {
        std::cout << "no task,wait for notify~" << std::endl;
        notEmpty_.wait(uniqueLock);
    }
//    printf("被唤醒了...\n");
    Task task;
    // 有可能是虚假唤醒 所以这里需要判断一下
    if (!tasks.empty()) {
        task = tasks.front();
        tasks.pop();
    }
    return task;
}

FastFlow::ThreadPool::~ThreadPool() {
    if (isRunning_) {
        stop();
    }
}

const std::string &FastFlow::ThreadPool::getPoolName() const {
    return poolName_;
};

std::atomic<int> FastFlow::ThreadPool::count = 0;



