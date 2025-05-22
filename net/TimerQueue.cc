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