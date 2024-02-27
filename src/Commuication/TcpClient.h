#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <functional>
#include "MThread.h"

class TcpClient : public MThread
{
public:
    TcpClient(const std::string &server_url, const uint16_t server_port);
    ~TcpClient();

    int Init();
    void Start();
    void SetMessageCallback(std::function<void (std::string &message)> callback);
    int SendData(const char *data, const uint16_t len);
    
    virtual void run() override;

private:
    std::string m_server_url;
    uint16_t m_server_port;
    SOCKET m_clientSocket;

    std::function<void (std::string &message)> OnMessageCallback;
    bool IsReceiveAble(timeval *tv);
    void DisClient();
};
#endif