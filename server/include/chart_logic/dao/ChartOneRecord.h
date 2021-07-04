#ifndef CHARTONERECORD_H
#define CHARTONERECORD_H

#include <cstring>

using namespace std;

class ChartOneRecord
{
private:
    char sender[16]; // 发送方账号
    char recver[16]; // 接收方账号
    char msg[1024]; // 消息

public:
    ChartOneRecord(/* args */);
    ~ChartOneRecord();
    void setContent(const char *, const char *, const char*);
};

inline ChartOneRecord::ChartOneRecord(/* args */)
{
}

inline ChartOneRecord::~ChartOneRecord()
{
}

inline void ChartOneRecord::setContent(const char *sender, const char *recver, const char *msg)
{
    strcpy(this->sender, sender);
    strcpy(this->recver, recver);
    strcpy(this->msg, msg);
}


#endif