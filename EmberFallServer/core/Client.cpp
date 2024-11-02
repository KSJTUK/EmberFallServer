#include "pch.h"
#include "Client.h"

Client::Client()
    : mSocket{ INVALID_SOCKET},
    mIP{ },
    mPort{ },
    mId{ NULLID },
    mSendBuffer{ },
    mRecvBuffer{ }
{
}

Client::~Client()
{
}

Client::Client(const Client& other)
{
}

BYTE Client::GetId() const
{
    return mId;
}

std::mutex& Client::GetRecvMutex()
{
    return mRecvLock;
}

std::mutex& Client::GetSendMutex()
{
    return mSendLock;
}

bool Client::InitializeClient(SOCKET socket, BYTE id)
{
    mSocket = socket;
    sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    int clAddrLen{ sizeof(clientAddr) };
    ::getpeername(mSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clAddrLen);

    ::inet_ntop(AF_INET, &clientAddr.sin_addr, mIP.data(), INET_ADDRSTRLEN);
    mPort = ::ntohs(clientAddr.sin_port);
    mId = id;

    std::cout << std::format("클라이언트 [{}] 입장: IP: {}, Port: {}\n", id, mIP.data(), mPort);

    mSendThread = std::thread{ [=](){ SendWorker(); } };
    mRecvThread = std::thread{ [=](){ RecvWorker(); } };
}

void Client::ShutdownClient()
{
    ::shutdown(mSocket, SD_BOTH);
    ::closesocket(mSocket);

    JoinThreads();

    mSocket = INVALID_SOCKET;
    memset(mIP.data(), 0, INET_ADDRSTRLEN);
    mPort = 0;
}

void Client::JoinThreads()
{
    if (mRecvThread.joinable()) {
        mRecvThread.join();
    }

    if (mSendThread.joinable()) {
        mSendThread.join();
    }
}

void Client::WakeSendThread()
{
    mSendConditionVar.notify_one();
}

void Client::SendWorker()
{
}

void Client::RecvWorker()
{
}

void Client::ReadFromRecvBuffer()
{
}

void Client::SendChatPacket(std::string_view str)
{
}

bool Client::NullClient() const
{
    return INVALID_SOCKET == mSocket;
}
