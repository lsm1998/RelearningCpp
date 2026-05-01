#include <functional>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include <string>
#include <thread>

using namespace muduo;
using namespace muduo::net;

class EchoServer
{
public:
    EchoServer(EventLoop* loop, const InetAddress& listenAddr) : server_(loop, listenAddr, "EchoServer"), loop_(loop)
    {
        // 设置连接事件回调
        server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));

        // 设置消息处理回调
        server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2,
                                             std::placeholders::_3));

        // 设置线程数                                        
        server_.setThreadNum(std::thread::hardware_concurrency());
    }

    void start() { server_.start(); }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected())
        {
            LOG_INFO << "new connection: " << conn->peerAddress().toIpPort() << " -> "
                     << conn->localAddress().toIpPort();
        }
        else
        {
            LOG_INFO << "connection down: " << conn->peerAddress().toIpPort() << " -> "
                     << conn->localAddress().toIpPort();
        }
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
    {
        std::string msg(buf->retrieveAllAsString());

        LOG_INFO << "received " << msg.size() << " bytes at " << time.toString() << ", message: " << msg;
        conn->send(msg);
    }

private:
    TcpServer server_;
    EventLoop* loop_;
};

int main()
{
    Logger::setLogLevel(Logger::INFO);

    EventLoop loop;
    InetAddress listenAddr(2007);

    EchoServer server(&loop, listenAddr);
    server.start();

    LOG_INFO << "EchoServer started, listen on port 2007";
    loop.loop();
    return 0;
}
