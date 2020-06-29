#include "stdafx.h"
#include "MainDlg.h"
#include "RtcTester.h"
#include "utils.h"
#include "PanoRtcEngine.h"
#include "MainViewController.h"
#include "RtcTester.h"

#include "atlctrls.h"
#include <windows.h>
#include <Shlobj.h>
#include <Commctrl.h>
#include <random>
#include <map>

using namespace pano::utils;
using namespace panortc;

const std::string kDemoChannelId = "1234";
const uint64_t kDemoUserId = 0x1000;

extern MainViewController _MainController;


void CMainDlg::InitDemoUI()
{
    
}

void CMainDlg::InitDemoSettings()
{
    std::mt19937 rand_engine{ std::random_device{}() };
    std::uniform_int_distribution<uint32_t> dist;
    uint64_t localUserId = 15000 + dist(rand_engine) % 1000;

    auto wstr = utf8_decode(kDemoChannelId);
    SetDlgItemText(IDC_EDIT_CHANNEL_ID, wstr.c_str());
    wstr = std::to_wstring(localUserId);
    SetDlgItemText(IDC_EDIT_USER_ID, wstr.c_str());
    CheckDlgButton(IDC_CHECK_CHAN_MODE, false);

    RefreshRtcDevices();

    CComboBox cbResolution(GetDlgItem(IDC_COMBO_RESOLUTION));
    cbResolution.Clear();
    cbResolution.AddString(_T("Lowest"));
    cbResolution.AddString(_T("Low"));
    cbResolution.AddString(_T("Standard"));
    cbResolution.AddString(_T("720P"));
    cbResolution.AddString(_T("1080P"));
    cbResolution.AddString(_T("Auto"));
    cbResolution.SetCurSel(5);

    CComboBox cbScalingMode(GetDlgItem(IDC_COMBO_SCALING_MODE));
    cbScalingMode.Clear();
    cbScalingMode.AddString(_T("Fit"));
    cbScalingMode.AddString(_T("FullFill"));
    cbScalingMode.AddString(_T("CropFill"));
    cbScalingMode.SetCurSel(0);

    CheckDlgButton(IDC_CHECK_ENABLE_VIDEO, true);
    CheckDlgButton(IDC_CHECK_MIRROR, true);
    CheckDlgButton(IDC_CHECK_ENABLE_AUDIO, true);
    CheckDlgButton(IDC_CHECK_ENABLE_WHITEBOARD, true);
}

void CMainDlg::RefreshRtcDevices()
{
    RefreshAudioDevices();
    RefreshVideoDevices();
    RefreshScreenSources();
}

void CMainDlg::RefreshAudioDevices()
{
    CComboBox cbMicrophone(GetDlgItem(IDC_COMBO_MICROPHONE));
    cbMicrophone.ResetContent();
    m_vecRecordDevices.clear();

    int idx = 0;
    RtcTester::instance().enumRecordDevices([&cbMicrophone, &idx, this]
        (const std::string &deviceName, const std::string &deviceId) {
        m_vecRecordDevices.emplace_back(deviceName, deviceId);
        auto wstr = utf8_decode(deviceName);
        cbMicrophone.AddString(wstr.c_str());
        cbMicrophone.SetItemData(idx, idx);
        ++idx;
    });
    cbMicrophone.SetCurSel(0);

    CComboBox cbSpeaker(GetDlgItem(IDC_COMBO_SPEAKER));
    cbSpeaker.ResetContent();
    m_vecPlayoutDevices.clear();

    idx = 0;
    RtcTester::instance().enumPlayoutDevices([&cbSpeaker, &idx, this]
        (const std::string &deviceName, const std::string &deviceId) {
        m_vecPlayoutDevices.emplace_back(deviceName, deviceId);
        auto wstr = utf8_decode(deviceName);
        cbSpeaker.AddString(wstr.c_str());
        cbSpeaker.SetItemData(idx, idx);
        ++idx;
    });
    cbSpeaker.SetCurSel(0);
}

void CMainDlg::RefreshVideoDevices()
{
    CComboBox cbCamera(GetDlgItem(IDC_COMBO_CAMERA));
    cbCamera.ResetContent();
    m_vecVideoDevices.clear();

    int idx = 0;
    RtcTester::instance().enumVideoDevices([&cbCamera, &idx, this]
    (const std::string &deviceName, const std::string &deviceId) {
        m_vecVideoDevices.emplace_back(deviceName, deviceId);
        auto wstr = utf8_decode(deviceName);
        cbCamera.AddString(wstr.c_str());
        cbCamera.SetItemData(idx, idx);
        ++idx;
    });
    cbCamera.SetCurSel(0);
}

