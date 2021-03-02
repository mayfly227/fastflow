//
// Created by itsuy on 2021/2/18.
//

#include <cassert>
#include "fastflow/net/poller.h"
#include "fastflow/net/channel.h"

poller::poller(event_loop *loop)
        : owner_loop(loop) {

}

poller::~poller() {

}

void poller::poll_channels(int timeout_ms, poller::channel_list *activate_channels) {
    //获取当前活动的IO事件
#ifdef WIN32
    int num_events = ::WSAPoll(poll_fd_list_.data(), poll_fd_list_.size(), timeout_ms);
#elif __linux__
    int num_events = ::poll(poll_fd_list_.data(), poll_fd_list_.size(), timeout_ms);
#endif
    if (num_events > 0) {
        fastflow::logging::trace("[%d] events happened", num_events);
        // 填充channel
        fill_activate_channels(num_events, activate_channels);
    } else if (num_events == 0) {
        fastflow::logging::warn("no events happened!");
    } else {
        fastflow::logging::error("poller::poll_channels() error!");
#ifdef WIN32
        int error = WSAGetLastError();
        fastflow::logging::error("poller::poll_channels() error! error number in win is [%d]", error);
#elif __linux__
#endif
    }

}

void poller::fill_activate_channels(int num_events, poller::channel_list *activate_channels) {
    for (auto pfd : poll_fd_list_) {
        // 有事件发生
        if (pfd.revents > 0) {
            --num_events;
            if (num_events < 0) {
                break;
            }
            // 从map中根据fd寻找对应的channel
            auto ch = channels_map_.find(pfd.fd);
            assert(ch != channels_map_.end());
            auto found_channel = ch->second;
            found_channel->set_revents(pfd.revents);
            activate_channels->emplace_back(found_channel);
        }
    }
}


void poller::update_channel(channel *channel) {
    // 必需在IO线程
    assert_in_loop_thread();
    fastflow::logging::trace("[poller::update_channel] fd = %d,events = %d", channel->get_fd(), channel->get_events());

    //新的channel,添加到poll_fd_list_
    if (channel->get_index() < 0) {
        // 保证是新加入的
        assert(channels_map_.find(channel->get_fd()) == channels_map_.end());
        pollfd pfd{};
        pfd.fd = channel->get_fd();
        pfd.events = (short) (channel->get_events());
        // 加入事件列表
        poll_fd_list_.emplace_back(pfd);
        // idx start with 0
        int idx = (int) (poll_fd_list_.size()) - 1;
        channel->set_index(idx);
        // 添加到channels_map
        channels_map_[pfd.fd] = channel;
    } else {
        //更新
        assert(channels_map_.find(channel->get_fd()) == channels_map_.end());
        //map的channel 必需和现在的一致
        assert(channels_map_[channel->get_fd()] == channel);
        int idx = channel->get_index();
        auto &pfd = poll_fd_list_[idx];
        //保证下标正确
        assert(idx >= 0 && idx < (int) (poll_fd_list_.size()));
        pfd.events = (short) (channel->get_events());
        if (channel->is_set_none_event()) {
            pfd.fd = -1;
        }
    }

}
