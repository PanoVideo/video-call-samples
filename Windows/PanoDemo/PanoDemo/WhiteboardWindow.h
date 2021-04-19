#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlframe.h>
#include <atldlgs.h>

#include <functional>
#include <map>
#include <vector>
#include <filesystem>

#include "resource.h"
#include "WhiteboardView.h"
#include "WBViewController.h"
#include "utils.h"
#include "ToolbarHelper.h"

class WhiteboardWindow :
    public CFrameWindowImpl<WhiteboardWindow>,
    public CUpdateUI<WhiteboardWindow>,
    public CMessageFilter, public CIdleHandler,
     public CToolBarHelper<WhiteboardWindow>
{
    friend class CToolBarHelper<WhiteboardWindow>;

public:
    using OnDestroyObserver = std::function<void(void)>;

    WhiteboardWindow(WBViewController *controller)
        : controller_(controller)
    {

    }
    virtual ~WhiteboardWindow()
    {
        if (m_hWnd && IsWindow()) {
            if (::IsWindow(m_view.m_hWnd)) {
                m_view.DestroyWindow();
            }
            DestroyWindow();
            m_hWnd = nullptr;
        }
    }

    void setOnDestroyObserver(OnDestroyObserver observer)
    {
        m_observer = std::move(observer);
    }

    void close()
    {
        OnWindowClose();
    }

    void updatePageNumber(uint32_t curPage, uint32_t totalPages)
    {
        currentPage_ = curPage;
        totalPage_ = totalPages;
        updateDefaultMsg();
    }

    void imageState(LPCTSTR url, int32_t state)
    {
        if (state == 0) {
            std::wstring msg = L"loading image " + std::wstring(url);
            updateStatusMsg(msg.c_str());
        } else {
            std::wstring rstr = state == 1 ? L"Success " : L"Failed ";
            std::wstring msg = L"load image " + rstr + std::wstring(url);
            updateStatusMsg(msg.c_str());
        }
    }

    void updateViewScale(float scale)
    {
        scale_ = int(scale * 100);
        updateViewMsg();
    }

    void addDoc(const std::string& fileId, const std::string& fileName)
    {
        if (::IsWindow(wbDocListBox_.m_hWnd)) {
            wbDocListBox_.AddString(pano::utils::utf8_decode(fileName).c_str());
            docVec_.emplace_back(fileId,fileName);
        }
    }

    void switchDoc(const std::string& fileId)
    {
        if (::IsWindow(wbDocListBox_.m_hWnd)) {
            for (size_t i = 0; i < docVec_.size(); i++) {
                if (fileId == docVec_[i].first) {
                    wbDocListBox_.SetCurSel((int)i);
                    docId_ = pano::utils::utf8_decode(fileId);
                }
            }
        }
    }

    void onDocCreate(bool success, LPCTSTR name) {
        updateDocList();
        std::wstring rstr = success ? L" Success" : L" Failed";
        std::wstring msg = L"Create doc " + std::wstring(name) + rstr;
        updateStatusMsg(msg.c_str());
    }

    void onDocDelete(bool success, LPCTSTR fileId) {
        std::wstring fileName = fileId;
        for (size_t i = 0; i < docVec_.size(); i++) {
            auto fileIdStr = docVec_[i].first;
            if (fileId == pano::utils::utf8_decode(fileIdStr)) {
                fileName = pano::utils::utf8_decode(docVec_[i].second);
            }
        }
        updateDocList();
        std::wstring rstr = success ? L" Success" : L" Failed";
        std::wstring msg = L"Delete doc " + fileName + rstr;
        updateStatusMsg(msg.c_str());
    }

    void onDocSwitch(bool success, LPCTSTR name) {
        std::wstring rstr = success ? L" Success" : L" Failed";
        std::wstring msg = L"Switch to doc " + std::wstring(name) + rstr;
        updateStatusMsg(msg.c_str());
        updateDefaultMsg();
    }

    void onDocSave(bool success, LPCTSTR name, LPCTSTR path)
    {
        std::wstring rstr = success ? L" Success" : L" Failed";
        std::wstring msg = L"Save doc " + std::wstring(name) + L" to " + std::wstring(path) + rstr;
        updateStatusMsg(msg.c_str());
                }

    void updateView(POINT pos, SIZE vSize, SIZE mSize)
    {
        if (mSize.cx > vSize.cx) {
            m_view.EnableScrollBar(SB_HORZ, ESB_ENABLE_BOTH);
            SCROLLINFO hsi;
            hsi.cbSize = sizeof(SCROLLINFO);
            hsi.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
            hsi.nMin = 0;
            hsi.nMax = mSize.cx;
            hsi.nPos = pos.x;
            hsi.nPage = vSize.cx;
            m_view.SetScrollInfo(SB_HORZ, &hsi);
        } else {
            m_view.EnableScrollBar(SB_HORZ, ESB_DISABLE_BOTH);
            }

        if (mSize.cy > vSize.cy) {
            m_view.EnableScrollBar(SB_VERT, ESB_ENABLE_BOTH);
            SCROLLINFO hsi;
            hsi.cbSize = sizeof(SCROLLINFO);
            hsi.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
            hsi.nMin = 0;
            hsi.nMax = mSize.cy;
            hsi.nPos = pos.y;
            hsi.nPage = vSize.cy;
            m_view.SetScrollInfo(SB_VERT, &hsi);
        } else {
            m_view.EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
        }
        pos_ = pos;
        viewSize_ = vSize;
        wbSize_ = mSize;
        updateViewMsg();
    }

    HWND getWhiteboardView()
    {
        return m_view;
    }

    CWhiteboardView m_view;

    DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        if (CFrameWindowImpl<WhiteboardWindow>::PreTranslateMessage(pMsg))
            return TRUE;

        return m_view.PreTranslateMessage(pMsg);
    }

    virtual BOOL OnIdle()
    {
        UIUpdateToolBar();
        UIUpdateStatusBar(TRUE);
        return FALSE;
    }

