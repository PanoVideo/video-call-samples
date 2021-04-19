#include "stdafx.h"
#include "WBViewController.h"
#include "WhiteboardWindow.h"
#include "RtcTester.h"
#include "PanoRtcEngine.h"


using namespace pano::utils;
using namespace panortc;

const int kDefaultVideoViewWidth = 1200;
const int kDefaultVideoViewHeight = 800;


WBViewController::WBViewController()
{
    whiteboardView_ = std::make_unique<WhiteboardWindow>(this);
    whiteboardView_->setOnDestroyObserver([this] {
        stopWhiteboard();
    });
}

WBViewController::WBViewController(const char *whiteboardId)
: whiteboardId_(whiteboardId)
{
    whiteboardView_ = std::make_unique<WhiteboardWindow>(this);
    whiteboardView_->setOnDestroyObserver([this] {
        stopWhiteboard();
    });
}

WBViewController::~WBViewController() = default;

void WBViewController::startWhiteboard()
{
    RtcTester::instance().switchWhiteboard(whiteboardId_.c_str());
    if (!whiteboardView_->IsWindow()) {
        RECT rc{ 0, 0, kDefaultVideoViewWidth, kDefaultVideoViewHeight };
        auto wstr = utf8_decode(std::string("Whiteboard ") + whiteboardId_);
        LPCTSTR windowName = wstr.c_str();
        whiteboardView_->Create(NULL, rc, windowName, WS_OVERLAPPEDWINDOW);
    }
    whiteboardView_->ShowWindow(SW_SHOW);
    whiteboardView_->UpdateWindow();
    HWND hWnd = whiteboardView_->getWhiteboardView();

    RtcTester::instance().startWhiteboard(hWnd, this);
    whiteboardStarted_ = true;
}

void WBViewController::stopWhiteboard()
{
    if (whiteboardStarted_) {
        RtcTester::instance().switchWhiteboard(whiteboardId_.c_str());
        RtcTester::instance().stopWhiteboard();
    }
    if (whiteboardView_->IsWindow()) {
        whiteboardView_->ShowWindow(SW_HIDE);
    }
    whiteboardStarted_ = false;
}

void WBViewController::close()
{
    whiteboardView_->close();
}

void WBViewController::onPageInfo(int &currentPage, int &totalPage)
{
    RtcTester::instance().getPageInfo(currentPage, totalPage);
}

void WBViewController::onScrollView(int dx, int dy)
{
    RtcTester::instance().scrollView(dx, dy);
}

void WBViewController::refreshDocs()
{
    RtcTester::instance().enumDocs([this](const char *fileId) {
        if (whiteboardView_) {
            const char *name = RtcTester::instance().getDocName(fileId);
            whiteboardView_->addDoc(fileId, name);
        }
    });

    const char* fileId = RtcTester::instance().getCurrentDoc();
    if (whiteboardView_) {
        whiteboardView_->switchDoc(fileId);
        const char *name = RtcTester::instance().getDocName(fileId);
        if (sizeof(TCHAR) == sizeof(char)) {
            whiteboardView_->onDocSwitch(true, (LPCTSTR)name);
        }
        else {
            auto wname = utf8_decode(name);
            whiteboardView_->onDocSwitch(true, wname.c_str());
        }
        }
    }

void WBViewController::addStampResource(const char* stampId, const char* path, bool resizable)
{
    RtcTester::instance().addStampResource(stampId, path, resizable);
}

void WBViewController::onSwitchWhiteboard()
{
    RtcTester::instance().switchWhiteboard(whiteboardId_.c_str());
}

void WBViewController::onToolSelect()
{
    RtcTester::instance().setWbToolType(WBToolType::SELECT);
}

void WBViewController::onToolText()
{
    RtcTester::instance().setWbToolType(WBToolType::TEXT);
}

void WBViewController::onToolPencil()
{
    RtcTester::instance().setWbToolType(WBToolType::PATH);
}

void WBViewController::onToolRectangle()
{
    RtcTester::instance().setWbToolType(WBToolType::RECT);
}

void WBViewController::onToolLine()
{
    RtcTester::instance().setWbToolType(WBToolType::LINE);
}

void WBViewController::onToolEllipse()
{
    RtcTester::instance().setWbToolType(WBToolType::ELLIPSE);
}

void WBViewController::onToolArrow()
{
    RtcTester::instance().setWbToolType(WBToolType::ARROW);
}

void WBViewController::onToolArc()
{
    RtcTester::instance().setWbToolType(WBToolType::ARC);
}

void WBViewController::onToolCurve()
{
    RtcTester::instance().setWbToolType(WBToolType::CURVE);
}

void WBViewController::onToolPolyline()
{
    RtcTester::instance().setWbToolType(WBToolType::POLYLINE);
}

