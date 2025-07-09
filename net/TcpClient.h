#ifndef MUDUO_NET_TCPCLIENT_H
#define MUDUO_NET_TCPCLIENT_H

#include "TcpConnection.h"
#include "../base/Timestamp.h"
#include <mutex>
#include "Connector.h"
namespace mulib{
    namespace net{
        class TcpClient
        {
        public:
            using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
            using ConnectorPtr = std::shared_ptr<Connector>;
            using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
            using MessageCallback = std::function<void(const TcpConnectionPtr &, Buffer *, Timestamp)>;
            using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
            TcpClient(EventLoop *loop, const InetAddress &serverAddr);
            ~TcpClient();
            void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
            void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
            void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

            void connect();
            void disconnect();
            void stop();
        
        private:
            void newConnection(int sockfd);
            void removeConnection(const TcpConnectionPtr &conn);

            bool connect_;
            bool retry_;
            int nextConnId_;
            EventLoop *loop_;
            ConnectorPtr connector_;
            ConnectionCallback connectionCallback_;
            MessageCallback messageCallback_;
            WriteCompleteCallback writeCompleteCallback_;

            TcpConnectionPtr connection_;
            std::mutex mutex_;
        };
    }
    
}
#endif