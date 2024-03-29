#pragma once

#include "Poller.h"
#include "Timestamp.h"

#include<vector>
#include<sys/epoll.h>

class Channel;

/*
epoll:
    epoll_create
    epoll_ctl
    epoll_wait
*/

class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop* loop);
    ~EPollPoller() override;

    Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;
    
private:
    static const int KInitEventListSize = 16;

    //返回发生发生的事件
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    //添加 删除Channel
    void update(int operation, Channel* channel);

    using EventList = std::vector<epoll_event>;

    int epollfd_;
    EventList events_;
};