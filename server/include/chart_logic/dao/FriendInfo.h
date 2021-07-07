#ifndef FRIENDINFO_H
#define FRIENDINFO_H

#include <cstring>

using namespace std;

class FriendInfo
{
public:
    char account[16];
    char username[32];
public:
    FriendInfo();
    ~FriendInfo();
    void setAccount(const char *ptr);
};

inline FriendInfo::FriendInfo()
{
}

inline FriendInfo::~FriendInfo()
{
}

inline void FriendInfo::setAccount(const char *ptr)
{
    strcpy(this->account, ptr);
    strcpy(this->username, ptr);
}


#endif