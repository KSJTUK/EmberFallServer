#include "pch.h"
#include "buffer/RecvBuffer.h"

RecvBuffer::RecvBuffer()
    : mReadCursor{ 0 },
    mWriteCursor{ 0 },
    mBufferSize{ BUFFER_SIZE * BUFFER_COUNT },
    mBuffer{ }
{
}

RecvBuffer::~RecvBuffer()
{
}

RecvBuffer::RecvBuffer(RecvBuffer&& other) noexcept
    : mReadCursor{ other.mReadCursor },
    mWriteCursor{ other.mWriteCursor },
    mBufferSize{ BUFFER_SIZE * BUFFER_COUNT }
{
    other.ReadAll(mBuffer.data());
    other.Clean();
}

void RecvBuffer::operator=(RecvBuffer&& other) noexcept
{
    mReadCursor = other.mReadCursor;
    mWriteCursor = other.mWriteCursor;
    other.ReadAll(mBuffer.data());
    other.Clean();
}

constexpr size_t RecvBuffer::GetBufferSize() const
{
    return BUFFER_SIZE * BUFFER_COUNT;
}

void RecvBuffer::Clean()
{
    size_t dataSize = DataSize();
    if (0 == dataSize) {
        mReadCursor = 0;
        mWriteCursor = 0;
    }
    else if (FreeSize() < BUFFER_SIZE) {
        ::memcpy(&mBuffer[0], &mBuffer[mReadCursor], dataSize);
        mReadCursor = 0;
        mWriteCursor = dataSize;
    }
}

bool RecvBuffer::Read(char* data, size_t readBytes)
{
    if (DataSize() < readBytes) {
        return false;
    }

    ::memcpy(data, &mBuffer[mReadCursor], readBytes);
    mReadCursor += readBytes;

    return true;
}

size_t RecvBuffer::ReadAll(char* data)
{
    size_t dataSize = DataSize();
    ::memcpy(data, &mBuffer[mReadCursor], dataSize);
    mReadCursor += dataSize;
    Clean();
    return dataSize;
}

bool RecvBuffer::Write(const char* data, size_t writeBytes)
{
    if (FreeSize() < writeBytes) {
        return false;
    }

    ::memcpy(&mBuffer[mWriteCursor], data, writeBytes);
    mWriteCursor += writeBytes;

    return true;
}

size_t RecvBuffer::FreeSize() const
{
    return GetBufferSize() - mWriteCursor;
}

size_t RecvBuffer::DataSize() const
{
    return mWriteCursor - mReadCursor;
}

bool RecvBuffer::Empty() const
{
    return 0 == DataSize();
}
