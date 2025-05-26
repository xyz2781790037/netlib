#include "TimerQueue.h"
#include "EventLoop.h"
#include "TimerId.h"
#include <algorithm>
#include "../base/Timestamp.h"
#include <sys/timerfd.h>
#include <assert.h>
#include <unistd.h>
#include <memory.h>

using namespace mulib::net;

TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop),
      timerfd_(createTimerfd()),
      timerfdChannel_(loop, timerfd_),
      timers_() {}

int TimerQueue::createTimerfd(){
    return timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
}
TimerId TimerQueue::addTimer(const Timer::TimerCallback &cb, Timestamp when, double interval){
    Timer *timer = new Timer(cb, when, interval);
    loop_->runInLoop([this, timer]
                     { addTimerInLoop(timer); 
                    });
    return TimerId(timer,timer->sequence());
}
void TimerQueue::cancel(TimerId timerid){
    loop_->runInLoop([this, timerid]
                     { cancelInLoop(timerid); });
}

bool TimerQueue::insert(Timer *timer){
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if(it == timers_.end() || when < it->first){
        earliestChanged = true; // 如果当前定时器是第一个插入的，或者它的触发时间比原来最早的还要早，那么我们需要更新 timerfd 的到期时间（所以设 earliestChanged = true）。
    }
    {
        std::pair<TimerList::iterator, bool> result = timers_.insert(Entry(when, timer));
        assert(result.second);
        (void)result;
    }
    {
        std::pair<ActiveTimerSet::iterator, bool> result = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
        assert(result.second);// set只能插入没有的变量
        (void)result; // 显式声明我知道这个变量没用，但我故意写它为了避免编译器报 “变量未使用” 的警告
    }
    assert(timers_.size() == activeTimers_.size());
    return earliestChanged;
}
void TimerQueue::resetTimerfd(int timerfd, Timestamp expiration){
    itimerspec newValue;
    itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(oldValue));
    newValue.it_value = 
}
void TimerQueue::addTimerInLoop(Timer *timer){
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);
    if(earliestChanged){
        resetTimerfd(timerfd_, timer->expiration());
    }
}