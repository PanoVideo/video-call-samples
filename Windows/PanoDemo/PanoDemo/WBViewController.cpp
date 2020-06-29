#include "stdafx.h"
#include "WBViewController.h"
#include "WhiteboardWindow.h"
#include "RtcTester.h"
#include "PanoRtcEngine.h"


using namespace pano::utils;
using namespace panortc;

const int kDefaultVideoViewWidth = 960;
const int kDefaultVideoViewHeight = 540;


WBViewController::WBViewController()
{
    whiteboardView_ = std::make_unique<WhiteboardWindow>(this);
    whiteboardView_->setOnDestroyObserver([this] {
        stopWhiteboard();
    });
}

WBViewController::~WBViewController() = default;

void WBViewController::startWhiteboard()
{
    if (!whiteboardView_->IsWindow()) {
        RECT rc{ 0, 0, kDefaultVideoViewWidth, kDefaultVideoViewHeight };
        whiteboardView_->Create(NULL, rc, _T("WhiteBoard"), WS_OVERLAPPEDWINDOW);
    }
    whiteboardView_->ShowWindow(SW_SHOW);
    whiteboardView_->UpdateWindow();
    HWND hWnd = whiteboardView_->getWhiteboardView();

    RtcTester::instance().startWhiteboard(hWnd);
    whiteboardStarted_ = true;
}

void WBViewController::stopWhiteboard()
{
    if (whiteboardStarted_) {
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

void WBViewController::refreshDocs()
{
    RtcTester::instance().enumDocs([this](const char *fileId) {
        if (whiteboardView_) {
            if (sizeof(TCHAR) == sizeof(char)) {
                whiteboardView_->createDoc((LPCTSTR)fileId);
            } else {
                auto wstr = utf8_decode(fileId);
                LPCTSTR lpFileId = wstr.c_str();
                whiteboardView_->createDoc(lpFileId);
            }
        }
    });

    const char* fileId = RtcTester::instance().getCurrentDoc();
    if (whiteboardView_) {
        if (sizeof(TCHAR) == sizeof(char)) {
            whiteboardView_->switchDoc((LPCTSTR)fileId);
        }
        else {
            auto wstr = utf8_decode(fileId);
            LPCTSTR lpFileId = wstr.c_str();
            whiteboardView_->switchDoc(lpFileId);
        }
    }
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

void WBViewController::onToolBrush()
{
    RtcTester::instance().setWbToolType(WBToolType::BRUSH);
}

void WBViewController::onToolImage(const char* imageUrl)
{
    RtcTester::instance().wbToolBoxBgImage(imageUrl);
}

void WBViewController::onToolCreateDoc(const char* docUrl)
{
    RtcTester::instance().wbToolBoxCreateDoc(docUrl);
}

void WBViewController::onToolSwitchDoc(const char* docUrl)
{
    RtcTester::instance().wbToolBoxSwitchDoc(docUrl);
}

void WBViewController::onToolSnapshot()
{
    TCHAR szPath[MAX_PATH];
    SHGetSpecialFolderPath(NULL, szPath, CSIDL_MYPICTURES, TRUE);
    auto str = utf8_encode(szPath, MAX_PATH);
    RtcTester::instance().wbToolBoxSnapshot(str.c_str());
}

void WBViewController::onPrevPage()
{
    RtcTester::instance().wbToolBoxPrevPage();
}

void WBViewController::onAddPage()
{
    RtcTester::instance().wbToolBoxAddPage();
}

void WBViewController::onDelPage()
{
    RtcTester::instance().wbToolBoxDelPage();
}

void WBViewController::onNextPage()
{
    RtcTester::instance().wbToolBoxNextPage();
}

void WBViewController::onCreateDoc(const char* fileId)
{
    if (whiteboardView_) {
        if (sizeof(TCHAR) == sizeof(char)) {
            whiteboardView_->createDoc((LPCTSTR)fileId);
        } else {
            auto wstr = utf8_decode(fileId);
            LPCTSTR lpFileId = wstr.c_str();
            whiteboardView_->createDoc(lpFileId);
        }
    }
}

void WBViewController::onDeleteDoc(const char* fileId)
{
    if (whiteboardView_) {
        if (sizeof(TCHAR) == sizeof(char)) {
            whiteboardView_->deleteDoc((LPCTSTR)fileId);
        } else {
            auto wstr = utf8_decode(fileId);
            LPCTSTR lpFileId = wstr.c_str();
            whiteboardView_->deleteDoc(lpFileId);
        }
    }
}

void WBViewController::onSwitchDoc(const char* fileId)
{
    if (whiteboardView_) {
        if (sizeof(TCHAR) == sizeof(char)) {
            whiteboardView_->switchDoc((LPCTSTR)fileId);
        } else {
            auto wstr = utf8_decode(fileId);
            LPCTSTR lpFileId = wstr.c_str();
            whiteboardView_->switchDoc(lpFileId);
        }
    }
}

