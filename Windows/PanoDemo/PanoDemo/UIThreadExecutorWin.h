#pragma once

#include "TestDefs.h"
#include "UIThreadExecutor.h"

#include <atlapp.h>

#include <memory>
#include <string>
#include <functional>
#include <condition_variable>
#include <queue>


class UIThreadExecutorWin : public CMessageFilter, public UIThreadExecutor
{
public:

    UIThreadExecutorWin();
    ~UIThreadExecutorWin();

    bool init(HWND hWnd);

    void async(Task task) override;
    void sync(Task task) override;
    bool inUIThread() const override;

protected:
    // CMessageFilter
    BOOL PreTranslateMessage(MSG* pMsg) override;

protected:
    void notifyUIThread();
    void processTasks();

protected:
    HWND hwndNotify_ = NULL;
    DWORD threadId_ = 0;

    LockType mutex_;
    std::queue<Task> taskQueue_;
};

