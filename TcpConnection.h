#pragma once

#include "noncopyable.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "Buffer.h"
#include "Timestamp.h"

#include <memory>
#include <atomic>
#include <string>
#include <any>

class Channel;
class EventLoop;
class Socket;

/*
TcpServer -> Acceptor -> 有新用户连接，获得connfd -> 打包TcpConnection 设置回调 -> Channel -> Poller -> Channel回调
*/

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *loop, const std::string &name, int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr);

    ~TcpConnection();

    EventLoop *getLoop() const { return loop_; }
    const std::string &name() const { return name_; }
    const InetAddress &localAddress() const { return localAddr_; }
    const InetAddress &peerAddress() const { return peerAddr_; }

    bool connected() const { return state_ == kConnected; }
    void send(const std::string &buf);
    void send(Buffer* message);
    // void send(const void* message, int len);

    void shutdown();

    void setConnectionCallback(const ConnectionCallback &cb)
    {
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback &cb)
    {
        messageCallback_ = cb;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    {
        writeCompleteCallback_ = cb;
    }

    void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark)
    {
        highWaterMarkCallback_ = cb;
        highWaterMark_ = highWaterMark;
    }

    void setCloseCallback(const CloseCallback &cb)
    {
        closeCallback_ = cb;
    }

    void connectEstablished();
    void connectDestoryed();

    void setContext(const std::any &context)
    {
        context_ = context;
    }

    const std::any &getContext() const
    {
        return context_;
    }

    std::any *getMutableContext()
    {
        return &context_;
    }

private:
    enum StateE
    {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
    };

    void setState(StateE state) { state_ = state; }

    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const void *data, size_t len);
    void shutdownInLoop();

    EventLoop *loop_; // 某一个suploop
    const std::string name_;
    std::atomic_int state_;
    bool reading_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    const InetAddress localAddr_;
    const InetAddress peerAddr_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;
    CloseCallback closeCallback_;

    size_t highWaterMark_;

    Buffer inputBuffer_;  // 接收数据
    Buffer outputBuffer_; // 发送数据

    std::any context_;
};