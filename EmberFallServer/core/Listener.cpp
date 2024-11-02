#include "pch.h"
#include "core/Listener.h"
#include "utils/Utils.h"
#include "utils/Constants.h"
#include "Global.h"

#include "core/ClientManager.h"

Listener::Listener()
    : mListenSocket{ INVALID_SOCKET },
    mAcceptThread{ }
{
}

Listener::~Listener()
{
    // JoinAccept 함수를 호출하지 않거나 모종의 이유로 socket이 종료되지 않는경우 실행
    if (INVALID_SOCKET != mListenSocket) {
        ::closesocket(mListenSocket);
        ::WSACleanup();
    }
}

void Listener::InitializeNetwork()
{
    WSAData wsaData;
    if (0 != ::WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        ErrorHandle::WSAErrorMessageBoxExit("WSAStartup failure");
    }

    mListenSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == mListenSocket) {
        ErrorHandle::WSAErrorMessageBoxExit("socket creation failure");
    }

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = ::htons(Constant::PORT);
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);

    if (SOCKET_ERROR == ::bind(mListenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr))) {
        ErrorHandle::WSAErrorMessageBoxExit("bind function failure");
    }

    if (SOCKET_ERROR == ::listen(mListenSocket, SOMAXCONN)) {
        ErrorHandle::WSAErrorMessageBoxExit("listen function failure");
    }

    mAcceptThread = std::thread{ [=]() { AcceptWorker(); } };
}

void Listener::AcceptWorker()
{
    sockaddr_in clientAddr;
    int addrLen{ sizeof(clientAddr) };

    while (true) {
        memset(&clientAddr, 0, sizeof(clientAddr));
        SOCKET clientSocket = ::accept(mListenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
        if (INVALID_SOCKET == clientSocket) {
            ErrorHandle::WSAErrorMessageBox("client accept failure");
            break;
        }
        
        char ipaddr[INET_ADDRSTRLEN]{ };
        UINT16 port;
        ::inet_ntop(AF_INET, &clientAddr.sin_addr, ipaddr, INET_ADDRSTRLEN);
        port = ::ntohs(clientAddr.sin_port);

        bool createSuccess = Global::cm.CreateClient(clientSocket);
        if (not createSuccess) {
            ::shutdown(clientSocket, SD_BOTH);
            ::closesocket(clientSocket);
        }
    }
}

void Listener::ShutdownAcceptWorker()
{
}

void Listener::JoinAccept()
{
    if (mAcceptThread.joinable()) {
        mAcceptThread.join();
    }

    ::closesocket(mListenSocket);
    mListenSocket = INVALID_SOCKET;
    ::WSACleanup();
}
