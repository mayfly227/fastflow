//
// Created by itsuy on 2021/2/8.
//

#ifndef FASTFLOW_BASE_TESTS_THREADPOOL_H
#define FASTFLOW_BASE_TESTS_THREADPOOL_H
/*
 * 简易线程池实现
 * */
#include "fastflow/base/noncopyable.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace FastFlow {
    class ThreadPool : noncopyable {
    public:
        explicit ThreadPool(std::string name = "ThreadPool");

        ~ThreadPool();

    public:
        // 任务回调
        typedef std::function<void()> Task;

        // 启动i个线程
        void start(int numThreads);

        // 运行任务(实际是唤醒某个线程)
        void run(const Task &task);

        // 停止所有任务
        void stop(bool wait_all_task_complete);

        const std::string &getPoolName() const;

    private:
        // 获取任务列表任务，消费并执行，一旦没有任务就会阻塞在take处
        void runInThread();

        //获取一个任务
        Task take();

        // 全局标志位，是否运行
        bool isRunning_;
        // 内部锁
        mutable std::mutex mutex_;
        // 队列是否为空条件变量
        std::condition_variable notEmpty_;
        std::condition_variable shutDown;
        std::string poolName_;

        // 任务列表
        std::queue<Task> tasks;
        //线程列表
        std::vector<std::unique_ptr<std::thread>> threads_;
    public:
        static std::atomic<int> count;

    };

}

#endif //FASTFLOW_BASE_TESTS_THREADPOOL_H
