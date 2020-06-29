#pragma once

#include <mutex>


using LockType = std::mutex;
using LockGuard = std::lock_guard<LockType>;
using LockTypeR = std::recursive_mutex;
using LockGuardR = std::lock_guard<LockTypeR>;