void CMainDlg::RefreshScreenSources()
{
    CComboBox cbCamera(GetDlgItem(IDC_COMBO_SOURCE));
    cbCamera.ResetContent();
    m_vecScreenSources.clear();

    std::map<std::string, ScreenSourceID> sourceMap;
    RtcTester::instance().enumDisplays([&sourceMap]
    (ScreenSourceID ssid, const char *name) {
        sourceMap.emplace(std::string(name), ssid);
    });

    for (auto &kv : sourceMap) {
        m_vecScreenSources.emplace_back(ScreenSource{ kv.second, ScreenSourceType::Display, kv.first });
    }

    sourceMap.clear();
    RtcTester::instance().enumApplications([&sourceMap]
    (ScreenSourceID ssid, const char *name) {
        sourceMap.emplace(std::string(name), ssid);
    });

    for (auto &kv : sourceMap) {
        m_vecScreenSources.emplace_back(ScreenSource{ kv.second, ScreenSourceType::Application, kv.first });
    }

    int idx = 0;
    for (auto &s : m_vecScreenSources) {
        auto wstr = utf8_decode(s.name);
        if (s.type != ScreenSourceType::Display) {
            wstr = L"[" + std::to_wstring(s.ssid) + L"] - " + wstr;
        }
        cbCamera.AddString(wstr.c_str());
        cbCamera.SetItemData(idx, idx);
        ++idx;
    }
    cbCamera.SetCurSel(0);
}

std::string CMainDlg::_GetDlgItemText(int nID) const
{
    ATLASSERT(::IsWindow(m_hWnd));

    auto hItem = GetDlgItem(nID);
    if (hItem != NULL)
    {
        int nLength;
        TCHAR szText[256];
        TCHAR *pszText = szText;

        nLength = ::GetWindowTextLength(hItem);
        if (nLength > sizeof(szText) - 1) {
            pszText = new TCHAR[nLength + 1];
        }
        nLength = ::GetWindowText(hItem, pszText, nLength + 1);
        auto str = utf8_encode(pszText, nLength);
        if (pszText != szText) {
            delete[] pszText;
        }
        return str;
    }
    else
    {
        return "";
    }
}

LRESULT CMainDlg::OnBnClickedStartCall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    m_bChannelStarted = !m_bChannelStarted;
    SetDlgItemText(IDC_START_CALL, m_bChannelStarted ? _T("Leave Channel") : _T("Join Channel"));
    if (m_bChannelStarted) {
        _MainController.joinChannel();
    }
    else {
        _MainController.leaveChannel();
        SetDlgItemText(IDC_BTN_VIDEO_MUTE, _T("Mute Video"));
        SetDlgItemText(IDC_BTN_AUDIO_MUTE, _T("Mute Audio"));
        SetDlgItemText(IDC_BTN_SHARE_START, _T("Start Sharing"));
        SetDlgItemText(IDC_BTN_SHARE_MUTE, _T("Mute Sharing"));
        SetDlgItemText(IDC_BTN_AUDIO_DUMP, _T("Start Dump"));
        GetDlgItem(IDC_BTN_VIDEO_MUTE).EnableWindow(IsDlgButtonChecked(IDC_CHECK_ENABLE_VIDEO));
        GetDlgItem(IDC_BTN_AUDIO_MUTE).EnableWindow(IsDlgButtonChecked(IDC_CHECK_ENABLE_AUDIO));
    }
    return 0;
}


LRESULT CMainDlg::OnBnClickedCameraPreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    m_bPreviewVideoStarted = !m_bPreviewVideoStarted;
    SetDlgItemText(IDC_BTN_CAMERA_PREVIEW, m_bPreviewVideoStarted ? _T("Stop Preview") : _T("Preview"));
    if (m_bPreviewVideoStarted) {
        _MainController.startPreviewVideo();
    }
    else {
        _MainController.stopPreviewVideo();
    }
    return 0;
}

std::string CMainDlg::GetChannelId() const
{
    return _GetDlgItemText(IDC_EDIT_CHANNEL_ID);
}

uint64_t CMainDlg::GetUserId() const
{
    return std::stoull(_GetDlgItemText(IDC_EDIT_USER_ID));
}

