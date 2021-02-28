//
// Created by itsuy on 2021/2/18.
//

#ifdef WIN32

#include <WinSock2.h>

#elif __linux__
#include <poll.h>
#endif

#include "fastflow/net/channel.h"

const int channel::k_none_event = 0;
#ifdef WIN32
//POLLPRI flag is not supported by the Microsoft Winsock provider.
const int channel::k_read_event = POLLIN;
#elif __linux__
const int channel::k_read_event = POLLIN | POLLPRI;
#endif
const int channel::k_write_event = POLLOUT;

channel::channel(event_loop *loop, int fd)
        : loop_(loop),
          fd_(fd),
          events_(0),
          revents_(0),
          index_(-1) {

}

void channel::handle_event() {
    if (revents_ & POLLNVAL) {
        fastflow::logging::warn("handle event [POLLNVAL]");
    }
    //错误回调
    if (revents_ & (POLLERR | POLLNVAL)) {
        if (error_cb_) {
            error_cb_();
        }
    }
    //read回调
    if (revents_ & (POLLIN | POLLPRI | POLLHUP)) {
        if (read_cb_) {
            read_cb_();
        }
    }
    //write回调
    if (revents_ & POLLOUT) {
        if (write_cb_) {
            write_cb_();
        }
    }
}

void channel::update() {
    loop_->update_channel(this);
}


