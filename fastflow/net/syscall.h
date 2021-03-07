//
// Created by itsuy on 2021/2/24.
//

#ifndef FASTFLOW_SYSCALL_H
#define FASTFLOW_SYSCALL_H

#include "fastflow/base/logging.h"
#include "fastflow/base/timestamp.h"

#ifdef WIN32

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")
#elif __linux__

#include <cstring>
#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>

#endif

#include <thread>
#include <chrono>
#include <future>
#include <tuple>

using namespace fastflow;
using namespace std::chrono_literals;


#ifdef WIN32

#define CLOCK_MONOTONIC 0
#define TFD_NONBLOCK 1
#define TFD_CLOEXEC 2

typedef struct {
    int pipe_read;
    int pipe_write;
} wakeup_fd_pipe;

typedef struct itimerspec {
    struct timespec it_interval;
    struct timespec it_value;
} itimerspec;
//struct timespec{
//    long int tv_sec;        /* Seconds.  */
//    long int tv_nsec;    /* Nanoseconds.  */
//};

int pipe(int fd[]);

std::tuple<int, int> eventfd(int f, int flags);

#endif

class timer_fd {
public:
    static int timerfd_create(int clock, int flags);

    static int timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value);

    static int fd_[2];
};


// 对系统调用的封装
#ifdef WIN32

wakeup_fd_pipe create_eventfd();

#elif __linux__

int create_eventfd();

#endif

int create_timerfd();

timespec time_from_now(timestamp when);

void read_timerfd(int timerfd, timestamp now);

void reset_timerfd(int timerfd, timestamp expiration);


#endif //FASTFLOW_SYSCALL_H
