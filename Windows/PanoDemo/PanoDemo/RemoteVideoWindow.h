#pragma once

#include "VideoRenderWindow.h"

#include <string>

class RemoteVideoWindow : public VideoRenderWindow
{
public:
    using OnDestroyObserver = std::function<void(void)>;

    void setUserInfo(uint64_t userId, std::string userName);
    void updateUserInfo(const std::string & title);

    void setOnDestroyObserver(OnDestroyObserver observer)
    {
        m_observer = std::move(observer);
    }

protected:
    LRESULT OnWindowClose() override
    {
        ShowWindow(SW_HIDE);
        if (m_observer) {
            m_observer();
        }

        return 0;
    }

protected:
    OnDestroyObserver m_observer;

    uint64_t m_userId = 0;
    std::string m_userName;
};

