#ifndef MUDUO_NET_SOCKETOPS_H
#define MUDUO_NET_SOCKETOPS_H

#include <arpa/inet.h>
#include <unistd.h>

namespace mulib{
    namespace net{
        namespace socket{
            int createNonblockingOrDie();
            void bindOrDie(int sockfd, const sockaddr_in &addr);
            void listenOrDie(int sockfd);
            int accept(int sockfd, sockaddr_in *addr);
            void close(int sockfd);
            int connect(int sockfd, const sockaddr_in &addr);

            void setNonBlockAndCloseOnExec(int sockfd);
            void shutdownWrite(int sockfd);

            int getSocketError(int sockfd);
            sockaddr_in getLocalAddr(int sockfd);
            sockaddr_in getPeerAddr(int sockfd);
            bool isSelfConnect(int sockfd);
        }
    }
}

#endif