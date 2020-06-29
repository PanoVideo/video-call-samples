#include "stdafx.h"
#include "RemoteVideoWindow.h"


void RemoteVideoWindow::setUserInfo(uint64_t userId, std::string userName)
{
    m_userId = userId;
    m_userName = std::move(userName);
    auto userInfo = m_userName + " [" + std::to_string(m_userId) + "]";
    SetWindowTitle(userInfo);
}
