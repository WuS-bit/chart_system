#ifndef CHARTONERECORD_H
#define CHARTONERECORD_H

class ChartOneRecord
{
private:
    char sender[16]; // 发送方账号
    char recver[16]; // 接收方账号
    char msg[1024]; // 消息

public:
    ChartOneRecord(/* args */);
    ~ChartOneRecord();
};

ChartOneRecord::ChartOneRecord(/* args */)
{
}

ChartOneRecord::~ChartOneRecord()
{
}


#endif