#include <iostream>
#include <memory.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8080);
    inet_pton(AF_INET, "localhost", &sin.sin_addr);

    connect(fd, (sockaddr *)&sin, sizeof(sin));

    cout << "connected!" << endl;

    char buff[1024] = {};
    recv(fd, buff, 1024, 0);

    cout << buff << endl;

    close(fd);

    // sleep(100);

    return 0;
}