void WBViewController::onToolPolygon()
{
    RtcTester::instance().setWbToolType(WBToolType::POLYGON);
}

void WBViewController::onToolPlay()
{
    RtcTester::instance().setWbToolType(WBToolType::NONE);
}

void WBViewController::onToolLaser()
{
    RtcTester::instance().setWbToolType(WBToolType::LASER);
}

void WBViewController::onToolUndo()
{
    RtcTester::instance().wbToolBoxUndo();
}

void WBViewController::onToolRedo()
{
    RtcTester::instance().wbToolBoxRedo();
}

void WBViewController::onToolForeColor(COLORREF color)
{
    RtcTester::instance().wbToolBoxForeColor(GetRValue(color), GetGValue(color), GetBValue(color));
}

void WBViewController::onToolFontStyle(WBFontStyle style)
{
    RtcTester::instance().wbToolBoxFontStyle(style);
}

void WBViewController::onToolFontSize(int size)
{
    RtcTester::instance().wbToolBoxFontSize(size);
}

void WBViewController::onToolLineWidth(int size)
{
    RtcTester::instance().wbToolBoxLineWidth(size);
}

void WBViewController::onToolEraser()
{
    RtcTester::instance().setWbToolType(WBToolType::ERASER);
}

void WBViewController::onToolClear()
{
    RtcTester::instance().wbToolClear();
}

void WBViewController::onToolBrush()
{
    RtcTester::instance().setWbToolType(WBToolType::BRUSH);
}

void WBViewController::onToolImage(const char* imageUrl)
{
    RtcTester::instance().wbToolBoxImage(imageUrl);
}

void WBViewController::onToolBgImage(const char* imageUrl)
{
    RtcTester::instance().wbToolBoxBgImage(imageUrl);
}

void WBViewController::OnToolVideo(const char* mediaUrl)
{
    RtcTester::instance().wbToolBoxVideo(mediaUrl);
}

void WBViewController::OnToolAudio(const char* mediaUrl)
{
    RtcTester::instance().wbToolBoxAudio(mediaUrl);
}

void WBViewController::OnToolSetStamp(const char* stampId)
{
    RtcTester::instance().wbToolSetStamp(stampId);
}

void WBViewController::onToolCreateDoc(const char* docUrl)
{
    RtcTester::instance().wbToolBoxCreateDoc(docUrl);
}

void WBViewController::onToolSwitchDoc(const char* docUrl)
{
    RtcTester::instance().wbToolBoxSwitchDoc(docUrl);
}

void WBViewController::onToolDeleteDoc(const char* docUrl)
{
    RtcTester::instance().wbToolBoxDeleteDoc(docUrl);
}

void WBViewController::onToolSnapshot()
{
    TCHAR szPath[MAX_PATH];
    SHGetSpecialFolderPath(NULL, szPath, CSIDL_MYPICTURES, TRUE);
    auto str = utf8_encode(szPath, MAX_PATH);
    RtcTester::instance().wbToolBoxSnapshot(str.c_str());
}

void WBViewController::onToolPrevPage()
{
    RtcTester::instance().wbToolBoxPrevPage();
}

void WBViewController::onToolAddPage()
{
    RtcTester::instance().wbToolBoxAddPage();
}

void WBViewController::onToolDelPage()
{
    RtcTester::instance().wbToolBoxDelPage();
}

void WBViewController::onToolNextPage()
{
    RtcTester::instance().wbToolBoxNextPage();
}

void WBViewController::onToolZoomIn()
{
    RtcTester::instance().wbToolBoxZoomIn();
}

void WBViewController::onToolZoomOut()
{
    RtcTester::instance().wbToolBoxZoomOut();
}

void WBViewController::onToolAdminRole()
{
    RtcTester::instance().wbToolAdminRole();
}

void WBViewController::onPageNumberChanged(panortc::WBPageNumber curPage, size_t totalPages)
{
    RtcTester::instance().runOnUIThread([=] {
        onPageNumberUpdate(curPage, totalPages);
    });
}

void WBViewController::onImageStateChanged(const char *url, panortc::WBImageState state)
{
    RtcTester::instance().runOnUIThread([=, str=std::string(url)] {
        onImageState(str.c_str(), state);
    });
}

void WBViewController::onViewScaleChanged(float scale)
{
    RtcTester::instance().runOnUIThread([=] {
        onViewScaleUpdate(scale);
    });
}

void WBViewController::onCreateDoc(panortc::QResult result, const char *fileId)
{
    RtcTester::instance().runOnUIThread([=, str=std::string(fileId)] {
        onCreateDoc(result == panortc::QResult::OK, str.c_str());
    });
}

