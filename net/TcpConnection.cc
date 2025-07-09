#include "TcpConnection.h"
#include "../base/logger.h"
#include "assert.h"
using namespace mulib::net;

TcpConnection::TcpConnection(EventLoop *loop, std::string conName, int sockfd, const InetAddress localAddr, const InetAddress peerAddr) : loop_(loop),
  name_(conName), state_(kConnecting),
socket_(new Socket(sockfd)),
channel_(new Channel(loop, sockfd)),
localAddr_(localAddr),
peerAddr_(peerAddr){
    channel_->setReadCallback([this](Timestamp recvTime)
                              { handleRead(recvTime); });
    channel_->setWriteCallback([this]
                               { handleWrite(); });
    channel_->setCloseCallback([this]
                               { handleClose(); });
    channel_->setErrorCallback([this]
                               { handleError(); });
    LOG_DEBUG << "TcpConnection::ctor[" << name_ << "] at " << this << " fd=" << sockfd;//DEBUG
}
const char *TcpConnection::stateToString() const{
    switch (state_)
    {
    case kDisconnected:
        return "kDisconnected";
    case kConnecting:
        return "kConnecting";
    case kConnected:
        return "kConnected";
    case kDisconnecting:
        return "kDisconnecting";
    default:
        return "unknown state";
    }
}
TcpConnection::~TcpConnection(){
    LOG_DEBUG << "TcpConnection::dtor[" << name_ << "] at " << this
              << " fd=" << channel_->fd() << " state=" << stateToString();//DUBUG
    assert(state_ == kDisconnected);
}
void TcpConnection::connectEstablished(){
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}
void TcpConnection::connectDestroyed(){
    loop_->assertInLoopThread();
    if(state_ == kConnected){
        setState(kDisconnected);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
    }
    loop_->removeChannel(channel_.get());
}
void TcpConnection::send(const std::string &message){
    if (state_ == kConnected){
        if (loop_->isInLoopThread()){
            sendInLoop(message);
        }
        else{
            loop_->runInLoop([this, &message]()
                             { sendInLoop(std::move(message)); });
        }
    }
}
void TcpConnection::shutdown(){
    if (state_ == kConnected)
    {
        setState(kDisconnecting);
        loop_->runInLoop([this]()
                         { shutdownInLoop(); });
    }
}
void TcpConnection::shutdownInLoop(){
    loop_->assertInLoopThread();
    if (!channel_->isWriting())
    {
        socket_->shutdownWrite();
    }
} // 只关闭写端，防止对面还在发消息
void TcpConnection::sendInLoop(const std::string &message){
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
    if (state_ == kDisconnected){
        LOG_WARN << "disconnected, give up writing";
        return;
    }
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0){
        nwrote = ::write(channel_->fd(), message.data(), message.size());
        if (nwrote >= 0){
            if (static_cast<size_t>(nwrote) < message.size()){
                LOG_WARN << "data not fully written";
            }
            else if (writeCompleteCallback_){
                loop_->queueInLoop([this]()
                                   { writeCompleteCallback_(shared_from_this()); });
            }
        }
        else{
            nwrote = 0;
            if (errno != EWOULDBLOCK) // 对端 socket 的发送缓冲区满了
            {
                LOG_SYSERR << "TcpConnection::sendInLoop: send failed";
            }
        }
    }
    assert(nwrote > 0);
    if (static_cast<size_t>(nwrote) < message.size())
    {
        outputBuffer_.append(message.data() + nwrote, message.size() - nwrote);
        if (!channel_->isWriting())
        {
            channel_->enableWriting();
        }
    }
}
void TcpConnection::handleRead(Timestamp receiveTime){
    loop_->assertInLoopThread();
    int saveErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
    if (n > 0){
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    }
    else if (n == 0){
        handleClose();
    }
    else{
        errno = saveErrno;
        LOG_SYSERR << "TcpConnection::handleRead";
        handleError();
    }
}
void TcpConnection::handleClose(){
    loop_->assertInLoopThread();
    LOG_INFO << "fd= " << channel_->fd() << " state= " << stateToString();
    assert(state_ == kConnected || state_ == kDisconnecting);
    setState(kDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());
    closeCallback_(shared_from_this());
    loop_->removeChannel(channel_.get());
}
void TcpConnection::handleWrite(){
    loop_->assertInLoopThread();
    if (channel_->isWriting()){
        ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
        if (n > 0){
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0){
                channel_->disableWriting();
            }
            if (writeCompleteCallback_){
                loop_->queueInLoop([this]()
                                   { writeCompleteCallback_(shared_from_this()); });
            }
            if (state_ == kDisconnecting){
                shutdownInLoop();
            }
        }
        else{
            LOG_SYSERR << "TcpConnection::handleWrite";
        }
    }
    else{
        LOG_TRACE << "Connection fd= " << channel_->fd() << " is down,no more writing";
    }
}
void TcpConnection::handleError(){
    int err = socket::getSocketError(channel_->fd());
    LOG_ERROR << "TcpConnection::handleError [" << name_
              << "] - SO_ERROR = " << err << " " << strerror(err);
}
void TcpConnection::forceClose(){
    if (state_ == kConnected || state_ == kDisconnecting){
        setState(kDisconnecting);
        loop_->runInLoop([self = shared_from_this()]()
                         {
                             self->handleClose();
                         });
    }
}