#include "../base/logger.h"
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
int main(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_port = 10;
    if (bind(fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        LOG_ERROR << "my" << std::endl;
    }

    return 0;
}