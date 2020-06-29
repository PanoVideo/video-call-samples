#pragma once
#include <atlwin.h>
#include <atlframe.h>
#include <functional>

class VideoRenderWindow : public CFrameWindowImpl<VideoRenderWindow>
{
public:
    BEGIN_MSG_MAP(VideoRenderWindow)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CenterWindow(GetParent());
        OnWindowCreate();
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
        auto ret = OnWindowDestroy();
        bHandled = ret == 0;
        return ret;
    }

    LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        CClientDC dc(m_hWnd);
        RECT rc;
        GetClientRect(&rc);
        CBrush bkgBrush((HBRUSH)GetStockObject(DKGRAY_BRUSH));
        auto old = dc.SelectBrush(bkgBrush);
        BOOL bRes = dc.PatBlt(0, 0, rc.right - rc.left, rc.bottom - rc.top, PATCOPY);
        dc.SelectBrush(old);
        return 1;
    }

    virtual void OnWindowCreate()
    {

    }

    virtual LRESULT OnWindowClose()
    {
        DestroyWindow();
        return 0;
    }

    virtual LRESULT OnWindowDestroy()
    {
        return 0;
    }

    void SetWindowTitle(const std::string &title);

    void close()
    {
        OnWindowClose();
    }

    virtual ~VideoRenderWindow()
    {
        if (m_hWnd && IsWindow()) {
            DestroyWindow();
            m_hWnd = nullptr;
        }
    }
};

