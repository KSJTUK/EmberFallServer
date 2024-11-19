#include "pch.h"
#include "core/Client.h"
#include "utils/Utils.h"
#include "buffer/RecvBuffer.h"
#include "buffer/SendBuffer.h"

constexpr char ShutdownSentence[]{ "shutdown thread" };
constexpr size_t ShutdownSentenceLen{ _countof(ShutdownSentence) };

Client::Client()
    : mSocket{ INVALID_SOCKET},
    mIP{ },
    mPort{ },
    mId{ NULLID },
    mSendBuffer{ },
    mRecvBuffer{ }
{
    mCleared.test_and_set();
}

Client::~Client()
{
}

BYTE Client::GetId() const
{
    return mId;
}

std::mutex& Client::GetRecvMutex()
{
    return mRecvLock;
}

std::mutex& Client::GetSendMutex()
{
    return mSendLock;
}

bool Client::InitializeClient(SOCKET socket, BYTE id)
{
    if (INVALID_SOCKET == socket) {
       return false;
    }

    mSocket = socket;
    sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    int clAddrLen{ sizeof(clientAddr) };
    ::getpeername(mSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clAddrLen);

    ::inet_ntop(AF_INET, &clientAddr.sin_addr, mIP.data(), INET_ADDRSTRLEN);
    mPort = ::ntohs(clientAddr.sin_port);
    mId = id;

    // 클라이언트에게 ID 통지
    int sendIdResult = ::send(mSocket, reinterpret_cast<char*>(&mId), 1, 0);

    std::cout << std::format("클라이언트 [{}] 입장: IP: {}, Port: {}\n", id, mIP.data(), mPort);

    mRecvBuffer = std::make_unique<RecvBuffer>();
    mSendBuffer = std::make_unique<SendBuffer>();

    mSendThread = std::thread{ [=](){ SendWorker(); } };
    mRecvThread = std::thread{ [=](){ RecvWorker(); } };

    mEntered.test_and_set();
    mCleared.clear();

    return true;
}

void Client::ShutdownClient()
{
    // 이미 초기화 된 상태라면 다시 초기화를 진행하지 않는다.
    if (true == mCleared.test_and_set()) { // std::atomic_flag 의 test_and_set 은 성공하는 경우 이전의 플래그 값을 반환한다.
        return;
    }

    ::shutdown(mSocket, SD_BOTH);
    ::closesocket(mSocket);

    JoinThreads();

    mRecvBuffer.reset();
    mSendBuffer.reset();

    mSocket = INVALID_SOCKET;
    memset(mIP.data(), 0, INET_ADDRSTRLEN);
    mPort = 0;
} 

void Client::JoinThreads()
{
    if (mRecvThread.joinable()) {
        mRecvThread.join();
    }

    if (mSendThread.joinable()) {
        if (mSendBuffer) {
            mSendBuffer->Write(ShutdownSentence, ShutdownSentenceLen);
            mSendConditionVar.notify_all();
        }
        mSendThread.join();
    }

    std::cout << std::format("클라이언트 [{}] 퇴장: IP: {}, Port: {}\n", mId, mIP.data(), mPort);
}

void Client::WakeSendThread()
{
    mSendConditionVar.notify_one();
}

void Client::SendWorker()
{
    int sendResult = 0;
    while (true) {
        std::unique_lock lock{ mSendLock };
        mSendConditionVar.wait(lock, [=]() { return false == mSendBuffer->Empty(); });

        /* TODO send 기능 작성 */
        int dataSize = mSendBuffer->DataSize();
#ifdef NETWORK_DEBUG
        std::cout << "SendThread WakeUp DataSize is : " << dataSize << std::endl;
#endif
        while (true) {
            if (dataSize < SEND_AT_ONCE) {
                sendResult = ::send(mSocket, mSendBuffer->Buffer(), dataSize, 0);
                dataSize = 0;
                break;
            }

            sendResult = ::send(mSocket, mSendBuffer->Buffer(), SEND_AT_ONCE, 0);
            dataSize -= SEND_AT_ONCE;
        }

        if (sendResult < 0) {
            break;
        }

        mSendBuffer->Clean();
    }

    mEntered.clear();
}

