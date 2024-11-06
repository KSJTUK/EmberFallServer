#pragma once

class Client;

class ClientManager {
public:
    ClientManager();
    ~ClientManager();

public:
    bool CreateClient(SOCKET socket);
    void ShutdownClient(BYTE id);
    /// <summary>
    /// 실제로 나갔는지 여부 & 나갔음에도 정리하지 않은 클라이언트가 있다면 정리하는 코드
    /// mEntered와 mCleared플래그로 검사 및 정리한다.
    /// </summary>
    void CheckNullClient();
    Client* GetClient(BYTE id);
    std::vector<std::unique_ptr<Client>>& GetClients();

private:
    std::vector<std::unique_ptr<Client>> mClients;
};