void WBViewController::onDeleteDoc(panortc::QResult result, const char *fileId)
{
    RtcTester::instance().runOnUIThread([=, str=std::string(fileId)] {
        onDeleteDoc(result == panortc::QResult::OK, str.c_str());
    });
}

void WBViewController::onSwitchDoc(panortc::QResult result, const char *fileId)
{
    RtcTester::instance().runOnUIThread([=, str=std::string(fileId)] {
        onSwitchDoc(result == panortc::QResult::OK, str.c_str());
    });
}

void WBViewController::onSaveDoc(panortc::QResult result, const char *fileId, const char *outputDir)
{
    RtcTester::instance().runOnUIThread([=, str = std::string(fileId), path = std::string(outputDir)]{
        onSaveDoc(result == panortc::QResult::OK, str.c_str(), path.c_str());
    });
}

void WBViewController::onDocThumbnailReady(const char *fileId, const char **urls, size_t count)
{
    std::vector<std::string> vecUrls;
    for (size_t i=0; i<count; i++) {
        vecUrls.emplace_back(urls[i]);
    }
}

void WBViewController::onDrawRectUpdate(panortc::WBPoint offset, panortc::WBSize viewSize, panortc::WBSize maxSize)
{
    RtcTester::instance().runOnUIThread([=] {
        POINT op = { (LONG)offset.x, (LONG)offset.y};
        SIZE vs = { (LONG)viewSize.cx, (LONG)viewSize.cy};
        SIZE ms = {(LONG)maxSize.cx, (LONG)maxSize.cy};
        onViewUpdate(op, vs, ms);
    });
}

void WBViewController::onPageNumberUpdate(uint32_t curPage, uint32_t totalPages)
{
    if (whiteboardView_) {
        whiteboardView_->updatePageNumber(curPage, totalPages);
    }
}

void WBViewController::onImageState(const char *url, panortc::WBImageState state)
{
    if (whiteboardView_) {
         if (sizeof(TCHAR) == sizeof(char)) {
            whiteboardView_->imageState((LPCTSTR)url, (int32_t)state);
        } else {
            auto wstr = utf8_decode(url);
            LPCTSTR lpUrl = wstr.c_str();
            whiteboardView_->imageState(lpUrl, (int32_t)state);
        }
    }
}

void WBViewController::onViewScaleUpdate(float scale)
{
    if (whiteboardView_) {
        whiteboardView_->updateViewScale(scale);
    }
}

void WBViewController::onCreateDoc(bool success, const char* fileId)
{
    if (whiteboardView_) {
        const char *name = RtcTester::instance().getDocName(fileId);
        if (sizeof(TCHAR) == sizeof(char)) {
            whiteboardView_->onDocCreate(success, (LPCTSTR)name);
        } else {
            auto wname = utf8_decode(name);
            LPCTSTR lpFileName = wname.c_str();
            whiteboardView_->onDocCreate(success, lpFileName);
        }
    }
}

void WBViewController::onDeleteDoc(bool success, const char* fileId)
{
    if (whiteboardView_) {
        if (sizeof(TCHAR) == sizeof(char)) {
            whiteboardView_->onDocDelete(success, (LPCTSTR)fileId);
        } else {
            auto wstr = utf8_decode(fileId);
            LPCTSTR lpFileId = wstr.c_str();
            whiteboardView_->onDocDelete(success, lpFileId);
        }
    }
}

void WBViewController::onSwitchDoc(bool success, const char* fileId)
{
    if (whiteboardView_) {
        whiteboardView_->switchDoc(fileId);
        const char *name = RtcTester::instance().getDocName(fileId);
        if (sizeof(TCHAR) == sizeof(char)) {
            whiteboardView_->onDocSwitch(success, (LPCTSTR)name);
        } else {
            auto wname = utf8_decode(name);
            LPCTSTR lpFileName = wname.c_str();
            whiteboardView_->onDocSwitch(success, lpFileName);
        }
    }
}

void WBViewController::onSaveDoc(bool success, const char* fileId, const char* outputDir)
{
    if (whiteboardView_) {
        const char *name = RtcTester::instance().getDocName(fileId);
        if (sizeof(TCHAR) == sizeof(char)) {
            whiteboardView_->onDocSave(success, (LPCTSTR)name, (LPCTSTR)outputDir);
        } else {
            auto wname = utf8_decode(name);
            LPCTSTR lpFileName = wname.c_str();
            auto wpath = utf8_decode(outputDir);
            LPCTSTR lpPath = wpath.c_str();
            whiteboardView_->onDocSave(success, lpFileName, lpPath);
        }
    }
        }

void WBViewController::onViewUpdate(POINT pos, SIZE vSize, SIZE mSize)
{
    if (whiteboardView_ && whiteboardView_->IsWindow()) {
        whiteboardView_->updateView(pos, vSize, mSize);
    }
}

