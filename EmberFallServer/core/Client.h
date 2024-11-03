#pragma once

class Client {
    static constexpr size_t RECV_AT_ONCE = 1024;
    static constexpr size_t SEND_AT_ONCE = 512;

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
    void JoinThreads();

    void WakeSendThread();

    void SendWorker();
    void RecvWorker();

    void ReadFromRecvBuffer(class RecvBuffer& buffer);
    void SendChatPacket(BYTE senderId, std::string_view str);

    bool ExitedClient() const;
    bool NullClient() const;

private:
    SOCKET mSocket;
    std::array<char, INET_ADDRSTRLEN> mIP;
    UINT16 mPort;

    std::thread mSendThread;
    std::thread mRecvThread;

    std::mutex mRecvLock;
    std::mutex mSendLock;
    std::condition_variable mSendConditionVar;

    std::unique_ptr<class RecvBuffer> mRecvBuffer;
    std::unique_ptr<class SendBuffer> mSendBuffer;

    BYTE mId;
    std::atomic_flag mCleared;
    std::atomic_flag mEntered;
};

