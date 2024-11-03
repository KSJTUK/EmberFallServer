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
    RecvBuffer rBuffer;

    std::vector<std::unique_ptr<Client>>& clients = Global::cm.GetClients();

    std::vector<std::pair<BYTE, std::string>> chatLog;
    PacketChatting chat;

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

            client->ReadFromRecvBuffer(rBuffer);

            while (false == rBuffer.Empty()) {
                bool readSuccess = rBuffer.Read(reinterpret_cast<char*>(&chat), sizeof(PacketChatting));
                if (not readSuccess) {
                    std::cout << "read failure" << std::endl;
                    //rBuffer.Clear();
                    continue;
                }
                chatLog.push_back(std::make_pair(chat.id, chat.chatBuffer));
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