public:
    BEGIN_UPDATE_UI_MAP(WhiteboardWindow)
        //UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(IDM_WB_SELECT, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDM_WB_TEXT, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDM_WB_PEN, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDM_WB_RECTANGLE, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDM_WB_ELLIPSE, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDM_WB_LINE, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDM_WB_BOLD, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDM_WB_ITALIC, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDM_WB_ERASER, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDM_WB_BRUSH, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDPANE_STATUS, UPDUI_STATUSBAR)
        UPDATE_ELEMENT(IDPANE_VIEW, UPDUI_STATUSBAR)
    END_UPDATE_UI_MAP()

    BEGIN_MSG_MAP(WhiteboardWindow)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
        MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
        MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
        COMMAND_ID_HANDLER(IDM_WB_SELECT, OnWbToolSelect)
        COMMAND_ID_HANDLER(IDM_WB_TEXT, OnWbToolText)
        COMMAND_ID_HANDLER(IDM_WB_PEN, OnWbToolPencil)
        COMMAND_ID_HANDLER(IDM_WB_RECTANGLE, OnWbToolRectangle)
        COMMAND_ID_HANDLER(IDM_WB_LINE, OnWbToolLine)
        COMMAND_ID_HANDLER(IDM_WB_ELLIPSE, OnWbToolEllipse)
        COMMAND_ID_HANDLER(IDM_WB_ARROW, OnWbToolArrow)
        COMMAND_ID_HANDLER(IDM_WB_ARC, OnWbToolArc)
        COMMAND_ID_HANDLER(IDM_WB_CURVE, OnWbToolCurve)
        COMMAND_ID_HANDLER(IDM_WB_POLYLINE, OnWbToolPolyline)
        COMMAND_ID_HANDLER(IDM_WB_POLYGON, OnWbToolPolygon)
        COMMAND_ID_HANDLER(IDM_WB_PLAY, OnWbToolPlay)
        COMMAND_ID_HANDLER(IDM_WB_LASER, OnWbToolLaser)
        COMMAND_ID_HANDLER(IDM_WB_UNDO, OnWbToolUndo)
        COMMAND_ID_HANDLER(IDM_WB_REDO, OnWbToolRedo)
        COMMAND_ID_HANDLER(IDM_WB_FORE_COLOR, OnWbToolForeColor)
        COMMAND_ID_HANDLER(IDM_WB_COLOR_RED, OnWbToolForeColorRed)
        COMMAND_ID_HANDLER(IDM_WB_COLOR_GREEN, OnWbToolForeColorGreen)
        COMMAND_ID_HANDLER(IDM_WB_COLOR_BLUE, OnWbToolForeColorBlue)
        COMMAND_ID_HANDLER(IDM_WB_COLOR_YELLOW, OnWbToolForeColorYellow)
        COMMAND_ID_HANDLER(IDM_WB_COLOR_BLACK, OnWbToolForeColorBlack)
        COMMAND_ID_HANDLER(IDM_WB_BOLD, OnWbToolBold)
        COMMAND_ID_HANDLER(IDM_WB_ITALIC, OnWbToolItalic)
        COMMAND_ID_HANDLER(IDM_WB_FONT, OnWbToolFont)
        COMMAND_ID_HANDLER(IDM_WB_ERASER_ONE, OnWbToolEraserOne)
        COMMAND_ID_HANDLER(IDM_WB_ERASER_CLEAR, OnWbToolEraserClear)
        COMMAND_ID_HANDLER(IDM_WB_ERASER_BRUSH, OnWbToolEraserBrush)
        COMMAND_ID_HANDLER(IDM_WB_IMAGE, OnWbToolImage)
        COMMAND_ID_HANDLER(IDM_WB_IMAGE_BACKGROUND, OnWbToolImageBg)
        COMMAND_ID_HANDLER(IDM_WB_VIDEO, OnWbToolVideo)
        COMMAND_ID_HANDLER(IDM_WB_AUDIO, OnWbToolAudio)
        COMMAND_ID_HANDLER(IDM_WB_STAMP_CROSS, OnWbToolStampCross)
        COMMAND_ID_HANDLER(IDM_WB_STAMP_HEART, OnWbToolStampHeart)
        COMMAND_ID_HANDLER(IDM_WB_STAMP_STAR, OnWbToolStampStar)
        COMMAND_ID_HANDLER(IDM_WB_STAMP_TICK, OnWbToolStampTick)
        COMMAND_ID_HANDLER(IDM_WB_STAMP_PRAISE, OnWbToolStampPraise)
        COMMAND_ID_HANDLER(IDM_WB_STAMP_HOUSE, OnWbToolStampHouse)
        COMMAND_ID_HANDLER(IDM_WB_STAMP_ROCKET, OnWbToolStampRocket)
        COMMAND_ID_HANDLER(IDM_WB_DOC, OnWbToolDoc)
        COMMAND_ID_HANDLER(IDM_WB_SNAPSHOT, OnWbToolSnapshot)
        COMMAND_ID_HANDLER(IDM_PREV_PAGE, OnWbToolPrevPage)
        COMMAND_ID_HANDLER(IDM_ADD_PAGE, OnWbToolAddPage)
        COMMAND_ID_HANDLER(IDM_DEL_PAGE, OnWbToolDelPage)
        COMMAND_ID_HANDLER(IDM_NEXT_PAGE, OnWbToolNextPage)
        COMMAND_ID_HANDLER(IDM_ZOOMIN, OnWbToolZoomIn)
        COMMAND_ID_HANDLER(IDM_ZOOMOUT, OnWbToolZoomOut)
        COMMAND_ID_HANDLER(IDM_ADMIN, OnWbToolAdminRole)
        //COMMAND_HANDLER(IDM_TB_COMBO_LINE_WIDTH, CBN_SELCHANGE, OnCbnSelchangeLineWidth)
        //COMMAND_HANDLER(IDM_TB_COMBO_WBDOC_LIST, CBN_SELCHANGE, OnCbnSelchangeWBDoc)
        CHAIN_MSG_MAP(CUpdateUI<WhiteboardWindow>)
        CHAIN_MSG_MAP(CFrameWindowImpl<WhiteboardWindow>)
        CHAIN_MSG_MAP(CToolBarHelper<WhiteboardWindow>)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CenterWindow(GetParent());

        // create command bar window
        HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);

        m_CmdBar.AttachMenu(GetMenu());

        // load command bar images
        m_CmdBar.LoadImages(IDR_TOOLBAR_WB);
        m_CmdBar.LoadImages(IDR_TOOLBAR_COLOR);
        m_CmdBar.LoadImages(IDR_TOOLBAR_HOLLOW);
        m_CmdBar.LoadImages(IDR_TOOLBAR_STAMP);
        m_CmdBar.LoadImages(IDR_TOOLBAR_ERASER);
        m_CmdBar.LoadImages(IDR_TOOLBAR_MEDIA);

        SetMenu(NULL);

        HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_TOOLBAR_WB, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE_EX);

        AddToolBarDropDownMenu(hWndToolBar, IDM_WB_FORE_COLOR, IDR_MENU_COLOR);
        AddToolbarButtonText(hWndToolBar, IDM_WB_FORE_COLOR);
        AddToolBarDropDownMenu(hWndToolBar, IDM_WB_HOLLOW, IDR_MENU_HOLLOW);
        AddToolBarDropDownMenu(hWndToolBar, IDM_WB_STAMP, IDR_MENU_STAMP);
        AddToolBarDropDownMenu(hWndToolBar, IDM_WB_ERASER, IDR_MENU_ERASER);
        AddToolBarDropDownMenu(hWndToolBar, IDM_WB_MEDIA, IDR_MENU_MEDIA);

        CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
        AddSimpleReBarBand(hWndCmdBar);
        AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

        lineWidthBox_ = CreateToolbarComboBox(hWndToolBar, IDM_WB_LINE_WIDTH);
        lineWidthBox_.AddString(_T("Line 1"));
        lineWidthBox_.AddString(_T("Line 2"));
        lineWidthBox_.AddString(_T("Line 3"));
        lineWidthBox_.AddString(_T("Line 4"));
        lineWidthBox_.AddString(_T("Line 5"));
        lineWidthBox_.SetCurSel(0);

        wbDocListBox_ = CreateToolbarComboBox(hWndToolBar, IDM_WB_DOC_LIST, 8, 8);

        //CreateSimpleStatusBar();

        m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL, 0);
        m_view.ShowScrollBar(0, 1);
        m_view.ShowScrollBar(1, 1);
        m_view.EnableScrollBar(SB_HORZ, ESB_DISABLE_BOTH);
        m_view.EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);

        UIAddToolBar(hWndToolBar);

        m_hWndStatusBar = m_wndStatusBar.Create(*this);
        UIAddStatusBar(m_hWndStatusBar);
        statusBarReady_ = true;

        int anPanes[] = { ID_DEFAULT_PANE, IDPANE_STATUS,
                  IDPANE_VIEW };

        m_wndStatusBar.SetPanes(anPanes, 3, false);
        m_wndStatusBar.SetPaneWidth(IDPANE_STATUS, 500);
        m_wndStatusBar.SetPaneWidth(IDPANE_VIEW, 300);

        controller_->refreshDocs();

        // register object for message filtering and idle updates
        CMessageLoop* pLoop = _Module.GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);
        pLoop->AddIdleHandler(this);

        OnWindowCreate();

        CReBarCtrl rebar = m_hWndToolBar;
        int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST);	// toolbar is first 1st band
        rebar.ShowBand(nBandIndex, TRUE);
        UpdateLayout();

        checkedButtonId_ = 0;
        UISetCheck(IDM_WB_SELECT, false);
        CheckToolBox(IDM_WB_SELECT);
        controller_->onToolSelect();

        enableBold_ = false;
        enableItalic_ = false;
        UISetCheck(IDM_WB_BOLD, enableBold_);
        UISetCheck(IDM_WB_ITALIC, enableItalic_);
        fontStyle_ = panortc::WBFontStyle::NORMAL;
        controller_->onToolFontStyle(fontStyle_);

        controller_->onToolLineWidth(lineWidth_);

        controller_->onPageInfo(currentPage_, totalPage_);
        updateDefaultMsg();

        addStampResource();

        return 0;
    }

    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        auto ret = OnWindowClose();
        bHandled = ret == 0;
        return ret;
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        // unregister message filtering and idle updates
        CMessageLoop* pLoop = _Module.GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->RemoveMessageFilter(this);
        pLoop->RemoveIdleHandler(this);

        auto ret = OnWindowDestroy();
        bHandled = ret == 0;
        return ret;
    }

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        //CPaintDC dc(m_hWnd);
        bHandled = FALSE;

        return 0;
    }

    LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        controller_->onSwitchWhiteboard();
        return 0;
    }

    LRESULT OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        SCROLLINFO si;
        si.fMask = SIF_ALL;
        si.cbSize = sizeof(si);
        m_view.GetScrollInfo(SB_HORZ, &si);
        int nPos = si.nPos;
        switch (LOWORD(wParam))
        {
        case SB_ENDSCROLL:
            break;
        case SB_LEFT:
            nPos = si.nMin;
            break;
        case SB_RIGHT:
            nPos = si.nMax;
            break;
        case SB_LINELEFT:
            nPos -= 10;
            break;
        case SB_LINERIGHT:
            nPos += 10;
            break;
        case SB_PAGELEFT:
            nPos -= si.nPage;
            break;
        case SB_PAGERIGHT:
            nPos += si.nPage;
            break;
        case SB_THUMBPOSITION:
            nPos = HIWORD(wParam);
            break;
        case SB_THUMBTRACK:
            nPos = HIWORD(wParam);
            break;
        default:
            break;
        }
        int max = si.nMax - si.nPage;
        if (nPos > max)
            nPos = si.nMax - si.nPage;
        if (nPos < si.nMin)
            nPos = si.nMin;
        //m_view.ScrollWindowEx(si.nPos - nPos, 0, 0);
        controller_->onScrollView(nPos - si.nPos, 0);
        si.nPos = nPos;
        m_view.SetScrollInfo(SB_HORZ, &si);
        return 0;
    }

    LRESULT OnVScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        SCROLLINFO si;
        si.fMask = SIF_ALL;
        si.cbSize = sizeof(si);
        m_view.GetScrollInfo(SB_VERT, &si);
        int nPos = si.nPos;
        switch (LOWORD(wParam))
        {
        case SB_ENDSCROLL:
            break;
        case SB_LEFT:
            nPos = si.nMin;
            break;
        case SB_RIGHT:
            nPos = si.nMax;
            break;
        case SB_LINELEFT:
            nPos -= 10;
            break;
        case SB_LINERIGHT:
            nPos += 10;
            break;
        case SB_PAGELEFT:
            nPos -= si.nPage;
            break;
        case SB_PAGERIGHT:
            nPos += si.nPage;
            break;
        case SB_THUMBPOSITION:
            nPos = HIWORD(wParam);
            break;
        case SB_THUMBTRACK:
            nPos = HIWORD(wParam);
            break;
        default:
            break;
        }
        int max = si.nMax - si.nPage;
        if (nPos > max)
            nPos = si.nMax - si.nPage;
        if (nPos < si.nMin)
            nPos = si.nMin;
        //ScrollWindowEx(si.nPos - nPos, 0, 0);
        controller_->onScrollView(0, nPos - si.nPos);
        si.nPos = nPos;
        m_view.SetScrollInfo(SB_VERT, &si);
        return 0;
    }

