#include "pch.h"
#include "core/ClientManager.h"
#include "core/Client.h"

ClientManager::ClientManager() 
{
    for (auto idx : std::views::iota(0, static_cast<int>(MAX_CLIENT))) {  // 클라이언트를 매번 새로 생성하지 않는다. Initailize 및 정리 작업만 진행해준다.
        mClients.emplace_back(std::make_unique<Client>());
    }
}

ClientManager::~ClientManager() { }

bool ClientManager::CreateClient(SOCKET socket)
{
    for (int id = 0; id < MAX_CLIENT; ++id) {
        if (mClients[id]->NullClient()) {  // 클라이언트가 퇴장했으며 또 정리가 된경우 -> 그자리에 새로 생성한다.
            mClients[id]->InitializeClient(socket, id);
        }
        else if (mClients[id]->ExitedClient()) {    // 클라이언트가 퇴장했으나 정리가 되지 않은 경우 -> 정리작업을 해준 후에 클라이언트를 생성한다.
            mClients[id]->ShutdownClient();
            mClients[id]->InitializeClient(socket, id);
            return true;
        }
        else {
            continue;
        }
    }
    return false;
}

void ClientManager::ShutdownClient(BYTE id)
{
    mClients[id]->ShutdownClient();
}

void ClientManager::CheckNullClient()
{
    for (int id = 0; id < MAX_CLIENT; ++id) {
        if (mClients[id]->ExitedClient()) {
            mClients[id]->ShutdownClient();
        }
    }
}

Client* ClientManager::GetClient(BYTE id)
{
    return mClients[id].get();
}

std::vector<std::unique_ptr<Client>>& ClientManager::GetClients()
{
    return mClients;
}
