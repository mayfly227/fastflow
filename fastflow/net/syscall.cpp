#include "fastflow/net/syscall.h"

int timer_fd::fd_[2] = {-1, -1};

int timer_fd::timerfd_create(int clock, int flags) {
#ifdef WIN32
    if (pipe(fd_) < 0) {
        fastflow::logging::error("create timer fd error");
    }
    fastflow::logging::trace("create timer fd[0] = %d ,fd[1] = %d", fd_[0], fd_[1]);
    return fd_[0];
#elif __linux__
    return ::timerfd_create(clock, flags);
#endif

}

int timer_fd::timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value) {
#ifdef WIN32
    auto sec = new_value->it_value.tv_sec;
    auto nsec = new_value->it_value.tv_nsec;
    auto millisecond = sec * 1000 + nsec / 1000000;
    // millisecond后唤醒fd
    std::async(std::launch::async, [=] {
        char buffer[1]{'8'};
        std::this_thread::sleep_for(std::chrono::milliseconds(millisecond));
        fastflow::logging::trace("pipe wake up after [%ld] ms", millisecond);
        send(fd_[1], buffer, sizeof(buffer), 0);
    }).wait_for(1ms);
#elif __linux__
    return ::timerfd_settime(fd, flags, new_value, old_value);
#endif
    return 0;
}

#ifdef WIN32

std::tuple<int, int> eventfd(int f, int flags) {
    int fd[2];
    if (pipe(fd) < 0) {
        fastflow::logging::error("create event fd error");
    }
    return std::make_tuple(fd[0], fd[1]);
}

wakeup_fd_pipe create_eventfd() {
    auto[read, write] = ::eventfd(0, 0);
    fastflow::logging::trace("create event fd[0] = %d ,fd[1] = %d", read, write);
    wakeup_fd_pipe t{read, write};
    return t;
}

int pipe(int fd[]) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
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

#elif __linux__

int create_eventfd() {
    int eventfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (eventfd < 0) {
        fastflow::logging::error("event fd create error!");
        exit(0);
    }
    return eventfd;
}

#endif

int create_timerfd() {
    int timerfd = timer_fd::timerfd_create(CLOCK_MONOTONIC,
                                           TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0) {
        fastflow::logging::error("Failed in timerfd_create");
    }
    return timerfd;
}

timespec time_from_now(timestamp when) {
//    fastflow::logging::trace("time_from_now :%s",timestamp::to_fmt_string(when.getTimePointSystem()).c_str());

    int64_t microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
            when.getTimePointSystem() - timestamp::now_system()).count();
//    fastflow::logging::trace("time_from_now microseconds: %ld",microseconds);
    if (microseconds < 100) {
        microseconds = 100;
    }
    struct timespec ts{};
    ts.tv_sec = static_cast<time_t>(
            microseconds / (1000 * 1000));
    ts.tv_nsec = static_cast<long>(
            (microseconds % (1000 * 1000)) * 1000);
    return ts;
}

void read_timerfd(int timerfd, timestamp now) {
#ifdef WIN32
    char howmany[1];
    auto n = ::recv(timerfd, howmany, sizeof howmany, 0);
#elif __linux__
    uint64_t howmany;
    auto n = ::read(timerfd, &howmany, sizeof howmany);
#endif
//    fastflow::logging::trace("TimerQueue::handleRead() [%ld] at", howmany);
    if (n != sizeof howmany) {
        fastflow::logging::error("TimerQueue::handleRead() reads [%d] instead of 8", n);
    }
}

void reset_timerfd(int timerfd, timestamp expiration) {
    // wake up loop by timerfd_settime()
    struct itimerspec newValue{};
    struct itimerspec oldValue{};
    memset(&newValue, 0, sizeof newValue);
    memset(&oldValue, 0, sizeof oldValue);
    newValue.it_value = time_from_now(expiration);
//    fastflow::logging::trace("sec: [%ld] nan[%ld]", newValue.it_value.tv_sec, newValue.it_value.tv_nsec);
    int ret = timer_fd::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret) {
        fastflow::logging::error("timerfd_settime()");
    }
}