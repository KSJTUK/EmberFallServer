#pragma once

class SendBuffer {
    inline static constexpr size_t BUFFER_SIZE = 1024 * 10;
public:
    SendBuffer();
    ~SendBuffer();

public:
    /// <summary>
    /// SendBuffer를 이용한 작업이 모두 끝났을 때 호출
    /// 버퍼에 쓰여진 데이터 크기를 0으로 설정한다
    /// </summary>
    void Clean();

    const char* Buffer() const;

    /// <returns>현재 버퍼에 쓰여진 데이터의 크기를 반환</returns>
    size_t DataSize() const;

    /// <summary>
    /// data에 넘어온 메모리를 writeByte만큼 내부 버퍼에 복사
    /// </summary>
    /// <returns>현재 버퍼에 남은 크기가 writeBytes 보다 작다면 복사하지 않고 false를 리턴</returns>
    bool Write(const void* data, size_t writeBytes);

    /// <returns>현재 버퍼에 쓰여진 데이터가 없다면 true리턴</returns>
    bool Empty() const;

private:
    size_t mWriteCursor;
    std::array<char, BUFFER_SIZE> mBuffer;
};