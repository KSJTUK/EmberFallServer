#include "pch.h"
#include "utils/Utils.h"
#include "core/Listener.h"

#include "buffer/RecvBuffer.h"
#include "Global.h"
#include "core/Client.h"
#include "core/ClientManager.h"
#include "utils/Timer.h"

/* TODO Chat Logic */
RecvBuffer recvBuffer;

std::vector<std::unique_ptr<Client>>& clients = Global::cm.GetClients();

std::vector<std::pair<BYTE, std::string>> chatLog;
std::vector<std::pair<BYTE, DirectX::SimpleMath::Vector3>> positionLog;


void ProcessPackets()
{
    for (auto& client : clients) {
        if (client->NullClient()) {
            continue;
        }

        client->ReadFromRecvBuffer(recvBuffer);

        Packet header;
        char temp[512];
        while (false == recvBuffer.Empty()) {
            if (false == recvBuffer.Read(reinterpret_cast<char*>(&header), sizeof(Packet))) {
                abort();
            }

            switch (header.type) {
            case PT_CS_PacketChatting:
            {
                // 패킷 조립
                PacketChatting chatPacket;
                char* chat = reinterpret_cast<char*>(&chatPacket);
                recvBuffer.Read(chat + sizeof(Packet), sizeof(PacketChatting) - sizeof(Packet));
                memcpy(chat, &header, sizeof(Packet));

                chatLog.push_back(std::make_pair(header.id, chatPacket.chatBuffer));
            }
            break;

            case PT_CS_PacketPlayerInfo:
            {
                PacketPlayerInfo playerInfo;
                char* info = reinterpret_cast<char*>(&playerInfo);
                recvBuffer.Read(info + sizeof(Packet), sizeof(PacketPlayerInfo) - sizeof(Packet));
                memcpy(info, &header, sizeof(Packet));

                positionLog.push_back(std::make_pair(header.id, playerInfo.position));
            }
            break;

            default:
                recvBuffer.Read(temp, header.size - sizeof(Packet)); // 지금 당장 처리하지 않는 패킷 데이터 제거
                break;
            }
        }
    }
}

void Send()
{
    for (auto& client : clients) {
        if (client->NullClient()) {
            continue;
        }

        std::lock_guard guard{ client->GetSendMutex() };
        if (false == chatLog.empty()) {
            for (const auto& [id, str] : chatLog) {
                client->SendChatPacket(id, str);
            }
        }

        if (false == positionLog.empty()) {
            for (const auto& [id, position] : positionLog) {
                client->SendPlayerInfoPacket(id, position);
            }
        }

        client->WakeSendThread();
    }
    chatLog.clear();
    positionLog.clear();
}

int main(int argc, char* argv[])
{
    std::cout << "Hello World" << std::endl;

    Listener listener;
    listener.InitializeNetwork();

    size_t checkClientExitedFrame = 100;
    size_t frame = 0;

    size_t sendPerTime = 144;
    float sendTime = 1.0f / static_cast<float>(sendPerTime);
    float sendCounter = 0.0f;

    while (true) {
        gTimer.AdvanceTime();

        sendCounter += gTimer.GetDeltaTime();

        ++frame;
        if (0 == frame % checkClientExitedFrame) {
            Global::cm.CheckNullClient();
        }

        ProcessPackets();

        if (sendCounter > sendTime) {
            Send();
            sendCounter = 0.0f;
        }
    }

    listener.JoinAccept();
}