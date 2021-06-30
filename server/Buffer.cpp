#include "include/Buffer.h"

Buffer::Buffer()
{
    this->data = (char *)malloc(sizeof(char)*DEFAULT_SIZE);
    this->readIndex = this->writeIndex = 0;

}

Buffer::~Buffer()
{
    delete data;
}

size_t Buffer::getRemaining()
{
    return this->writeIndex - this->readIndex;
}

size_t Buffer::getRemainingSize()
{
    return this->size - this->writeIndex;
}

void Buffer::ensureCapacity(size_t len)
{
    if ((this->readIndex > this->size >> 2) && (this->readIndex + (this->size - this->writeIndex) > len))
    {
        // 当前buffer有效数据不到容量一半，且要添加的数据长度小于可用容量,此时只用将buffer数据进行整理
        memmove(this->data, this->data+readIndex, this->writeIndex - this->readIndex);
        this->writeIndex -= this->readIndex;
        this->readIndex = 0;

        return;
    }
    
    if (getRemainingSize() < len)
    {
        // 扩容

        size_t cap = ((writeIndex - readIndex) + len) << 1;
        char *temp = (char *)malloc(sizeof(char)*cap);

        // 字节级拷贝
        memcpy(temp, data+readIndex, writeIndex-readIndex);

        free(data);

        data = temp;

        writeIndex -= readIndex;
        readIndex = 0;
        size = cap;
    }
    return;
    
}

int Buffer::writeBuffer(size_t len, char *ptr)
{
    // 扩容机制
    ensureCapacity(len);

    // 可能添加定期减小buffer机制


    // 字节级内存拷贝库函数
    memcpy(this->data+writeIndex, ptr, len);
    // 更新当前写位置
    this->writeIndex += len;
    return len;
}



