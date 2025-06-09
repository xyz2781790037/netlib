#include "../net/TcpServer.h"
#include "../net/EventLoop.h"
#include "../net/InetAddress.h"
#include "../base/logger.h"

using namespace mulib;

void onConnection(const net::TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        std::cout << "A connection has came\n";
        conn->send("hello!");
    }
}

void onClose(const net::TcpConnectionPtr &conn)
{
    std::cout << "Close\n";
}

int main()
{
    net::EventLoop mainLoop;
    net::InetAddress addr(8080);
    net::TcpServer server(&mainLoop, addr);

    server.setThreadNum(32);
    server.start();
    server.setConnectionCallback(onConnection);

    mainLoop.loop(-1);

    return 0;
}