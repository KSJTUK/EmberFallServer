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
    void SendPlayerInfoPacket(BYTE senderId, const DirectX::SimpleMath::Vector3& position);

    // 클라이언트가 퇴장했으나 정리되지 않은 경우 true를 반환한다.
    bool ExitedClient() const;
    // 클라이언트가 퇴장했고 정리되었다면 true를 반환한다.
    bool NullClient() const;

    size_t CheckPackets(char* buffer, size_t len);

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
    /// <summary>
    /// mCleared 변수는 현재 클라이언트가 정리된 상태인지 여부를 판별한다.
    /// 즉, 나갔음에도 정리되지 않았다면 (Thread가 살아있거나, buffer, socket이 살아있는경우)
    /// mEntered == false 이면서 mCleared == false인 상태이다.
    /// 이경우에는 클라이언트를 정리하는 ShutdownClient 메소드를 호출한다.
    /// </summary>
    std::atomic_flag mCleared;
    /// <summary>
    /// 클라이언트의 상태가 퇴장상태인지를 판별한다.
    /// </summary>
    std::atomic_flag mEntered;
};

