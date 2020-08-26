
#include "ExternalVideoCapturer.h"

#include <fstream>
#include <vector>
#include <chrono>
#include <Windows.h>
#include <sstream>

using namespace std::chrono;
PANORTC_NS_USING

ExternalVideoCapturer::ExternalVideoCapturer(panortc::RtcExternalVideoCapturer *rtcCapturer)
    : exCapturer_(rtcCapturer)
{

}

ExternalVideoCapturer::~ExternalVideoCapturer()
{
    stop();
}

bool ExternalVideoCapturer::start(std::wstring yuvFile, int width, int height, int fps)
{
    if (fps <= 0) fps = 20;
    stopped_ = false;
    exThread_ = std::thread([=, yuv=std::move(yuvFile)] {
        std::ifstream f(yuv, std::ios::binary | std::ios::in);
        if (!f.is_open()) {
            return;
        }
        MediaFormat format;
        format.mediaType = MediaType::Video;
        format.videoFmt.type = VideoType::I420;
        format.videoFmt.width = width;
        format.videoFmt.height = height;
        format.videoFmt.rotation = VideoRotation::Rotation_0;
        format.videoFmt.count = 3;
        format.videoFmt.offset[0] = 0;
        format.videoFmt.offset[1] = width * height;
        format.videoFmt.offset[2] = format.videoFmt.offset[1] + width * height / 4;
        format.videoFmt.stride[0] = width;
        format.videoFmt.stride[1] = width / 2;
        format.videoFmt.stride[2] = width / 2;
        int interval = (int)std::ceil(1000.f / fps);
        std::streamsize sz = width * height * 3 / 2;
        std::vector<char> buf(sz);
        while (!stopped_ && f.good())
        {
            f.read(&buf[0], sz);
            if (!f) {
                if (f.eof()) {
                    f.clear();
                    f.seekg(0, std::ios::beg);
                    f.read(&buf[0], sz);
                }
                auto nfsize = f.gcount();
                if (nfsize < sz) {
                    break; // error
                }
            }
            exCapturer_->putData(&buf[0], (int)sz, format);
            std::this_thread::sleep_for(milliseconds(interval));
        }
        f.close();
    });
    return true;
}

void ExternalVideoCapturer::stop()
{
    stopped_ = true;
    try
    {
        if (exThread_.joinable()) {
            exThread_.join();
        }
    }
    catch (const std::exception&)
    {

    }
}

