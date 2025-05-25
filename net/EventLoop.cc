#include "EventLoop.h"
#include <iostream>
#include <sys/epoll.h>
#include "Channel.h"
#include "Epoller.h"
#include <sys/eventfd.h>
#include "../base/logger.h"
#include <unistd.h>
#include <assert.h>

using namespace mulib::net;

thread_local EventLoop *t_loopInThisThread = nullptr;
IgnoreSigPipe __on;
EventLoop::EventLoop()
: looping_(false), quit_(false),iteration_(0),
threadId_(std::this_thread::get_id()),
poller_(std::make_unique<Epoller>(this)),
wakeupFd_(createEventfd()),
wakeupChannel_(new Channel(this, wakeupFd_)){
    LOG_DEBUG << "EventLoop created " << this << " in thread " << threadId_;
    if(t_loopInThisThread){
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
        << " exists in this thread " << threadId_;
    }
    else{
        t_loopInThisThread = this;
    }
    wakeupChannel_->setReadCallback([this](Timestamp)
                                    { handleRead(); });
    wakeupChannel_->enableReading();
}
EventLoop::~EventLoop(){
    LOG_DEBUG << "EventLoop " << this << " of thread " << threadId_ 
    << " destructs in thread " << std::this_thread::get_id();
    wakeupChannel_->disableAll();
    t_loopInThisThread = nullptr;
}
void EventLoop::loop(int timeout = -1){
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;
    LOG_TRACE << "EventLoop " << this << " start looping";

    while(!quit_){
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(timeout, activeChannels_);
        for(auto channel : activeChannels_){
            channel->handleEvent(pollReturnTime_);
        }
        doPendingFunctors();
    }
}
void EventLoop::quit(){
    quit_ = true;
    if(!isInLoopThread()){
        wakeup();
    }
}

bool EventLoop::isInLoopThread() const{
    return std::this_thread::get_id() == threadId_;
}

void EventLoop::assertInLoopThread(){
    if(!isInLoopThread()){
        abortNotInLoopThread();
    }
}
void EventLoop::abortNotInLoopThread(){
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << std::this_thread::get_id();
}
void EventLoop::runInLoop(const Functor &cb)
{
    if(isInLoopThread()){
        cb();
    }
    else{
        queueInLoop(cb);
    }
}
void EventLoop::wakeup(){
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof(one))
    {
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}
void EventLoop::queueInLoop(const Functor &cb)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb); // 加入待执行队列
    }

    // 如果不是在本线程，或者正在调用 functors（说明是嵌套），就唤醒 EventLoop
    if (!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}

int EventLoop::createEventfd(){
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_SYSERR << "Failed in eventfd";
        abort(); // 或者抛出异常
    }
    return evtfd;
}

void EventLoop::handleRead(){
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof(one))
    {
        LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
} // 读出写进 eventfd 的值，清空事件，避免 epoll 一直触发。
void EventLoop::doPendingFunctors(){
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        std::unique_lock lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(auto functor : functors){
        functor();
    }

    callingPendingFunctors_ = false;
}