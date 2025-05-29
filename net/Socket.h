#ifndef MUDUO_NET_SOCKET_H
#define MUDUO_NET_SOCKET_H

#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include "../base/noncopyable.h"
#include "SocketOps.h"
#include "InetAddress.h"

namespace mulib{
    namespace net{
        class Socket : noncopyable{
        public:
            explicit Socket(int sockfd) : sockfd_(sockfd) {}
            ~Socket();
            int fd() const { return sockfd_; };

            void bindAddress(const InetAddress &localaddr);
            void listen() const;
            int accept(InetAddress *peeraddr);

            void setTcpNoDelay(bool on);
            void setReuseAddr(bool on);
            void shutdownWrite() const;

        private:
            int sockfd_;
        };
    }
}
using namespace mulib::net;
Socket::~Socket(){
    socket::close(sockfd_);
}
void Socket::bindAddress(const InetAddress &localaddr){
    socket::bindOrDie(sockfd_, localaddr.getSockAddr());
}

void Socket::listen() const{
    socket::listenOrDie(sockfd_);
}
void Socket::shutdownWrite() const{
    socket::shutdownWrite(sockfd_);
}
int Socket::accept(InetAddress *peeraddr){
    struct sockaddr_in addr;
    memset(&addr, 0,sizeof(addr));
    int connfd = socket::accept(sockfd_, &addr);
    if (connfd >= 0)
    {
        *peeraddr = addr;
    }
    return connfd;
}
void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}
#endif