#pragma once

#include <functional>


class UIThreadExecutor
{
public:
    using Task = std::function<void(void)>;

    virtual ~UIThreadExecutor() {}

    virtual void async(Task task) = 0;
    virtual void sync(Task task) = 0;
    virtual bool inUIThread() const = 0;
};

