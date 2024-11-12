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

    // Ŭ���̾�Ʈ�� ���������� �������� ���� ��� true�� ��ȯ�Ѵ�.
    bool ExitedClient() const;
    // Ŭ���̾�Ʈ�� �����߰� �����Ǿ��ٸ� true�� ��ȯ�Ѵ�.
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
    /// mCleared ������ ���� Ŭ���̾�Ʈ�� ������ �������� ���θ� �Ǻ��Ѵ�.
    /// ��, ���������� �������� �ʾҴٸ� (Thread�� ����ְų�, buffer, socket�� ����ִ°��)
    /// mEntered == false �̸鼭 mCleared == false�� �����̴�.
    /// �̰�쿡�� Ŭ���̾�Ʈ�� �����ϴ� ShutdownClient �޼ҵ带 ȣ���Ѵ�.
    /// </summary>
    std::atomic_flag mCleared;
    /// <summary>
    /// Ŭ���̾�Ʈ�� ���°� ������������� �Ǻ��Ѵ�.
    /// </summary>
    std::atomic_flag mEntered;
};

