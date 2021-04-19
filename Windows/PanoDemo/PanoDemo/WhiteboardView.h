
#pragma once

#include <atlwin.h>

class CWhiteboardView : public CWindowImpl<CWhiteboardView>
{
public:
    DECLARE_WND_CLASS(NULL)

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        pMsg;
        return FALSE;
    }

    BEGIN_MSG_MAP(CView)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
        MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        //CPaintDC dc(m_hWnd);
        bHandled = FALSE;

        return 0;
    }

    LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        RECT rect;
        GetClientRect(&rect);
        CDCHandle myDC((HDC)wParam);
        CBrush myBrush;
        myBrush.CreateSolidBrush(RGB(255, 255, 255));
        auto hOld = myDC.SelectBrush(myBrush);
        BOOL bRes = myDC.PatBlt(0, 0, rect.right - rect.left, rect.bottom - rect.top, PATCOPY);
        myDC.SelectBrush(hOld);
        bHandled = bRes;
        return bRes;
    }

    LRESULT OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        HWND hParent = GetParent();
        ::PostMessage(hParent, WM_HSCROLL, wParam, NULL);
        return 0;
    }

    LRESULT OnVScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        HWND hParent = GetParent();
        ::PostMessage(hParent, WM_VSCROLL, wParam, NULL);
        return 0;
    }
};