std::string CMainDlg::GetUserName1() const
{
    return _GetDlgItemText(IDC_EDIT_USER_NAME);
}

bool CMainDlg::IsMode_1v1() const
{
    return IsDlgButtonChecked(IDC_CHECK_CHAN_MODE);
}

bool CMainDlg::IsVideoEnabled() const
{
    return IsDlgButtonChecked(IDC_CHECK_ENABLE_VIDEO);
}

bool CMainDlg::IsAudioEnabled() const
{
    return IsDlgButtonChecked(IDC_CHECK_ENABLE_AUDIO);
}

bool CMainDlg::IsOptimizedForMotion() const
{
    return IsDlgButtonChecked(IDC_CHECK_SHARE_MOTION);
}

bool CMainDlg::IsWhiteboardEnabled() const
{
    return IsDlgButtonChecked(IDC_CHECK_ENABLE_WHITEBOARD);
}

std::string CMainDlg::GetCurrentRecordDevice() const
{
    CComboBox cbMicrophone(GetDlgItem(IDC_COMBO_MICROPHONE));
    auto sel = cbMicrophone.GetCurSel();
    if (sel == CB_ERR || sel >= m_vecRecordDevices.size()) {
        return "";
    }
    return m_vecRecordDevices[sel].second;
}

std::string CMainDlg::GetCurrentPlayoutDevice() const
{
    CComboBox cbSpeaker(GetDlgItem(IDC_COMBO_SPEAKER));
    auto sel = cbSpeaker.GetCurSel();
    if (sel == CB_ERR || sel >= m_vecPlayoutDevices.size()) {
        return "";
    }
    return m_vecPlayoutDevices[sel].second;
}

std::string CMainDlg::GetCurrentVideoDevice() const
{
    CComboBox cbCamera(GetDlgItem(IDC_COMBO_CAMERA));
    auto sel = cbCamera.GetCurSel();
    if (sel == CB_ERR || sel >= m_vecVideoDevices.size()) {
        return "";
    }
    return m_vecVideoDevices[sel].second;
}

VideoProfileType CMainDlg::GetCurrentVideoProfile() const
{
    CComboBox cbResolution(GetDlgItem(IDC_COMBO_RESOLUTION));
    auto sel = cbResolution.GetCurSel();
    auto profile = VideoProfileType::HD720P;
    if (sel != CB_ERR && sel <= int(VideoProfileType::Max)) {
        profile = VideoProfileType(sel);
    }
    return profile;
}

VideoScalingMode CMainDlg::GetCurrentVideoScalingMode() const
{
    CComboBox cbScalingMode(GetDlgItem(IDC_COMBO_SCALING_MODE));
    auto sel = cbScalingMode.GetCurSel();
    auto mode = VideoScalingMode::Fit;
    if (sel != CB_ERR && sel <= int(VideoScalingMode::CropFill)) {
        mode = VideoScalingMode(sel);
    }
    return mode;
}

bool CMainDlg::IsMirror() const
{
    return IsDlgButtonChecked(IDC_CHECK_MIRROR);
}

ScreenSource CMainDlg::GetCurrentScreenSource() const
{
    CComboBox cbSorce(GetDlgItem(IDC_COMBO_SOURCE));
    auto sel = cbSorce.GetCurSel();
    if (sel == CB_ERR || sel >= m_vecScreenSources.size()) {
        auto hMonitor = ::MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
        auto ssid = reinterpret_cast<ScreenSourceID>(hMonitor);
        return { ssid, ScreenSourceType::Display, ""};
    }
    return m_vecScreenSources[sel];
}

void CMainDlg::notifyChannelJoined()
{

}

void CMainDlg::notifyChannelLeft()
{
    m_bChannelStarted = false;
    SetDlgItemText(IDC_START_CALL, _T("Join Channel"));
    SetDlgItemText(IDC_BTN_VIDEO_MUTE, _T("Mute Video"));
    SetDlgItemText(IDC_BTN_AUDIO_MUTE, _T("Mute Audio"));
    SetDlgItemText(IDC_BTN_SHARE_START, _T("Start Sharing"));
    SetDlgItemText(IDC_BTN_SHARE_MUTE, _T("Mute Sharing"));
    SetDlgItemText(IDC_BTN_AUDIO_DUMP, _T("Start Dump"));
    GetDlgItem(IDC_BTN_VIDEO_MUTE).EnableWindow(IsDlgButtonChecked(IDC_CHECK_ENABLE_VIDEO));
    GetDlgItem(IDC_BTN_AUDIO_MUTE).EnableWindow(IsDlgButtonChecked(IDC_CHECK_ENABLE_AUDIO));
}

