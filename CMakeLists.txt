cmake_minimum_required(VERSION 3.10)
project(NetLib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -pthread")

# base 源文件
set(BASE_SOURCES
    base/logger.cc
    base/logStream.cc
    base/Timestamp.cc
    base/CurrentThread.cc
)

# base 头文件
set(BASE_HEADERS
    base/logger.h
    base/logStream.h
    base/Timestamp.h
    base/CurrentThread.h
    base/noncopyable.h
)

# net 源文件
set(NET_SOURCES
    net/Acceptor.cc
    net/Buffer.cc
    net/Channel.cc
    net/Epoller.cc
    net/EventLoop.cc
    net/EventLoopThreadpool.cc
    net/SocketOps.cc
    net/TcpConnection.cc
    net/TcpServer.cc
    net/Timer.cc
    net/TimerQueue.cc
)

# net 头文件
set(NET_HEADERS
    net/Acceptor.h
    net/Buffer.h
    net/Channel.h
    net/Epoller.h
    net/EventLoop.h
    net/EventLoopThread.h
    net/EventLoopThreadpool.h
    net/InetAddress.h
    net/sigpipe.h
    net/Socket.h
    net/SocketOps.h
    net/TcpConnection.h
    net/TcpServer.h
    net/Timer.h
    net/TimerId.h
    net/TimerQueue.h
)

# test 源文件
set(TEST_SOURCES
    test/server.cc
)

# 汇总所有参与编译的源文件
add_executable(netlib_test
    ${BASE_SOURCES}
    ${NET_SOURCES}
    ${TEST_SOURCES}
)

# 添加头文件用于 IDE 提示
target_sources(netlib_test PRIVATE ${BASE_HEADERS} ${NET_HEADERS})

# 头文件搜索路径
target_include_directories(netlib_test PRIVATE
    ${CMAKE_SOURCE_DIR}/base
    ${CMAKE_SOURCE_DIR}/net
)
