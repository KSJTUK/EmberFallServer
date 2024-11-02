#pragma once
class Client {
public:
    Client();
    ~Client();

public:
    BYTE GetId() const;
    std::mutex& GetRecvMutex();
    std::mutex& GetSendMutex();

public:
    bool InitializeClient(SOCKET socket, BYTE id);
    void ShutdownClient();

    void WakeSendThread();

    void SendWorker();
    void RecvWorker();

    void ReadFromRecvBuffer();
    void SendChatPacket(std::string_view str);

private:
    SOCKET mSocket;
    std::array<char, INET_ADDRSTRLEN> mIP;
    UINT16 mPort;

    std::thread mSendThread;
    std::thread mRecvThread;

    std::mutex mRecvLock;
    std::mutex mSendLock;
    std::condition_variable mSendConditionVar;

    std::array<char, 1024> mRecvBuffer;
    std::array<char, 1024> mSendBuffer;

    BYTE mId;
};

