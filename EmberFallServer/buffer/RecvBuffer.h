#pragma once

class RecvBuffer {
    inline static constexpr size_t BUFFER_SIZE = 1024;
    inline static constexpr size_t BUFFER_COUNT = 10;

public:
    RecvBuffer();
    ~RecvBuffer();

    RecvBuffer(RecvBuffer&& other) noexcept;
    void operator=(RecvBuffer&& other) noexcept;

public:
    constexpr size_t GetBufferSize() const;

    /// <summary>
    /// 버퍼에서 모든 작업을 끝마친 후 버퍼를 정리하는 작업을 함.
    /// </summary>
    void Clean();

    /// <summary>
    /// 버퍼에서 readBytes만큼 데이터를 읽음.
    /// </summary>
    /// <param name="data">이 함수에서 data의 메모리 공간은 보장하지 않는다. 항상 readBytes이상의 메모리를 할당하고 함수를 호출할것</param>
    /// <returns>데이터의 크기만큼 읽읅 수 있다면 true를 리턴</returns>
    bool Read(char* data, size_t readBytes);
    /// <summary>
    /// 버퍼에 저장된 모든 데이터를 복사해서 읽음.
    /// </summary>
    /// <param name="data">이 함수에서 data의 메모리 공간은 보장하지 않는다. 항상 BufferSize 이상의 메모리를 할당하고 함수를 호출할것</param>
    /// <returns>읽은 데이터의 크기를 반환</returns>
    size_t ReadAll(char* data);
    bool Write(const char* data, size_t writeBytes);

    /// <returns>데이터가 없는 빈 공간의 크기를 반환</returns>
    size_t FreeSize() const;
    /// <returns>현재 버퍼에 쓰여진 데이터의 크기를 반환</returns>
    size_t DataSize() const;

    bool Empty() const;

private:
    size_t mReadCursor;
    size_t mWriteCursor;

    size_t mBufferSize;
    std::array<char, BUFFER_SIZE* BUFFER_COUNT> mBuffer;
};