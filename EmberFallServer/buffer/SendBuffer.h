#pragma once

class SendBuffer {
    inline static constexpr size_t BUFFER_SIZE = 4096;
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
    INT32 DataSize() const;

    /// <summary>
    /// data�� �Ѿ�� �޸𸮸� writeByte��ŭ ���� ���ۿ� ����
    /// </summary>
    /// <returns>���� ���ۿ� ���� ũ�Ⱑ writeBytes ���� �۴ٸ� �������� �ʰ� false�� ����</returns>
    bool Write(void* data, INT32 writeBytes);

    /// <returns>���� ���ۿ� ������ �����Ͱ� ���ٸ� true����</returns>
    bool Empty() const;

private:
    INT32 mWriteCursor;
    std::array<char, BUFFER_SIZE> mBuffer;
};