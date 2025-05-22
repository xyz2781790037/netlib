#include "Epoller.h"
#include <errno.h>
#include <unistd.h>

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
    
}