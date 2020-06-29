#include "stdafx.h"
#include "VideoRenderWindow.h"
#include "utils.h"

PANO_UTILS_NS_USING

void VideoRenderWindow::SetWindowTitle(const std::string &title)
{
    if (IsWindow()) {
        auto wstr = utf8_decode(title);
        SetWindowText(wstr.c_str());
    }
}
