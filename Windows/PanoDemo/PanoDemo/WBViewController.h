#pragma once

#include "PanoWhiteboard.h"

#include <memory>
#include <string>

class WhiteboardWindow;

class WBViewController
{
public:
    WBViewController();
    ~WBViewController();

    void startWhiteboard();
    void stopWhiteboard();
    void close();

    void refreshDocs();

    void onToolSelect();
    void onToolText();
    void onToolPencil();
    void onToolRectangle();
    void onToolLine();
    void onToolEllipse();
    void onToolArrow();
    void onToolArc();
    void onToolCurve();
    void onToolPolyline();
    void onToolPolygon();
    void onToolUndo();
    void onToolRedo();

    void onToolForeColor(COLORREF color);
    void onToolFontStyle(panortc::WBFontStyle style);
    void onToolFontSize(int size);
    void onToolLineWidth(int size);
    void onToolEraser();
    void onToolBrush();
    void onToolImage(const char* imageUrl);
    void onToolCreateDoc(const char* docUrl);
    void onToolSwitchDoc(const char* docUrl);

    void onToolSnapshot();

    void onPrevPage();
    void onAddPage();
    void onDelPage();
    void onNextPage();

    void onCreateDoc(const char* fileId);
    void onDeleteDoc(const char* fileId);
    void onSwitchDoc(const char* fileId);

protected:
    bool whiteboardStarted_ = false;
    std::unique_ptr<WhiteboardWindow> whiteboardView_;
};

