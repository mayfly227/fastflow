//
// Created by itsuy on 2021/2/17.
//

#include "fastflow/net/channel.h"
#include "fastflow/net/eventloop.h"

#include <thread>
#include <iostream>
#include <string.h>

#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#elif __linux__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/timerfd.h>
#include "unistd.h"

#endif

int pipe(int fd[]) {
    int tcp1, tcp2;
    sockaddr_in sin{};
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
//    sin.sin_port = htons(7777);
    inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);
    socklen_t sin_len = sizeof(sin);
    tcp1 = tcp2 = -1;
    // 监听套接字
    int tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp == -1) {
        goto clean;
    }
    if (bind(tcp, (sockaddr *) &sin, sin_len) == -1) {
        goto clean;
    }
    if (listen(tcp, 5) == -1) {
        goto clean;
    }
    if (getsockname(tcp, (sockaddr *) &sin, &sin_len) == -1) {
        goto clean;
    }
    // read端
    tcp1 = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp1 == -1) {
        goto clean;
    }
    if (-1 == connect(tcp1, (sockaddr *) &sin, sin_len)) {
        goto clean;
    }
    // write端
    tcp2 = accept(tcp, (sockaddr *) &sin, &sin_len);
    if (tcp2 == -1) {
        goto clean;
    }

//#ifdef Win32
//    if (closesocket(tcp) == -1){
//    goto clean;
//}
//#elif __linux__
//    if (close(tcp) == -1) {
//    goto clean;
//}
//#endif
    fd[0] = tcp1;
    fd[1] = tcp2;
    return 0;
    clean:
#ifdef WIN32
    if (tcp != -1) {
        closesocket(tcp);
    }
    if (tcp2 != -1) {
        closesocket(tcp2);
    }
    if (tcp1 != -1) {
        closesocket(tcp1);
    }
#elif __linux__
    if (tcp != -1) {
        close(tcp);
    }
    if (tcp2 != -1) {
        close(tcp2);
    }
    if (tcp1 != -1) {
        close(tcp1);
    }
#endif
    return -1;
}

using namespace std;

event_loop *g_loop;

void timeout() {
    printf("Timeout!\n");
//    char buffer[2];
//    recv(fd[0], buffer, 2, 0);
//    cout << buffer << endl;
    g_loop->quit();
}

int main() {
    event_loop loop;
    g_loop = &loop;

//    int fd[2];
//    pipe(fd);
//    printf("fd[0] = %d fd[1] = %d\n", fd[0], fd[1]);
//    thread t([=] {
//        this_thread::sleep_for(chrono::seconds(3));
//        send(fd[1], "ok", 2, 0);
//    });
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
//    int pipe_fd_read = fd[0];
    channel ch(&loop, timerfd);
    ch.set_read_cb([&] { timeout(); });
//    设置channel的事件
    ch.enable_reading();

    struct itimerspec new_time;
    struct itimerspec old_time;
    bzero(&new_time, sizeof new_time);
    bzero(&old_time, sizeof old_time);
    new_time.it_value.tv_nsec = 10000;
    ::timerfd_settime(timerfd, 0, &new_time, &old_time);
//    t.join();
    loop.loop();

    ::close(timerfd);
}
