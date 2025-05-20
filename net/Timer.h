#ifndef MUDUO_NET_TIMER_H
#define MUDUO_NET_TIMER_H


#include "../base/Timestamp.h"
#include "../base/noncopyable.h"
#include <functional>
#include <atomic>


namespace mulib{
    using base::Timestamp;
    namespace net{
        class Timer : noncopyable
        {
        public:
            using TimerCallback = std::function<void()>;
            Timer(TimerCallback cb, Timestamp when, double interval);
            void run() const;
            Timestamp expiration() const;
            bool repeat() const;
            int64_t sequence() const;

            void restart(Timestamp now);
            static int64_t numCreated();

        private:
            const TimerCallback callback_;
            Timestamp expiration_;
            const double interval_;
            const bool repeat_;

            const int64_t sequence_;
            static std::atomic<int64_t> s_numCreated;
        };
    }
}

#endif