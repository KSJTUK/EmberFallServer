#pragma once

class Listener {
public:
    Listener();
    ~Listener();

public:
    void InitializeNetwork();
    void AcceptWorker();
    void ShutdownAcceptWorker();
    void JoinAccept();

private:
    SOCKET mListenSocket;
    std::thread mAcceptThread;
};