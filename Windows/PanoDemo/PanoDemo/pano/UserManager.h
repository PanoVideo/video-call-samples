#pragma once

#include "TestDefs.h"
#include "UserInfo.h"

#include <unordered_map>


class UserManager
{
public:
    UserManager();

    void addUser(uint64_t userId, UserInfo::Ptr u);
    void removeUser(uint64_t userId);
    UserInfo::Ptr getUser(uint64_t userId);
    void clearAllUsers();

protected:
    std::unordered_map<uint64_t, UserInfo::Ptr> users_;
    LockType mutex_;
};

