//
// Created by itsuy on 2021/2/18.
//

#ifndef FASTFLOW_POLLER_H
#define FASTFLOW_POLLER_H

#include <functional>
#include <vector>
#include <map>

#include "fastflow/base/noncopyable.h"
#include "fastflow/net/eventloop.h"

#ifdef WIN32

#include <WinSock2.h>

#elif __linux__
#include <poll.h>
#endif

class channel;

//IO多路复用的封装
// poller是event loop的间接成员,只供其所属loop在IO线程使用
class poller : fastflow::noncopyable {
public:
    // 保存所有的IO处理通道
    typedef std::vector<channel *> channel_list;

    explicit poller(event_loop *loop);

    //remove channel after ~poller()
    ~poller();

    // 处理IO事件
    // 必需在IO线程(也就是loop所在的线程)
    void poll_channels(int timeout_ms, channel_list *activate_channels);

    //channel->update->(loop->update_channel)->this update_channel
    // 更新channel
    // 维护和更新poll_fd_list_数组
    void update_channel(channel *channel);

    //确保在IO线程进行
    void assert_in_loop_thread() {
        owner_loop->assert_in_loop_thread();
    }

private:
    void fill_activate_channels(int num_events, channel_list *activate_channels);

    //pollfd成员
    typedef std::vector<pollfd> poll_fd_list;
    //fd -> channel 映射
    typedef std::map<int, channel *> channels_map;
    event_loop *owner_loop;
    poll_fd_list poll_fd_list_;
    channels_map channels_map_;

};


#endif //FASTFLOW_POLLER_H
