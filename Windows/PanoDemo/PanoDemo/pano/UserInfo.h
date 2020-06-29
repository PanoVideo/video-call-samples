#pragma once

#include "PanoRtcDevice.h"

#include <memory>
#include <string>

struct VideoStats
{
    int width = 0;
    int height = 0;
    int fps = 0;
    size_t bitrate = 0;
    float loss = 0.f;
};

class UserInfo
{
public:
    using Ptr = std::shared_ptr<UserInfo>;

    UserInfo(uint64_t userId);
    ~UserInfo();

    uint64_t getUserId() const { return userId_; }

    void setMaxVideoProfile(panortc::VideoProfileType maxProfile) { maxProfile_ = maxProfile; }
    panortc::VideoProfileType getMaxVideoProfile() const { return maxProfile_; }

public:
    bool audioSubscribed_ = false;
    bool videoSubscribed_ = false;
    VideoStats videoStats_;
    VideoStats screenStats_;

protected:
    uint64_t    userId_ = 0;
    std::string userName_;
    panortc::VideoProfileType maxProfile_ = panortc::VideoProfileType::Lowest;
};

