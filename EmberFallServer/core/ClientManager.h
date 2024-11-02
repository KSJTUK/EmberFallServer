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

private:
    std::vector<Client> mClients;
};