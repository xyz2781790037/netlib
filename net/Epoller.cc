#include "Epoller.h"
#include <errno.h>
#include <unistd.h>

const int _knew = -1;
const int _kadded = 1;
const int _kdel_ed = 2;
using namespace mulib::net;
using ChannelList = std::vector<Channel *>;
Epoller::Epoller(EventLoop *loop) :
ownerLoop_(loop),epollfd_(epoll_create1(EPOLL_CLOEXEC)),events_(kInitEventListSize){
    if (epollfd_ < 0)
    {
        LOG_FATAL << "Epoller::Epoller";
    }
}

Epoller::~Epoller()
{
    close(epollfd_);
}

mulib::base::Timestamp Epoller::poll(int timeoutMs, ChannelList &activeChannels){
    int numEvents = epoll_wait(epollfd_, events_.data(), events_.size(), timeoutMs);
    Timestamp now(Timestamp::now());
    if(numEvents > 0){
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents == events_.size()){
            events_.resize(2 * events_.size());
        }
    }
    else if(numEvents == 0){
        LOG_INFO << "Epoller::epoll: nothing happend!";
    }
    else{
        LOG_WARN << "Epoller::epoll: ret < 0";
    }
    return now;
}
void Epoller::fillActiveChannels(int numEvents, ChannelList &activeChannels) const{
    for (int i = 0; i < numEvents;i++){
        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
        channel->setRevents(events_[i].events);
        activeChannels.push_back(channel);
    }
}
void Epoller::removeChannel(Channel *channel)
{
    assertInLoopThread();
    int fd = channel->fd();
    int index = channel->index();
    channels_.erase(fd);
    if (index == _kadded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(_knew);
}