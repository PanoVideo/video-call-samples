#include "stdafx.h"

#include "UIThreadExecutorWin.h"

#define WM_RTC_UI_EXECUTOR_NOTIFY    (WM_USER + 551)


UIThreadExecutorWin::UIThreadExecutorWin()
{
    
}

UIThreadExecutorWin::~UIThreadExecutorWin()
{
    if (threadId_ != 0) {
        auto msg_loop = _Module.GetMessageLoop(threadId_);
        if (msg_loop) {
            msg_loop->RemoveMessageFilter(this);
        }
    }
}

bool UIThreadExecutorWin::init(HWND hWnd)
{
    hwndNotify_ = hWnd;
    DWORD pid = 0;
    threadId_ = ::GetWindowThreadProcessId(hWnd, &pid);
    auto msg_loop = _Module.GetMessageLoop(threadId_);
    if (msg_loop) {
        msg_loop->AddMessageFilter(this);
        return true;
    }
    return false;
}

bool UIThreadExecutorWin::inUIThread() const
{
    return ::GetCurrentThreadId() == threadId_;
}

void UIThreadExecutorWin::async(Task task)
{
    if (inUIThread()) {
        task();
    }
    else {
        {
            LockGuard g(mutex_);
            taskQueue_.push(std::move(task));
        }
        notifyUIThread();
    }
}

void UIThreadExecutorWin::sync(Task task)
{
    if (inUIThread()) {
        task();
    }
    else {
        std::mutex m;
        std::condition_variable cv;
        bool ready = false;
        Task task_sync([&] {
            task();
            std::unique_lock<std::mutex> lk(m);
            ready = true;
            cv.notify_one(); // the waiting thread may block again since m is not released
            lk.unlock();
        });
        async(std::move(task_sync));
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&ready] { return ready; });
    }
}

void UIThreadExecutorWin::post(Task task)
{
    {
        LockGuard g(mutex_);
        taskQueue_.push(std::move(task));
    }
    notifyUIThread();
}

void UIThreadExecutorWin::processTasks()
{
    std::unique_lock<LockType> g(mutex_);
    while (!taskQueue_.empty())
    {
        auto task = std::move(taskQueue_.front());
        taskQueue_.pop();
        if (task) {
            g.unlock();
            task();
            g.lock();
        }
    }
}

void UIThreadExecutorWin::notifyUIThread()
{
    if (hwndNotify_ && ::IsWindow(hwndNotify_)) {
        ::PostMessage(hwndNotify_, WM_RTC_UI_EXECUTOR_NOTIFY, 0, 0);
    }
}

BOOL UIThreadExecutorWin::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_RTC_UI_EXECUTOR_NOTIFY) {
        processTasks();
        return TRUE;
    }
    return FALSE;
}

