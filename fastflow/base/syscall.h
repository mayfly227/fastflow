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

#include "sys/timerfd.h"
#include <unistd.h>

#endif

#include <thread>
#include <chrono>
#include <future>
#include <tuple>

using namespace fastflow;

#ifdef WIN32
inline int pipe(int fd[]) {
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    int tcp1, tcp2;
    sockaddr_in sin{};
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
//    sin.sin_port = htons(7777);
    inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);
    socklen_t sin_len = sizeof(sin);
    tcp1 = tcp2 = -1;
    // 监听套接字
    int tcp = ::socket(AF_INET, SOCK_STREAM, 0);
    if (tcp == -1) {
        goto clean;
    }
    if (::bind(tcp, (sockaddr *) &sin, sin_len) == -1) {
        goto clean;
    }
    if (::listen(tcp, 5) == -1) {
        goto clean;
    }
    if (::getsockname(tcp, (sockaddr *) &sin, &sin_len) == -1) {
        goto clean;
    }
    // read端
    tcp1 = ::socket(AF_INET, SOCK_STREAM, 0);
    if (tcp1 == -1) {
        goto clean;
    }
    if (-1 == ::connect(tcp1, (sockaddr *) &sin, sin_len)) {
        goto clean;
    }
    // write端
    tcp2 = ::accept(tcp, (sockaddr *) &sin, &sin_len);
    if (tcp2 == -1) {
        goto clean;
    }

    if (::closesocket(tcp) == -1) {
        goto clean;
    }
    fd[0] = tcp1;
    fd[1] = tcp2;
    return 0;
    clean:
    if (tcp != -1) {
        ::closesocket(tcp);
    }
    if (tcp2 != -1) {
        ::closesocket(tcp2);
    }
    if (tcp1 != -1) {
        ::closesocket(tcp1);
    }
    return -1;
}
inline std::tuple<int,int> eventfd(int f,int flags){
    int fd[2];
    if (pipe(fd) < 0) {
        fastflow::logging::error("create event fd error");
    }
    return std::make_tuple(fd[0],fd[1]);
}
#endif

class timer_fd {
public:
#ifdef WIN32
    struct itimerspec {
        struct timespec it_interval;
        struct timespec it_value;
    };
    struct timespec {
        long int tv_sec;        /* Seconds.  */
        long int tv_nsec;    /* Nanoseconds.  */
    };
#endif

    static int timerfd_create(int clock, int flags) {
#ifdef WIN32
        if (pipe(fd_) < 0) {
            fastflow::logging::error("create timer fd error");
        }
        printf("fd[0] = %d ,fd[1] = %d\n", fd_[0], fd_[1]);
        return fd_[0];
#elif __linux__
        return ::timerfd_create(clock, flags);
#endif

    }

    static int timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value) {
#ifdef WIN32
        auto sec = new_value->it_value.tv_sec;
        auto nsec = new_value->it_value.tv_nsec;
        auto millisecond = sec * 1000 + nsec / 1000000;
        // millisecond后唤醒fd
        std::async(std::launch::async,[=] {
            char buffer[1]{'8'};
            std::this_thread::sleep_for(std::chrono::milliseconds(millisecond));
            printf("send\n");
            printf("fd[0] = %d ,fd[1] = %d\n", fd_[0], fd_[1]);
            send(fd_[1], buffer, sizeof(buffer), 0);
        });
//        std::thread t();
//        t.join();
#elif __linux__
        return ::timerfd_settime(fd, flags, new_value, old_value);
#endif
        return 0;
    }

//private:
    static int fd_[2];
};


#endif //FASTFLOW_SYSCALL_H
