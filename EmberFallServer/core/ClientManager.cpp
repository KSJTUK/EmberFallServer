#include "pch.h"
#include "core/ClientManager.h"
#include "core/Client.h"

ClientManager::ClientManager() 
{
    for (auto idx : std::views::iota(0, static_cast<int>(MAX_CLIENT))) {  // Ŭ���̾�Ʈ�� �Ź� ���� �������� �ʴ´�. Initailize �� ���� �۾��� �������ش�.
        mClients.emplace_back(std::make_unique<Client>());
    }
}

ClientManager::~ClientManager() { }

bool ClientManager::CreateClient(SOCKET socket)
{
    for (int id = 0; id < MAX_CLIENT; ++id) {
        if (mClients[id]->NullClient()) {  // Ŭ���̾�Ʈ�� ���������� �� ������ �Ȱ�� -> ���ڸ��� ���� �����Ѵ�.
            mClients[id]->InitializeClient(socket, id);
        }
        else if (mClients[id]->ExitedClient()) {    // Ŭ���̾�Ʈ�� ���������� ������ ���� ���� ��� -> �����۾��� ���� �Ŀ� Ŭ���̾�Ʈ�� �����Ѵ�.
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
