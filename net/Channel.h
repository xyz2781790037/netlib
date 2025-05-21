#ifndef MUDUO_NET_CHANNEL_H
#define MUDUO_NET_CHANNEL_H

#include "../base/noncopyable.h"
#include "../base/Timestamp.h"

#include <functional>
#include <memory>
namespace mulib{
    namespace net{
        class EventLoop;
        class Channel : mulib::noncopyable{
        public:
            using EventCallback = std::function<void()>;
            using ReadEventCallback = std::function<void(base::Timestamp)>;

            Channel(EventLoop *loop, int fd);
            ~Channel();
            void handleEvent(base::Timestamp);
            inline void setReadCallback(const ReadEventCallback &cb);
            inline void setWriteCallback(const EventCallback &cb);
            inline void setErrorCallback(const EventCallback &cb);
            inline void setCloseCallback(const EventCallback &cb);

            inline int fd() const;
            inline int events() const;
            inline void set_revents(int revt);
            inline bool isNoneEvent() const;

            void enableReading();
            void disableReading();
            void enableWriting();
            void disableWriting();
            void disableAll();

            bool isWriting() const;
            bool isReading() const;

            int index();
            void set_index(int idx);

        private:
            void update();
            static const int kNoneEvent;
            static const int kReadEvent;
            static const int kWriteEvent;
            EventLoop *loop_; // 所属的 EventLoop（1个 Channel 属于 1 个 EventLoop）
            const int fd_;    // 被监听的文件描述符
            int events_;      // 当前监听的事件（EPOLLIN / EPOLLOUT 等）
            int revents_;     // 实际发生的事件（由 Poller 设置）
            int index_;       // Poller 中使用的索引

            ReadEventCallback readCallback_;
            EventCallback writeCallback_;
            EventCallback closeCallback_;
            EventCallback errorCallback_;
        };
    }
}
#endif
using namespace mulib::net;
Channel::Channel(EventLoop *loop, int fd) : 
loop_(loop),fd_(fd),events_(kNoneEvent),revents_(kNoneEvent),index_(-1){}
