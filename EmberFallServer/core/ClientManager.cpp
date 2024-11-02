#include "pch.h"
#include "core/ClientManager.h"
#include "core/Client.h"

ClientManager::ClientManager() 
{
    mClients.resize(MAX_CLIENT);
}

ClientManager::~ClientManager() { }

bool ClientManager::CreateClient(SOCKET socket)
{
    for (int id = 0; id < MAX_CLIENT; ++id) {
        if (mClients[id].NullClient()) {
            mClients[id].InitializeClient(socket, id);
            return true;
        }
    }
    return false;
}

void ClientManager::ShutdownClient(BYTE id)
{
    mClients[id].ShutdownClient();
}

Client* ClientManager::GetClient(BYTE id)
{
    return &mClients[id];
}
