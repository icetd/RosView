
#include "TcpClient.h"
#include "log.h"

TcpClient::TcpClient(const std::string &server_url, const uint16_t server_port) :
    m_server_url(server_url),
    m_server_port(server_port)
{
}

TcpClient::~TcpClient()
{
}

void TcpClient::Start()
{
    this->start();
    this->detach();
}

int TcpClient::Init()
{   
    WSADATA wsadata;
    int re = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (re != 0) {
        LOG(ERRO, "WIN32 create socket failed");
        WSACleanup();
        return -1;
    }

    m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_clientSocket == INVALID_SOCKET) {
        LOG(ERRO, "Create socket failed.");
        WSACleanup();
        return -1;
    }

    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(m_server_port);
    client_addr.sin_addr.S_un.S_addr = inet_addr(m_server_url.c_str());


    if (connect(m_clientSocket, (PSOCKADDR)&client_addr, sizeof(sockaddr_in))== SOCKET_ERROR) {
        closesocket(m_clientSocket);
        WSACleanup();
        LOG(ERRO, "client to RosPowerControl failed.");
        return -1;
    }

    LOG(NOTICE, "client to RosPowerConteol success socket id:%d.", m_clientSocket);
    return 0;
}

void TcpClient::SetMessageCallback(std::function<void (std::string &message)> callback)
{
    OnMessageCallback = std::move(callback);
}

int TcpClient::SendData(const char *data, const uint16_t len)
{
    int ret = send(m_clientSocket, data, len, 0);
    if (ret == SOCKET_ERROR) {
        LOG(ERRO, "send to powercontrol failed %d", WSAGetLastError());
        return -1;
    }
    return 0;
}

void TcpClient::run()
{
    char buf[128];
    timeval tv;

    while(!this->isStoped()) {
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int ret = IsReceiveAble(&tv);


        if (ret == 1) {
            int size = recv(m_clientSocket, buf, 128, 0);
            if (size > 0) {
                std::string message;
                buf[size + 1] = '\0';
                message = buf;
                try {
                    OnMessageCallback(message);
                } catch (char *message) {
                }

            } else if (ret == 0) {
                LOG(WARN, "tcp client closed.");
                break;
            } else {
                LOG(ERRO, "tcp client receive failed.");
                break;
            }
        } else if (ret == -1) {
            LOG(NOTICE, "stop %s", strerror(errno));
            this->stop();
            return;
        }
    }
    DisClient();
}

bool TcpClient::IsReceiveAble(timeval * tv)
{
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(m_clientSocket, &fdset);
    return(select(m_clientSocket + 1, &fdset, NULL, NULL, tv) == 1);
}

void TcpClient::DisClient()
{
    int ret = 0;
    ret = closesocket(m_clientSocket);
    if (ret == 0)
        LOG(INFO, "close socket successed.");
    else 
        LOG(ERRO, "close socket falied");
    WSACleanup();
}

