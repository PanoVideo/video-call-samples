#pragma once

#include "PanoWhiteboard.h"

#include <memory>
#include <string>

class WhiteboardWindow;

class WBViewController : public panortc::RtcWhiteboard::Callback
{
public:
    WBViewController();
    WBViewController(const char *whiteboardId);
    ~WBViewController();

    void startWhiteboard();
    void stopWhiteboard();
    void close();

    void onPageInfo(int &currentPage, int &totalPage);
    void onScrollView(int dx, int dy);
    void refreshDocs();
    void addStampResource(const char* stampId, const char* path, bool resizable);

    void onSwitchWhiteboard();

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
    void onToolPlay();
    void onToolLaser();
    void onToolUndo();
    void onToolRedo();

    void onToolForeColor(COLORREF color);
    void onToolFontStyle(panortc::WBFontStyle style);
    void onToolFontSize(int size);
    void onToolLineWidth(int size);
    void onToolEraser();
    void onToolClear();
    void onToolBrush();
    void onToolImage(const char* imageUrl);
    void onToolBgImage(const char* imageUrl);
    void OnToolVideo(const char* mediaUrl);
    void OnToolAudio(const char* mediaUrl);
    void OnToolSetStamp(const char* stampId);
    void onToolCreateDoc(const char* docUrl);
    void onToolSwitchDoc(const char* docUrl);
    void onToolDeleteDoc(const char* docUrl);

    void onToolSnapshot();

    void onToolPrevPage();
    void onToolAddPage();
    void onToolDelPage();
    void onToolNextPage();

    void onToolZoomIn();
    void onToolZoomOut();

    void onToolAdminRole();

public: // RtcWhiteboard::Callback
    void onPageNumberChanged(panortc::WBPageNumber curPage, size_t totalPages) override;
    void onImageStateChanged(const char *url, panortc::WBImageState state) override;
    void onViewScaleChanged(float scale) override;
    //void onRoleTypeChanged(panortc::WBRoleType newRole) override;
    //void onContentUpdated() override;
    //void onSnapshotComplete(panortc::QResult result, const char *filename) override;
    //void onMessage(uint64_t userId, const char *msg, size_t size) override;
    //void onAddBackgroundImages(panortc::QResult result, const char *fileId) override;
    //void onDocTranscodeStatus(panortc::QResult result,
    //                            const char *fileId,
    //                            uint32_t progress,
    //                            uint32_t totalPage) override;
    void onCreateDoc(panortc::QResult result, const char *fileId) override;
    void onDeleteDoc(panortc::QResult result, const char *fileId) override;
    void onSwitchDoc(panortc::QResult result, const char *fileId) override;
    void onSaveDoc(panortc::QResult result, const char *fileId, const char *outputDir) override;
    void onDocThumbnailReady(const char *fileId, const char **urls, size_t count) override;
    void onDrawRectUpdate(panortc::WBPoint offset, panortc::WBSize viewSize, panortc::WBSize maxSize) override;

    void onPageNumberUpdate(uint32_t curPage, uint32_t totalPages);
    void onImageState(const char *url, panortc::WBImageState state);
    void onViewScaleUpdate(float scale);
    void onCreateDoc(bool success, const char* fileId);
    void onDeleteDoc(bool success, const char* fileId);
    void onSwitchDoc(bool success, const char* fileId);
    void onSaveDoc(bool success, const char *fileId, const char *outputDir);
    void onViewUpdate(POINT pos, SIZE vSize, SIZE mSize);

protected:
    std::string whiteboardId_;
    bool whiteboardStarted_ = false;
    std::unique_ptr<WhiteboardWindow> whiteboardView_;
};

