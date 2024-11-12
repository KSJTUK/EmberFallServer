#include "pch.h"
#include "buffer/SendBuffer.h"

SendBuffer::SendBuffer()
    : mWriteCursor{ 0 },
    mBuffer{ }
{
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Clean()
{
    mWriteCursor = 0;
}

const char* SendBuffer::Buffer() const
{
    return mBuffer.data();
}

INT32 SendBuffer::DataSize() const
{
    return mWriteCursor;
}

bool SendBuffer::Write(const void* data, INT32 writeBytes)
{
    if (mBuffer.size() - mWriteCursor < writeBytes) {
        return false;
    }

    memcpy(&mBuffer[mWriteCursor], data, writeBytes);
    mWriteCursor += writeBytes;
    return true;
}

bool SendBuffer::Empty() const
{
    return 0 == mWriteCursor;
}
