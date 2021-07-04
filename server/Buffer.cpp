#include "include/net/Buffer.h"

Buffer::Buffer()
{
    this->data = (char *)malloc(sizeof(char)*DEFAULT_SIZE);
    this->readIndex = this->writeIndex = 0;

}

Buffer::~Buffer()
{
    delete data;
}


// 下面三个函数都与定义的通信协议紧耦合，因为需要按照协议读取
bool Buffer::tryRead()
{
    // 先尝试获取协议头
    size_t header_length = sizeof(Header);
    if (getRemaining() >= header_length)
    {
        Header *header = (Header *)malloc(header_length);
        // 字节级拷贝
        memcpy((char*)header, data+readIndex, header_length);

        // 携带数据长度
        long long data_length = header->length;
        // 是否形成一次完整的协议报文
        if (getRemaining() >= data_length+header_length)
        {
            return true;
        }
        else
        {
            return false;
        }
        
    }
    
}

Header * Buffer::readHeader()
{
    size_t len = sizeof(Header);

    if (getRemaining() >= len)
    {
        Header *header = (Header *)malloc(len);
        // 字节级拷贝
        memcpy((char*)header, data+readIndex, len);

        // 修改接收buffer的读指针
        readIndex += len;

        return header;
    }
    else
    {
        return NULL;
    }
}

void * Buffer::readData(size_t len, int interface_type)
{
    if (getRemaining() >= len)
    {
        void * ptr = NULL;
        // 针对特定接口的请求参数数据
        switch (interface_type)
        {
        case USER_REGISTER:
        {
            REGISTER *data = (REGISTER *)malloc(len);
            memcpy((char *)data, this->data+readIndex, len);

            readIndex+=len;
            ptr = (void *)data;
        }
        break;
        case USER_LOGIN:
        {
            LOGIN *data = (LOGIN *)malloc(len);
            memcpy((char *)data, this->data+readIndex, len);

            readIndex+=len;
            ptr = (void *)data;
        }
        break;
        case USER_GET_FRIEND_LIST:
        {
            GET_FRIEND_LIST *data = (GET_FRIEND_LIST *)malloc(len);
            memcpy((char *)data, this->data+readIndex, len);

            readIndex+=len;
            ptr = (void *)data;
        }
        break;
        case CHART_ONE_INTERFACE:
        {
            CHART_ONE *data = (CHART_ONE *)malloc(len);
            memcpy((char *)data, this->data+readIndex, len);

            readIndex+=len;
            ptr = (void *)data;
        }
        break;
        case USER_GET_CHART_RECORD:
        {
            GET_CHART_RECORD *data = (GET_CHART_RECORD *)malloc(len);
            memcpy((char *)data, this->data+readIndex, len);

            readIndex+=len;
            ptr = (void *)data;
        }
        break;
        case CHART_GROUP_INTERFACE:
        {
            CHART_GROUP *data = (CHART_GROUP *)malloc(len);
            memcpy((char *)data, this->data+readIndex, len);

            readIndex+=len;
            ptr = (void *)data;
        }
        break;
        case USER_GET_GROUP_RECORD:
        {
            
        }
        break;
        case USER_LOGOUT:
        {
            LOGOUT *data = (LOGOUT *)malloc(len);
            memcpy((char *)data, this->data+readIndex, len);

            readIndex+=len;
            ptr = (void *)data;
        }
        break;
        default:
            break;
        }
        return ptr;
    }
    return NULL;
    
}

// 下面函数与发送数据有关
char * Buffer::readAllBuffer()
{
    size_t len = getRemaining();
    char *buf = (char *)malloc(len);

    memcpy(buf, data+readIndex, len);
    readIndex += len;
    return buf;
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

