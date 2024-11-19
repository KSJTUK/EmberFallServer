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
    bool Read(char* data, size_t readBytes);
    /// <summary>
    /// ���ۿ� ����� ��� �����͸� �����ؼ� ����.
    /// </summary>
    /// <param name="data">�� �Լ����� data�� �޸� ������ �������� �ʴ´�. �׻� BufferSize �̻��� �޸𸮸� �Ҵ��ϰ� �Լ��� ȣ���Ұ�</param>
    /// <returns>���� �������� ũ�⸦ ��ȯ</returns>
    size_t ReadAll(char* data);
    bool Write(const char* data, size_t writeBytes);

    /// <returns>�����Ͱ� ���� �� ������ ũ�⸦ ��ȯ</returns>
    size_t FreeSize() const;
    /// <returns>���� ���ۿ� ������ �������� ũ�⸦ ��ȯ</returns>
    size_t DataSize() const;

    bool Empty() const;

private:
    size_t mReadCursor;
    size_t mWriteCursor;

    size_t mBufferSize;
    std::array<char, BUFFER_SIZE* BUFFER_COUNT> mBuffer;
};