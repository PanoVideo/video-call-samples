#pragma once

#include "PanoRtcDevice.h"
#include "PanoRtcEngine.h"
#include "utils.h"

#include <memory>
#include <string>
#include <thread>


class ExternalVideoCapturer
{
public:
    ExternalVideoCapturer(panortc::RtcExternalVideoCapturer *rtcCapturer);
    ~ExternalVideoCapturer();

    bool start(std::wstring yuvFile, int width, int height, int fps);
    void stop();


protected:
    pano::utils::DUniquePtr<panortc::RtcExternalVideoCapturer> exCapturer_;
    bool stopped_ = true;
    std::thread exThread_;
};