protected:
    LRESULT OnWbToolSelect(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_SELECT);
        toolType_ = panortc::WBToolType::SELECT;
        controller_->onToolSelect();
        return 0;
    }

    LRESULT OnWbToolText(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_TEXT);
        toolType_ = panortc::WBToolType::TEXT;
        controller_->onToolText();
        return 0;
    }

    LRESULT OnWbToolPencil(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_PEN);
        toolType_ = panortc::WBToolType::PATH;
        controller_->onToolPencil();
        return 0;
    }

    LRESULT OnWbToolRectangle(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_RECTANGLE);
        toolType_ = panortc::WBToolType::RECT;
        controller_->onToolRectangle();
        return 0;
    }

    LRESULT OnWbToolLine(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_LINE);
        toolType_ = panortc::WBToolType::LINE;
        controller_->onToolLine();
        return 0;
    }

    LRESULT OnWbToolEllipse(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_ELLIPSE);
        toolType_ = panortc::WBToolType::ELLIPSE;
        controller_->onToolEllipse();
        return 0;
    }

    LRESULT OnWbToolArrow(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_ARROW);
        toolType_ = panortc::WBToolType::ARROW;
        controller_->onToolArrow();
        return 0;
    }

    LRESULT OnWbToolArc(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_ARC);
        toolType_ = panortc::WBToolType::ARC;
        controller_->onToolArc();
        return 0;
    }

    LRESULT OnWbToolCurve(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_CURVE);
        toolType_ = panortc::WBToolType::CURVE;
        controller_->onToolCurve();
        return 0;
    }

    LRESULT OnWbToolPolyline(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_POLYLINE);
        toolType_ = panortc::WBToolType::POLYLINE;
        controller_->onToolPolyline();
        return 0;
    }

    LRESULT OnWbToolPolygon(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_POLYGON);
        toolType_ = panortc::WBToolType::POLYGON;
        controller_->onToolPolygon();
        return 0;
    }

    LRESULT OnWbToolPlay(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        if (checkedButtonId_ != IDM_WB_PLAY) {
            CheckToolBox(IDM_WB_PLAY);
            controller_->onToolPlay();
        } else {
            if (toolType_ == panortc::WBToolType::PATH) {
                CheckToolBox(IDM_WB_PEN);
                controller_->onToolPencil();
            } else if (toolType_ == panortc::WBToolType::LINE) {
                CheckToolBox(IDM_WB_LINE);
                controller_->onToolLine();
            } else if (toolType_ == panortc::WBToolType::RECT) {
                CheckToolBox(IDM_WB_RECTANGLE);
                controller_->onToolRectangle();
            } else if (toolType_ == panortc::WBToolType::ELLIPSE) {
                CheckToolBox(IDM_WB_ELLIPSE);
                controller_->onToolEllipse();
            } else {
                CheckToolBox(IDM_WB_SELECT);
                controller_->onToolSelect();
            }
        }
        
        return 0;
    }

    LRESULT OnWbToolLaser(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_LASER);
        toolType_ = panortc::WBToolType::LASER;
        controller_->onToolLaser();
        return 0;
    }

    LRESULT OnWbToolUndo(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolUndo();
        return 0;
    }

    LRESULT OnWbToolRedo(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolRedo();
        return 0;
    }

    LRESULT OnWbToolForeColor(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CColorDialog dlg(foreColor_, CC_ANYCOLOR);
        if (dlg.DoModal() == IDOK) {
            foreColor_ = dlg.GetColor();
            controller_->onToolForeColor(foreColor_);
        }
        return 0;
    }

    LRESULT OnWbToolForeColorRed(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolForeColor(RGB(255, 0, 0));
        return 0;
    }

    LRESULT OnWbToolForeColorGreen(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolForeColor(RGB(64, 224, 0));
        return 0;
    }

    LRESULT OnWbToolForeColorBlue(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolForeColor(RGB(0, 96, 192));
        return 0;
    }

    LRESULT OnWbToolForeColorYellow(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolForeColor(RGB(255, 242, 0));
        return 0;
    }

    LRESULT OnWbToolForeColorBlack(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolForeColor(RGB(0, 0, 0));
        return 0;
    }

    LRESULT OnWbToolBold(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        enableBold_ = !enableBold_;
        UISetCheck(IDM_WB_BOLD, enableBold_);
        updateStyleBold();
        controller_->onToolFontStyle(fontStyle_);
        return 0;
    }

    LRESULT OnWbToolItalic(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        enableItalic_ = !enableItalic_;
        UISetCheck(IDM_WB_ITALIC, enableItalic_);
        updateStyleItalic();
        controller_->onToolFontStyle(fontStyle_);
        return 0;
    }

    LRESULT OnWbToolFont(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        LOGFONT lf{ 0 };
        lf.lfWeight = enableBold_ ? FW_BOLD : 0;
        lf.lfItalic = enableItalic_ ? 1 : 0;
        CFontDialog dlg(&lf);
        dlg.m_cf.iPointSize = fontSize_ * 10;
        if (dlg.DoModal() == IDOK) {
            fontSize_ = dlg.GetSize() / 10;
            controller_->onToolFontSize(fontSize_);
        }
        return 0;
    }

    LRESULT OnWbToolEraserOne(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_ERASER_ONE);
        controller_->onToolEraser();
        return 0;
    }

    LRESULT OnWbToolEraserClear(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_ERASER_CLEAR);
        controller_->onToolClear();
        return 0;
    }

    LRESULT OnWbToolEraserBrush(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_ERASER_BRUSH);
        controller_->onToolBrush();
        return 0;
    }

    LRESULT OnWbToolImage(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        wchar_t* filter = L"image files (*.png; *.jpg; *.bmp)\0*.png;*.jpeg;*.jpg;*.bmp\0All Files (*.*)\0*.*\0\0";
        CFileDialog dlg(TRUE, 0, 0, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, filter, m_hWnd);
        if (dlg.DoModal() == IDOK) {
            LPWSTR lpFilePath = dlg.m_ofn.lpstrFile;
            std::string path = pano::utils::utf8_encode(lpFilePath);
            controller_->onToolImage(path.c_str());
        }
        return 0;
    }

    LRESULT OnWbToolImageBg(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        wchar_t* filter = L"image files (*.png; *.jpg; *.bmp)\0*.png;*.jpeg;*.jpg;*.bmp\0All Files (*.*)\0*.*\0\0";
        CFileDialog dlg(TRUE, 0, 0, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, filter, m_hWnd);
        if (dlg.DoModal() == IDOK) {
            LPWSTR lpFilePath = dlg.m_ofn.lpstrFile;
            std::string path = pano::utils::utf8_encode(lpFilePath);
            controller_->onToolBgImage(path.c_str());
        }
        return 0;
    }

    LRESULT OnWbToolVideo(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        wchar_t* filter = L"video files (*.mp4)\0*.mp4\0All Files (*.*)\0*.*\0\0";
        CFileDialog dlg(TRUE, 0, 0, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, filter, m_hWnd);
        if (dlg.DoModal() == IDOK) {
            LPWSTR lpFilePath = dlg.m_ofn.lpstrFile;
            std::string path = pano::utils::utf8_encode(lpFilePath);
            controller_->OnToolVideo(path.c_str());
        }
        return 0;
    }

    LRESULT OnWbToolAudio(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        wchar_t* filter = L"audio files (*.mp3)\0*.mp3\0All Files (*.*)\0*.*\0\0";
        CFileDialog dlg(TRUE, 0, 0, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, filter, m_hWnd);
        if (dlg.DoModal() == IDOK) {
            LPWSTR lpFilePath = dlg.m_ofn.lpstrFile;
            std::string path = pano::utils::utf8_encode(lpFilePath);
            controller_->OnToolAudio(path.c_str());
        }
        return 0;
    }

    LRESULT OnWbToolStampCross(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->OnToolSetStamp("cross");
        return 0;
    }

    LRESULT OnWbToolStampHeart(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->OnToolSetStamp("heart");
        return 0;
    }

    LRESULT OnWbToolStampStar(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->OnToolSetStamp("star");
        return 0;
    }

    LRESULT OnWbToolStampTick(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->OnToolSetStamp("tick");
        return 0;
    }

    LRESULT OnWbToolStampPraise(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->OnToolSetStamp("zan");
        return 0;
    }

    LRESULT OnWbToolStampHouse(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->OnToolSetStamp("house");
        return 0;
    }

    LRESULT OnWbToolStampRocket(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->OnToolSetStamp("rocket");
        return 0;
    }

    LRESULT OnWbToolDoc(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        wchar_t* filter = L"doc files (*.xlsx; *.docx; *.pptx)\0*.xlsx;*.docx;*.pptx\0All Files (*.*)\0*.*\0\0";
        CFileDialog dlg(TRUE, 0, 0, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, filter, m_hWnd);
        if (dlg.DoModal() == IDOK) {
            LPWSTR lpFilePath = dlg.m_ofn.lpstrFile;
            std::string path = pano::utils::utf8_encode(lpFilePath);
            controller_->onToolCreateDoc(path.c_str());
        }
        return 0;
    }

    LRESULT OnWbToolSnapshot(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolSnapshot();
        return 0;
    }

    LRESULT OnWbToolPrevPage(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolPrevPage();
        return 0;
    }

    LRESULT OnWbToolAddPage(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolAddPage();
        return 0;
    }

    LRESULT OnWbToolDelPage(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolDelPage();
        return 0;
    }

    LRESULT OnWbToolNextPage(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolNextPage();
        return 0;
    }

    LRESULT OnWbToolZoomIn(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolZoomIn();
        return 0;
    }

    LRESULT OnWbToolZoomOut(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolZoomOut();
        return 0;
    }

    LRESULT OnWbToolAdminRole(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onToolAdminRole();
        return 0;
    }

    void OnToolBarCombo(HWND hWndCombo, UINT nID, int nSel, LPCTSTR lpszText, DWORD dwItemData)
    {
        BOOL bHandled = FALSE;
        if (nID == IDM_WB_LINE_WIDTH) {
            OnCbnSelchangeLineWidth(0, nID, hWndCombo, bHandled);
        }
        else if (nID == IDM_WB_DOC_LIST) {
            OnCbnSelchangeWBDoc(0, nID, hWndCombo, bHandled);
        }
    }

    LRESULT OnCbnSelchangeLineWidth(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        int w = lineWidthBox_.GetCurSel() + 1;
        if (w != lineWidth_) {
            lineWidth_ = w;
            controller_->onToolLineWidth(lineWidth_);
        }
        return 0;
    }

    LRESULT OnCbnSelchangeWBDoc(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        int idx = wbDocListBox_.GetCurSel();
        std::string docStr = docVec_[idx].first;
        controller_->onToolSwitchDoc(docStr.c_str());
        return 0;
    }

    void CheckToolBox(int newId)
    {
        if (checkedButtonId_ != 0 && newId != checkedButtonId_) {
            UISetCheck(checkedButtonId_, false);
        }
        checkedButtonId_ = newId;
        UISetCheck(checkedButtonId_, true);
    }

