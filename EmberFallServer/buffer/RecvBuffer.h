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
    /// ���ۿ��� ��� �۾��� ����ģ �� ���۸� �����ϴ� �۾��� ��.
    /// </summary>
    void Clean();

    /// <summary>
    /// ���ۿ��� readBytes��ŭ �����͸� ����.
    /// </summary>
    /// <param name="data">�� �Լ����� data�� �޸� ������ �������� �ʴ´�. �׻� readBytes�̻��� �޸𸮸� �Ҵ��ϰ� �Լ��� ȣ���Ұ�</param>
    /// <returns>�������� ũ�⸸ŭ �П� �� �ִٸ� true�� ����</returns>
    bool Read(char* data, INT32 readBytes);
    /// <summary>
    /// ���ۿ� ����� ��� �����͸� �����ؼ� ����.
    /// </summary>a
    /// <param name="data">�� �Լ����� data�� �޸� ������ �������� �ʴ´�. �׻� BufferSize �̻��� �޸𸮸� �Ҵ��ϰ� �Լ��� ȣ���Ұ�</param>
    /// <returns>���� �������� ũ�⸦ ��ȯ</returns>
    INT32 ReadAll(char* data);
    bool Write(const char* data, INT32 writeBytes);

    /// <returns>�����Ͱ� ���� �� ������ ũ�⸦ ��ȯ</returns>
    INT32 FreeSize() const;
    /// <returns>���� ���ۿ� ������ �������� ũ�⸦ ��ȯ</returns>
    INT32 DataSize() const;

    bool Empty() const;

private:
    INT32 mReadCursor;
    INT32 mWriteCursor;

    INT32 mBufferSize;
    std::array<char, BUFFER_SIZE* BUFFER_COUNT> mBuffer;
};