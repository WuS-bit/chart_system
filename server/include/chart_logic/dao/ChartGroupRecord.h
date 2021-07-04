#ifndef CHARTGROUPRECORD_H
#define CHARTGROUPRECORD_H

#include <cstring>

using namespace std;

class ChartGroupRecord
{
private:
    char sender[16]; // 发送方账号
    char msg[1024]; // 消息

public:
    ChartGroupRecord(/* args */);
    ~ChartGroupRecord();
    void setContent(const char *, const char *);
};

inline ChartGroupRecord::ChartGroupRecord(/* args */)
{
}

inline ChartGroupRecord::~ChartGroupRecord()
{
}

inline void ChartGroupRecord::setContent(const char * sender, const char *msg)
{
    strcpy(this->sender, sender);
    strcpy(this->msg, msg);
}

#endif