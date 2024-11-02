#pragma once

class Client;

class ClientManager {
public:
    ClientManager();
    ~ClientManager();

public:
    bool CreateClient(SOCKET socket);
    void ShutdownClient(BYTE id);
    Client* GetClient(BYTE id);
    std::vector<std::unique_ptr<Client>>& GetClients();

private:
    std::vector<std::unique_ptr<Client>> mClients;
};