#include "TcpClient.h"
#include "../base/logger.h"
#include <mutex>
#include <assert.h>
#include "Connector.h"
#include "EventLoop.h"
#include "SocketOps.h"
#include <string>
using namespace mulib::net;
TcpClient::TcpClient(EventLoop *loop, const InetAddress &serverAddr)
    : loop_(loop),
      connector_(new Connector(loop, serverAddr)),
      retry_(false),
      connect_(true),
      nextConnId_(1){
    connector_->setNewConnectionCallback([this](int sockfd)
                                         { newConnection(sockfd); });
    LOG_INFO << "TcpClient::TcpClient[" << this << "] - connector " << connector_.get();
}
void TcpClient::connect(){
    LOG_INFO << "TcpClient::connect[" << this << "] - connecting to "
             << connector_->serverAddress().toHostPort();
    connect_ = true;
    connector_->start();
}
void TcpClient::disconnect(){
    connect_ = false;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (connection_){
            connection_->shutdown();
        }
    }
}
void TcpClient::stop(){
    connect_ = false;
    connector_->stop();
}
void TcpClient::newConnection(int sockfd){
    loop_->assertInLoopThread();
    InetAddress peerAddr(socket::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof(buf), ":%s#%d", peerAddr.toHostPort().c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = buf;
    InetAddress localAddr(socket::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(loop_,
        connName,
        sockfd,
        localAddr,
        peerAddr));
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback([this](const TcpConnectionPtr &conn)
                           { removeConnection(conn); });
    {
        std::unique_lock<std::mutex> lock(mutex_);
        connection_ = conn;
    }
    conn->connectEstablished();
}
void TcpClient::removeConnection(const TcpConnectionPtr &conn){
    loop_->assertInLoopThread();
    assert(loop_ == conn->getLoop());

    {
        std::unique_lock<std::mutex> lock(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if (retry_ && connect_)
    {
        LOG_INFO << "TcpClient::connect[" << this << "] - Reconnecting to "
                 << connector_->serverAddress().toHostPort();
        connector_->restart();
    }
}