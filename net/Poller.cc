#include "Poller.h"
#include <poll.h>
#include "../base/logger.h"
#include "../base/Timestamp.h"
#include <assert.h>
#include "Channel.h"

using namespace mulib::base;
using namespace mulib::net;

Poller::Poller(EventLoop *loop) : ownerLoop_(loop) {}
Poller::~Poller() {}