void Client::RecvWorker()
{
    int recvLen = 0;

    RBuffer buffer;
    ByteArray<MAX_ONE_PACKET_SIZE> remainBuffer;

    auto bufferBegin = buffer.begin();
    auto bufferDataPos = buffer.begin();
    auto remainBegin = remainBuffer.begin();
    auto remainPos = remainBuffer.begin();
    ptrdiff_t remainLen = 0;

    while (true) {
        bufferBegin = buffer.begin();
        recvLen = ::recv(mSocket, DataAddress(bufferBegin), RECV_AT_ONCE, 0);
        if (recvLen < 0) {
#ifdef NETWORK_DEBUG
            ErrorHandle::WSAErrorMessageBox("recv function failure");
#endif
            break;
        }
        else if (recvLen == 0) {
            break;
        }

        bufferDataPos = bufferBegin + recvLen;

#ifdef NETWORK_DEBUG
        std::cout << "RecvLen: " << recvLen << std::endl;
#endif

        remainLen = std::distance(remainBuffer.begin(), remainPos);
#ifdef NETWORK_DEBUG
        std::cout << "RemainLen: " << remainLen << std::endl;
#endif
        // 이전에 남은 데이터가 있을때 다시 검사하는 과정
        if (remainLen != 0) {
            bufferDataPos = std::move(bufferBegin, bufferDataPos, bufferBegin + remainLen);
            std::copy(remainBegin, remainPos, bufferBegin);
            remainPos = remainBuffer.begin();
        }

        //// 새로 받아온 데이터에 대해서 검사하는 과정
        auto remainDataBegin = ValiatePackets(bufferBegin, bufferDataPos);
        if (remainDataBegin != bufferBegin) {
            remainPos = std::copy(remainDataBegin, bufferDataPos, remainBegin);
            bufferDataPos = remainDataBegin;
        }

        std::lock_guard lock{ mRecvLock };
        mRecvBuffer->Write(DataAddress(bufferBegin), std::distance(bufferBegin, bufferDataPos));
    }

    mEntered.clear();
}

void Client::ReadFromRecvBuffer(RecvBuffer& buffer)
{
    std::lock_guard guard{ mRecvLock };
    buffer = std::move(*mRecvBuffer);
}

void Client::SendChatPacket(BYTE senderId, std::string_view str)
{
    PacketChatting chat{ sizeof(PacketChatting), PT_SC_PacketChatting, senderId, { } };
    if (str.size() > CHAT_PACKET_MAX_SIZE) {
        ErrorHandle::CommonErrorMessageBox("ChatPacketSize Over MaxSize", "...");
        return;
    }
    memcpy(chat.chatBuffer, str.data(), str.size());

    std::cout << std::format("Send id: {}, contents: {}\n", chat.id, chat.chatBuffer);
    mSendBuffer->Write(&chat, chat.size);
}

void Client::SendPlayerInfoPacket(BYTE senderId, const DirectX::SimpleMath::Vector3& position)
{
    PacketPlayerInfo playerInfo{ sizeof(PacketPlayerInfo), PT_SC_PacketPlayerInfo, senderId, position };

    mSendBuffer->Write(&playerInfo, playerInfo.size);
}

bool Client::ExitedClient() const
{
    return not mEntered.test() and not mCleared.test();
}

bool Client::NullClient() const
{
    return not mEntered.test() and mCleared.test();
}

// 수정 필: RBuffer::end를 호출하면 DataLen과 다름
Client::RBuffer::iterator Client::ValiatePackets(const RBuffer::iterator& dataBegin, const RBuffer::iterator& dataEnd)
{
    auto it = dataBegin;
    while (it != dataEnd) {
        if (std::distance(it, dataEnd) < *it) {
            break;
        }
        it += *it;
    }

    return it;
}
