#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <atlframe.h>
#include <atldlgs.h>

#include <functional>

#include "resource.h"
#include "WhiteboardView.h"
#include "WBViewController.h"
#include "utils.h"

#define IDM_TB_COMBO_LINE_WIDTH 8008
#define IDM_TB_COMBO_WBDOC_LIST 8009

class WhiteboardWindow :
    public CFrameWindowImpl<WhiteboardWindow>,
    public CUpdateUI<WhiteboardWindow>,
    public CMessageFilter, public CIdleHandler
{
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

    void createDoc(LPCTSTR fileId)
    {
        if (::IsWindow(wbDocListBox_.m_hWnd)) {
            wbDocListBox_.AddItem(fileId, -1, -1, 0);
        }
    }

    void deleteDoc(LPCTSTR fileId)
    {
        if (::IsWindow(wbDocListBox_.m_hWnd)) {
            int count = wbDocListBox_.GetCount();
            for (int i = 0; i < count; i++) {
                TCHAR szText[256];
                wbDocListBox_.GetItemText(i, szText, 256);
                if (0 == _tcscmp(szText, fileId)) {
                    wbDocListBox_.DeleteItem(i);
                }
            }
        }
    }

    void switchDoc(LPCTSTR fileId)
    {
        if (::IsWindow(wbDocListBox_.m_hWnd)) {
            int count = wbDocListBox_.GetCount();
            for (int i = 0; i < count; i++) {
                TCHAR szText[256];
                wbDocListBox_.GetItemText(i, szText, 256);
                if (0 == _tcscmp(szText, fileId)) {
                    wbDocListBox_.SetCurSel(i);
                }
            }
        }
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
        UPDATE_ELEMENT(IDM_PREV_PAGE, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDM_ADD_PAGE, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDM_DEL_PAGE, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(IDM_NEXT_PAGE, UPDUI_TOOLBAR)
    END_UPDATE_UI_MAP()

    BEGIN_MSG_MAP(WhiteboardWindow)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
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
        COMMAND_ID_HANDLER(IDM_WB_UNDO, OnWbToolUndo)
        COMMAND_ID_HANDLER(IDM_WB_REDO, OnWbToolRedo)
        COMMAND_ID_HANDLER(IDM_WB_FORE_COLOR, OnWbToolForeColor)
        COMMAND_ID_HANDLER(IDM_WB_BOLD, OnWbToolBold)
        COMMAND_ID_HANDLER(IDM_WB_ITALIC, OnWbToolItalic)
        COMMAND_ID_HANDLER(IDM_WB_FONT, OnWbToolFont)
        COMMAND_ID_HANDLER(IDM_WB_ERASER, OnWbToolEraser)
        COMMAND_ID_HANDLER(IDM_WB_BRUSH, OnWbToolBrush)
        COMMAND_ID_HANDLER(IDM_WB_IMAGE, OnWbToolImage)
        COMMAND_ID_HANDLER(IDM_WB_DOC, OnWbToolDoc)
        COMMAND_ID_HANDLER(IDM_WB_SNAPSHOT, OnWbToolSnapshot)
        COMMAND_ID_HANDLER(IDM_PREV_PAGE, OnWbPrevPage)
        COMMAND_ID_HANDLER(IDM_ADD_PAGE, OnWbAddPage)
        COMMAND_ID_HANDLER(IDM_DEL_PAGE, OnWbDelPage)
        COMMAND_ID_HANDLER(IDM_NEXT_PAGE, OnWbNextPage)
        COMMAND_HANDLER(IDM_TB_COMBO_LINE_WIDTH, CBN_SELCHANGE, OnCbnSelchangeLineWidth)
        COMMAND_HANDLER(IDM_TB_COMBO_WBDOC_LIST, CBN_SELCHANGE, OnCbnSelchangeWBDoc)
        CHAIN_MSG_MAP(CUpdateUI<WhiteboardWindow>)
        CHAIN_MSG_MAP(CFrameWindowImpl<WhiteboardWindow>)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CenterWindow(GetParent());

        HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_TOOLBAR_WB, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
        CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
        AddSimpleReBarBand(hWndToolBar);

        //{
        int nIndex = 16;
        TBBUTTONINFO tbbi = { 0 };
        tbbi.cbSize = sizeof(tbbi);
        tbbi.dwMask = TBIF_SIZE | TBIF_BYINDEX;
        tbbi.cx = 60;
        ::SendMessage(hWndToolBar, TB_SETBUTTONINFO, nIndex, (LPARAM)&tbbi);
        RECT rect;
        ::SendMessage(hWndToolBar, TB_GETITEMRECT, nIndex, (LPARAM)&rect);
        rect.top = 1;
        rect.bottom = rect.top + 120;
        lineWidthBox_.Create(hWndToolBar, rect, 0, CBS_DROPDOWNLIST | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_CHILD, 0, IDM_TB_COMBO_LINE_WIDTH);
        lineWidthBox_.AddItem(_T("Line 1"), -1, -1, 0);
        lineWidthBox_.AddItem(_T("Line 2"), -1, -1, 0);
        lineWidthBox_.AddItem(_T("Line 3"), -1, -1, 0);
        lineWidthBox_.AddItem(_T("Line 4"), -1, -1, 0);
        lineWidthBox_.AddItem(_T("Line 5"), -1, -1, 0);
        lineWidthBox_.SetCurSel(lineWidth_ - 1);

        nIndex = 24;
        tbbi.cx = 120;
        ::SendMessage(hWndToolBar, TB_SETBUTTONINFO, nIndex, (LPARAM)&tbbi);
        ::SendMessage(hWndToolBar, TB_GETITEMRECT, nIndex, (LPARAM)&rect);
        rect.top = 1;
        rect.bottom = rect.top + 120;
        wbDocListBox_.Create(hWndToolBar, rect, 0, CBS_DROPDOWNLIST | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_CHILD, 0, IDM_TB_COMBO_WBDOC_LIST);
        controller_->refreshDocs();
        //}

        m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);

        UIAddToolBar(hWndToolBar);

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

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        //CPaintDC dc(m_hWnd);

        //TODO: Add your drawing code here

        return 0;
    }

protected:
    LRESULT OnWbToolSelect(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_SELECT);
        controller_->onToolSelect();
        return 0;
    }

    LRESULT OnWbToolText(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_TEXT);
        controller_->onToolText();
        return 0;
    }

    LRESULT OnWbToolPencil(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_PEN);
        controller_->onToolPencil();
        return 0;
    }

    LRESULT OnWbToolRectangle(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_RECTANGLE);
        controller_->onToolRectangle();
        return 0;
    }

    LRESULT OnWbToolLine(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_LINE);
        controller_->onToolLine();
        return 0;
    }

    LRESULT OnWbToolEllipse(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_ELLIPSE);
        controller_->onToolEllipse();
        return 0;
    }

    LRESULT OnWbToolArrow(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_ARROW);
        controller_->onToolArrow();
        return 0;
    }

    LRESULT OnWbToolArc(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_ARC);
        controller_->onToolArc();
        return 0;
    }

    LRESULT OnWbToolCurve(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_CURVE);
        controller_->onToolCurve();
        return 0;
    }

    LRESULT OnWbToolPolyline(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_POLYLINE);
        controller_->onToolPolyline();
        return 0;
    }

    LRESULT OnWbToolPolygon(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_POLYGON);
        controller_->onToolPolygon();
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

    LRESULT OnWbToolEraser(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_ERASER);
        controller_->onToolEraser();
        return 0;
    }

    LRESULT OnWbToolBrush(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CheckToolBox(IDM_WB_BRUSH);
        controller_->onToolBrush();
        return 0;
    }

    LRESULT OnWbToolImage(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CFileDialog dlg(TRUE);
        if (dlg.DoModal() == IDOK) {
            LPWSTR lpFilePath = dlg.m_ofn.lpstrFile;
            std::string path = pano::utils::utf8_encode(lpFilePath);
            controller_->onToolImage(path.c_str());
        }
        return 0;
    }

    LRESULT OnWbToolDoc(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CFileDialog dlg(TRUE);
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

    LRESULT OnWbPrevPage(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onPrevPage();
        return 0;
    }

    LRESULT OnWbAddPage(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onAddPage();
        return 0;
    }

    LRESULT OnWbDelPage(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onDelPage();
        return 0;
    }

    LRESULT OnWbNextPage(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        controller_->onNextPage();
        return 0;
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
        TCHAR szText[256];
        wbDocListBox_.GetItemText(idx, szText, 256);
        std::string docStr = pano::utils::utf8_encode(szText);
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
        SetWindowText(_T("WhiteBoard"));
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

protected:
    WBViewController* controller_;
    OnDestroyObserver m_observer;

    int checkedButtonId_ = 0;

    bool enableBold_ = false;
    bool enableItalic_ = false;
    panortc::WBFontStyle fontStyle_ = panortc::WBFontStyle::NORMAL;
    int fontSize_ = 12;
    COLORREF foreColor_ = RGB(0, 0, 0);

    int lineWidth_ = 1;
    CComboBoxEx lineWidthBox_;
    CComboBoxEx wbDocListBox_;
};

