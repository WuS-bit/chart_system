#ifndef BUFFER_H
#define BUFFER_H

#include <cstdlib>
#include <cstring>
#include "../chart_logic/protocol.h"

#define DEFAULT_SIZE 512
// 多线程下对同一个使用连接进行IO是线程不安全的，此系统下，写同一个连接的缓冲区是不安全的，最终采用将其转换到同一个线程（reactor）中进行，添加回调时使用循环CAS保护

// 因设计的不完全合理，导致从reactor线程和线程池中线程都会操作应用层缓冲区，因此需互斥处理，使用自旋锁较合理，要防止不同事务数据交替写入了缓冲区
// 接受请求不影响，因为，每个连接上的读操作都是在从Reactor线程中执行的，但写操作可能会被多线程竞争
// 最终采用类似muduo库方法将IO转移到同一个线程

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
    // 对于接收缓冲区的常用函数
    bool tryRead();
    Header * readHeader();
    void * readData(size_t len, int interface_type);

    char * readBuffer();

    char * readAllBuffer();

    size_t getRemaining(); //剩余有效数据字节数
    size_t getRemainingSize();  // 剩余可用空间

    void ensureCapacity(size_t len);
};

#endif