protected:
    void OnWindowCreate()
    {
        //SetWindowText(_T("WhiteBoard"));
    }

    virtual LRESULT OnWindowClose()
    {
        if (::IsWindow(m_view.m_hWnd)) {
            m_view.DestroyWindow();
        }
        if (::IsWindow(lineWidthBox_.m_hWnd)) {
            lineWidthBox_.DestroyWindow();
        }
        if (::IsWindow(wbDocListBox_.m_hWnd)) {
            wbDocListBox_.DestroyWindow();
        }
        if (::IsWindow(m_hWnd)) {
            DestroyWindow();
        }
        return 0;
    }

    LRESULT OnWindowDestroy()
    {
        if (m_observer) {
            m_observer();
        }
        return 0;
    }

    void updateStyleBold()
    {
        if (enableBold_) {
            if (fontStyle_ == panortc::WBFontStyle::NORMAL) {
                fontStyle_ = panortc::WBFontStyle::BOLD;
            }
            else if (fontStyle_ == panortc::WBFontStyle::ITALIC) {
                fontStyle_ = panortc::WBFontStyle::BOLD_ITALIC;
            }
        }
        else {
            if (fontStyle_ == panortc::WBFontStyle::BOLD) {
                fontStyle_ = panortc::WBFontStyle::NORMAL;
            }
            else if (fontStyle_ == panortc::WBFontStyle::BOLD_ITALIC) {
                fontStyle_ = panortc::WBFontStyle::ITALIC;
            }
        }
    }

    void updateStyleItalic()
    {
        if (enableItalic_) {
            if (fontStyle_ == panortc::WBFontStyle::NORMAL) {
                fontStyle_ = panortc::WBFontStyle::ITALIC;
            }
            else if (fontStyle_ == panortc::WBFontStyle::BOLD) {
                fontStyle_ = panortc::WBFontStyle::BOLD_ITALIC;
            }
        }
        else {
            if (fontStyle_ == panortc::WBFontStyle::ITALIC) {
                fontStyle_ = panortc::WBFontStyle::NORMAL;
            }
            else if (fontStyle_ == panortc::WBFontStyle::BOLD_ITALIC) {
                fontStyle_ = panortc::WBFontStyle::BOLD;
            }
        }
    }

    void updateDocList() {
        wbDocListBox_.ResetContent();
        docVec_.clear();
        controller_->refreshDocs();
    }

    void updateDefaultMsg()
    {
        if (!statusBarReady_)
            return;
        std::wstring msg = L"Doc: " + docId_ + L", Page: " + std::to_wstring(currentPage_) + L"/" + std::to_wstring(totalPage_);
        m_wndStatusBar.SetPaneText(ID_DEFAULT_PANE, msg.c_str());
    }

    void updateStatusMsg(LPCTSTR msg)
    {
        if (!statusBarReady_)
            return;
        m_wndStatusBar.SetPaneText(IDPANE_STATUS, msg);
    }

    void updateViewMsg()
    {
        if (!statusBarReady_)
            return;
        std::wstring msg = L"[" + std::to_wstring(pos_.x) + L"," + std::to_wstring(pos_.y) + L","
            + std::to_wstring(viewSize_.cx) + L"," + std::to_wstring(viewSize_.cy) + L"], "
            + std::to_wstring(wbSize_.cx) + L"," + std::to_wstring(wbSize_.cy) + L" ratio: "
            + std::to_wstring(scale_) + L"%";
        m_wndStatusBar.SetPaneText(IDPANE_VIEW, msg.c_str());
    }

    void addStampResource()
    {
        auto modulePath = pano::utils::utf8_encode(pano::utils::getModulePathW());

        std::filesystem::path resPath = std::filesystem::u8path(modulePath);
        resPath.append("resource");
        resPath = std::filesystem::absolute(resPath);

        if (!std::filesystem::exists(resPath))
            return;

        std::filesystem::directory_iterator dirList(resPath);
        for (auto& iter : dirList) {
            auto stampId = iter.path().stem().u8string();
            auto path = iter.path().u8string();

            if (stampId == "house" || stampId == "rocket")
                controller_->addStampResource(stampId.c_str(), path.c_str(), true);
            else
                controller_->addStampResource(stampId.c_str(), path.c_str(), false);
        }
    }

protected:
    WBViewController* controller_;
    OnDestroyObserver m_observer;
    CMultiPaneStatusBarCtrl m_wndStatusBar;
    bool statusBarReady_ = false;

    int checkedButtonId_ = 0;
    panortc::WBToolType toolType_ = panortc::WBToolType::SELECT;

    bool enableBold_ = false;
    bool enableItalic_ = false;
    panortc::WBFontStyle fontStyle_ = panortc::WBFontStyle::NORMAL;
    int fontSize_ = 12;
    COLORREF foreColor_ = RGB(0, 0, 0);

    int lineWidth_ = 1;
    CCommandBarCtrl m_CmdBar;
    CComboBox lineWidthBox_;
    CComboBox wbDocListBox_;

    POINT pos_;
    SIZE viewSize_;
    SIZE wbSize_;
    int scale_ = 100;

    int currentPage_ = 1;
    int totalPage_ = 1;
    std::wstring docId_ = L"default";
    std::vector<std::pair<std::string,std::string>> docVec_;
};

