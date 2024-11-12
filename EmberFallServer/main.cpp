#include "pch.h"
#include "utils/Utils.h"
#include "core/Listener.h"

#include "buffer/RecvBuffer.h"
#include "Global.h"
#include "core/Client.h"
#include "core/ClientManager.h"

int main(int argc, char* argv[])
{
    std::cout << "Hello World" << std::endl;

    Listener listener;
    listener.InitializeNetwork();

    /* TODO Chat Logic */
    RecvBuffer recvBuffer;

    std::vector<std::unique_ptr<Client>>& clients = Global::cm.GetClients();

    std::vector<std::pair<BYTE, std::string>> chatLog;

    size_t checkClientExitedFrame = 100;
    size_t frame = 0;

    while (true) {
        ++frame;
        if (0 == frame % checkClientExitedFrame) {
            Global::cm.CheckNullClient();
        }

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

                    std::cout << std::format("Packet Position : ({}, {}, {})\n", playerInfo.position.x, playerInfo.position.y, playerInfo.position.z);
                }
                break;

                default:
                    recvBuffer.Read(temp, header.size - sizeof(Packet)); // 지금 당장 처리하지 않는 패킷 데이터 제거
                    break;
                }
            }
        }

        for (auto& client : clients) {
            if (client->NullClient()) {
                continue;
            }

            if (false == chatLog.empty()) {
                std::lock_guard guard{ client->GetSendMutex() };
                for (const auto& [id, str] : chatLog) {
                    client->SendChatPacket(id, str);
                }
                client->WakeSendThread();
            }
        }

        chatLog.clear();
    }

    listener.JoinAccept();
}