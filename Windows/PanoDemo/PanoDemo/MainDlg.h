// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlframe.h>

#include "resource.h"
#include "AboutDlg.h"
#include "PanoRtcDefs.h"
#include "PanoRtcDevice.h"

#include <string>
#include <vector>


using NameIDVector = std::vector<std::pair<std::string, std::string>>;
struct ScreenSource
{
    panortc::ScreenSourceID ssid;
    panortc::ScreenSourceType type;
    std::string name;
};

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
protected:
    bool m_bChannelStarted = false;
    bool m_bPreviewVideoStarted = false;

    NameIDVector m_vecRecordDevices;
    NameIDVector m_vecPlayoutDevices;
    NameIDVector m_vecVideoDevices;
    std::vector<ScreenSource> m_vecScreenSources;

public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateChildWindows();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        COMMAND_HANDLER(IDC_START_CALL, BN_CLICKED, OnBnClickedStartCall)
        COMMAND_HANDLER(IDC_BTN_CAMERA_PREVIEW, BN_CLICKED, OnBnClickedCameraPreview)
        COMMAND_HANDLER(IDC_BTN_VIDEO_MUTE, BN_CLICKED, OnBnClickedBtnVideoMute)
        COMMAND_HANDLER(IDC_BTN_AUDIO_MUTE, BN_CLICKED, OnBnClickedBtnAudioMute)
        COMMAND_HANDLER(IDC_BTN_SHARE_START, BN_CLICKED, OnBnClickedBtnShareStart)
        COMMAND_HANDLER(IDC_BTN_SHARE_MUTE, BN_CLICKED, OnBnClickedBtnShareMute)
        COMMAND_HANDLER(IDC_COMBO_SOURCE, CBN_SELCHANGE, OnCbnSelchangeComboSource)
        COMMAND_HANDLER(IDC_CHECK_ENABLE_VIDEO, BN_CLICKED, OnBnClickedCheckEnableVideo)
        COMMAND_HANDLER(IDC_CHECK_ENABLE_AUDIO, BN_CLICKED, OnBnClickedCheckEnableAudio)
        COMMAND_HANDLER(IDC_COMBO_SOURCE, CBN_DROPDOWN, OnCbnDropdownComboSource)
        COMMAND_HANDLER(IDC_CHECK_SHARE_MOTION, BN_CLICKED, OnBnClickedCheckShareMotion)
        COMMAND_HANDLER(IDC_COMBO_MICROPHONE, CBN_SELCHANGE, OnCbnSelchangeComboMicrophone)
        COMMAND_HANDLER(IDC_COMBO_SPEAKER, CBN_SELCHANGE, OnCbnSelchangeComboSpeaker)
        COMMAND_HANDLER(IDC_COMBO_CAMERA, CBN_SELCHANGE, OnCbnSelchangeComboCamera)
        COMMAND_HANDLER(IDC_BTN_YUV_BROWSE, BN_CLICKED, OnBnClickedBtnYuvBrowse)
    END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		UIAddChildWindowContainer(m_hWnd);

        InitDemoUI();
        InitDemoSettings();

		return TRUE;
	}

    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        OnClose();
        CloseDialog(0);
        return 0;
    }

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		return 0;
	}

    std::string _GetDlgItemText(int nID) const;
    std::wstring _GetDlgItemTextW(int nID) const;

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: Add validation code 
		CloseDialog(wID);
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CloseDialog(wID);
		return 0;
	}

	void CloseDialog(int nVal)
	{
		DestroyWindow();
		::PostQuitMessage(nVal);
	}
    LRESULT OnBnClickedStartCall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedCameraPreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    std::string GetChannelId() const;
    uint64_t GetUserId() const;
    std::string GetUserName1() const;
    bool IsMode_1v1() const;
    bool IsVideoEnabled() const;
    bool IsAudioEnabled() const;
    bool IsOptimizedForMotion() const;
    bool IsWhiteboardEnabled() const;

    std::string GetCurrentRecordDevice() const;
    std::string GetCurrentPlayoutDevice() const;
    std::string GetCurrentVideoDevice() const;
    panortc::VideoProfileType GetCurrentVideoProfile() const;
    panortc::VideoScalingMode GetCurrentVideoScalingMode() const;
    bool IsMirror() const;
    std::wstring GetYuvFile() const;


    ScreenSource GetCurrentScreenSource() const;

    void notifyChannelJoined();
    void notifyChannelLeft();
    void notifyPreviewVideoStopped();
    void notifyAudioDeviceChanged();
    void notifyVideoDeviceChanged();
	void notifyVideoStopped();

protected:
    void InitDemoUI();
    void InitDemoSettings();
    void RefreshRtcDevices();
    void RefreshAudioDevices();
    void RefreshVideoDevices();
    void RefreshScreenSources();

    void OnClose();

public:
    LRESULT OnBnClickedBtnVideoMute(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedBtnAudioMute(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedBtnShareStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedBtnShareMute(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCbnSelchangeComboSource(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedCheckEnableVideo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedCheckEnableAudio(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCbnDropdownComboSource(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedCheckShareMotion(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCbnSelchangeComboMicrophone(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCbnSelchangeComboSpeaker(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCbnSelchangeComboCamera(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnEnChangeEditChannelId(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedBtnYuvBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedCheckEnableWhiteboard(WORD, WORD, HWND, BOOL&);
};
