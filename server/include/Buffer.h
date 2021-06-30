#ifndef BUFFER_H
#define BUFFER_H

#include <cstdlib>
#include <cstring>

#define DEFAULT_SIZE 512

class Buffer
{
private:
    char *data; // 数据内存指针
    size_t size; // buffer实际内存字节容量
    size_t readIndex;   // 当前读取位置
    size_t writeIndex;  // 当前写入位置
public:
    Buffer();
    ~Buffer();
    int writeBuffer(size_t len, char *ptr);
    bool tryRead();

    char * readBuffer();

    char * readAllBuffer();

    size_t getRemaining(); //剩余有效数据字节数
    size_t getRemainingSize();  // 剩余可用空间

    void ensureCapacity(size_t len);
};

#endif