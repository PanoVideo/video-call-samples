
#include "UserManager.h"


UserManager::UserManager()
{

}

void UserManager::addUser(uint64_t userId, UserInfo::Ptr u)
{
    LockGuard g(mutex_);
    users_[userId] = std::move(u);
}

void UserManager::removeUser(uint64_t userId)
{
    LockGuard g(mutex_);
    users_.erase(userId);
}

UserInfo::Ptr UserManager::getUser(uint64_t userId)
{
    LockGuard g(mutex_);
    auto it = users_.find(userId);
    if (it != users_.end()) {
        return it->second;
    }

    return nullptr;
}

void UserManager::clearAllUsers()
{
    LockGuard g(mutex_);
    users_.clear();
}