void CMainDlg::notifyPreviewVideoStopped()
{
    m_bPreviewVideoStarted = false;
    SetDlgItemText(IDC_BTN_CAMERA_PREVIEW, _T("Preview"));
}

void CMainDlg::notifyAudioDeviceChanged()
{
    RefreshAudioDevices();
}

void CMainDlg::notifyVideoDeviceChanged()
{
    RefreshVideoDevices();
}

void CMainDlg::notifyVideoStopped() {
	CheckDlgButton(IDC_CHECK_ENABLE_VIDEO, false);
}

void CMainDlg::OnClose()
{
    _MainController.onMainViewClosing();
}


LRESULT CMainDlg::OnBnClickedBtnVideoMute(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    if (_MainController.isVideoMuted()) {
        _MainController.unmuteVideo();
    }
    else {
        _MainController.muteVideo();
    }
    SetDlgItemText(IDC_BTN_VIDEO_MUTE, _MainController.isVideoMuted() ? _T("Unmute Video") : _T("Mute Video"));
    return 0;
}

LRESULT CMainDlg::OnBnClickedBtnAudioMute(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    if (_MainController.isAudioMuted()) {
        _MainController.unmuteAudio();
    }
    else {
        _MainController.muteAudio();
    }
    SetDlgItemText(IDC_BTN_AUDIO_MUTE, _MainController.isAudioMuted() ? _T("Unmute Audio") : _T("Mute Audio"));
    return 0;
}

LRESULT CMainDlg::OnBnClickedBtnShareStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    if (_MainController.isScreenStarted()) {
        _MainController.stopScreen();
    }
    else {
        _MainController.startScreen();
    }
    SetDlgItemText(IDC_BTN_SHARE_START, _MainController.isScreenStarted() ? _T("Stop Sharing") : _T("Start Sharing"));
    return 0;
}

LRESULT CMainDlg::OnBnClickedBtnShareMute(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    if (_MainController.isScreenMuted()) {
        _MainController.unmuteScreen();
    }
    else {
        _MainController.muteScreen();
    }
    SetDlgItemText(IDC_BTN_SHARE_MUTE, _MainController.isScreenMuted() ? _T("Unmute Sharing") : _T("Mute Sharing"));
    return 0;
}

LRESULT CMainDlg::OnCbnSelchangeComboSource(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    _MainController.updateScreenSource();
    return 0;
}

LRESULT CMainDlg::OnBnClickedCheckEnableVideo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    if (!_MainController.isChannelJoined()) {
        GetDlgItem(IDC_BTN_VIDEO_MUTE).EnableWindow(IsDlgButtonChecked(IDC_CHECK_ENABLE_VIDEO));
	}
	else {
		if (IsDlgButtonChecked(IDC_CHECK_ENABLE_VIDEO)) {
			if (!_MainController.isVideoStarted()) {
			    _MainController.startLocalVideo();
			}
		}
		else {
			if (_MainController.isVideoStarted()) {
			    _MainController.stopLocalVideo();
			}
		}
	}
    return 0;
}

LRESULT CMainDlg::OnBnClickedCheckEnableAudio(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    if (!_MainController.isChannelJoined()) {
        GetDlgItem(IDC_BTN_AUDIO_MUTE).EnableWindow(IsDlgButtonChecked(IDC_CHECK_ENABLE_AUDIO));
    }
    return 0;
}

LRESULT CMainDlg::OnCbnDropdownComboSource(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    RefreshScreenSources();
    return 0;
}

LRESULT CMainDlg::OnBnClickedCheckShareMotion(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    _MainController.updateScreenOptimization();
    return 0;
}

LRESULT CMainDlg::OnCbnSelchangeComboMicrophone(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    _MainController.updateAudioRecordDevice();
    return 0;
}

LRESULT CMainDlg::OnCbnSelchangeComboSpeaker(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    _MainController.updateAudioPlayoutDevice();
    return 0;
}

LRESULT CMainDlg::OnCbnSelchangeComboCamera(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    _MainController.updateVideoCaptureDevice();
    return 0;
}
