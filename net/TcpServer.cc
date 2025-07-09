#include "../base/logger.h"
#include "TcpServer.h"
#include <assert.h>
using namespace mulib::net;

TcpServer::TcpServer(EventLoop *loop, std::string nameArg,const InetAddress &listenAddr) : loop_(loop), 
name_(nameArg),
acceptor_(new Acceptor(loop,listenAddr)),
threadpool_(new EventLoopThreadPool(loop)),
connectionCallback_(),
messageCallback_(),
started_(false),
nextConnId_(1){
    acceptor_->setNewConnectionCallback([this](int sockfd, const InetAddress &peerAddr) { newConnection(sockfd, peerAddr); });
}
void TcpServer::start(){
    if(! started_){
        started_ = true;
        threadpool_->start();
    }
    if (!acceptor_->listening()){
        loop_->runInLoop([this] { acceptor_->listen(); });
    }
}
void TcpServer::setThreadNum(int numThreads){
    assert(numThreads >= 0);
    threadpool_->setThreadNum(numThreads);
}
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr){
    loop_->assertInLoopThread();
    EventLoop *ioLoop = threadpool_->getNextLoop();
    char buff[32];
    snprintf(buff, sizeof(buff), "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buff;
    LOG_INFO << "TcpServer::newConnection [" << name_ << "] - new connection [" << connName << "] form " << peerAddr.toHostPort().c_str();
    InetAddress localAddr(socket::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback([this](const TcpConnectionPtr &conn){
    removeConnection(conn); });
    ioLoop->runInLoop([conn]
                      { conn->connectEstablished(); });
}
void TcpServer::removeConnection(const TcpConnectionPtr &conn){
    loop_->runInLoop([this, conn]
                     { removeConnectionInLoop(conn); });
}
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn){
    loop_->assertInLoopThread();
    size_t n = connections_.erase(conn->name());
    (void)n;
    assert(n == 1);
    EventLoop *subLoop = conn->getLoop();
    subLoop->queueInLoop([conn]
                         { conn->connectDestroyed(); });
} // 在对应的 EventLoop 中销毁该连接。
TcpServer::~TcpServer() {
    for (auto &item : connections_)
    {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}