#pragma once

class SendBuffer {
    inline static constexpr size_t BUFFER_SIZE = 1024 * 10;
public:
    SendBuffer();
    ~SendBuffer();

public:
    /// <summary>
    /// SendBuffer�� �̿��� �۾��� ��� ������ �� ȣ��
    /// ���ۿ� ������ ������ ũ�⸦ 0���� �����Ѵ�
    /// </summary>
    void Clean();

    const char* Buffer() const;

    /// <returns>���� ���ۿ� ������ �������� ũ�⸦ ��ȯ</returns>
    size_t DataSize() const;

    /// <summary>
    /// data�� �Ѿ�� �޸𸮸� writeByte��ŭ ���� ���ۿ� ����
    /// </summary>
    /// <returns>���� ���ۿ� ���� ũ�Ⱑ writeBytes ���� �۴ٸ� �������� �ʰ� false�� ����</returns>
    bool Write(const void* data, size_t writeBytes);

    /// <returns>���� ���ۿ� ������ �����Ͱ� ���ٸ� true����</returns>
    bool Empty() const;

private:
    size_t mWriteCursor;
    std::array<char, BUFFER_SIZE> mBuffer;
};