#include "TcpConnection.h"
#include "../base/logger.h"
using namespace mulib::net;

TcpConnection::TcpConnection(EventLoop *loop,std::string conName,int sockfd,const InetAddress localAddr,const InetAddress peerAddr) : loop_(loop),
name_(conName),state_(kConnecting),
socket_(new Socket(sockfd)),
channel_(new Channel(loop,sockfd)),
localAddr_(localAddr),
peerAddr_(peerAddr){
    channel_->setReadCallback([this](Timestamp recvTime) { handleRead(recvTime); });
    channel_->setWriteCallback([this]
                               { handleWrite(); });
    channel_->setCloseCallback([this]
                               { handleClose(); });
    channel_->setErrorCallback([this]
                               { handleError(); });
    LOG_DEBUG << "TcpConnection::ctor[" << name_ << "] at " << this << " fd=" << sockfd;
}
