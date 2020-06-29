#pragma once

#include "RtcViewController.h"

#include "PreviewVideoWindow.h"
#include "LocalVideoWindow.h"
#include "RemoteVideoWindow.h"
#include "WBViewController.h"
#include "RtcTester.h"

#include "UIThreadExecutor.h"

#include <string>
#include <unordered_map>

class CMainDlg;

class MainViewController : public RtcViewController
{
public:
    MainViewController();

    bool init(CMainDlg *mainView);
    bool joinChannel();
    void leaveChannel();
    void startPreviewVideo();
    void stopPreviewVideo();
    void startLocalVideo();
    void stopLocalVideo();
    void startLocalAudio();
    void stopLocalAudio();
    void startScreen();
    void stopScreen();
    void updateScreenSource();
    void updateScreenOptimization();
    bool muteVideo();
    bool unmuteVideo();
    bool muteAudio();
    bool unmuteAudio();
    bool muteScreen();
    bool unmuteScreen();
    bool startAudioDump();
    bool stopAudioDump();
    bool setAudioEqualization(const int option);
    bool setAudioReverb(const int option);
    bool startAudioDeviceTest();
    bool stopAudioDeviceTest();
    void uninit();

    void updateAudioRecordDevice();
    void updateAudioPlayoutDevice();
    void updateVideoCaptureDevice();

    void startWhiteboard();
    void stopWhiteboard();

    void updateLogsUploadFlag();
    void updateAudioDumpUploadFlag();
    void updateVideoFrameRate();

    void startAudioMixing(const char* filename);
    void stopAudioMixing();
    void snapshotVideo(const char* outputDir);

    bool isChannelJoined() const { return channelJoined_; }
    bool isVideoStarted() const { return videoStarted_; }
    bool isAudioStarted() const { return audioStarted_; }
    bool isScreenStarted() const { return screenStarted_; }
    bool isVideoMuted() const { return videoMuted_; }
    bool isAudioMuted() const { return audioMuted_; }
    bool isScreenMuted() const { return screenMuted_; }
    bool isAudioDumpStarted() const { return audioDumpStarted_; }
    bool isAudioTestStarted() const { return audioTestStarted_; }
    bool isAudioMixingStarted() const { return audioMixingStarted_; }

    void onMainViewClosing();

public:
    void onAuthFailure(panortc::QResult result) override;
    void onChannelJoinConfirm(panortc::QResult result) override;
    void onChannelLeaveIndication(panortc::QResult result) override;
    void onUserJoinIndication(uint64_t userId) override;
    void onUserLeaveIndication(uint64_t userId, panortc::UserLeaveReason reason) override;
    void onUserVideoStart(uint64_t userId, panortc::VideoProfileType maxProfile) override;
    void onUserVideoStop(uint64_t userId) override;
    void onUserScreenStart(uint64_t userId) override;
    void onUserScreenStop(uint64_t userId) override;

    void onWhiteboardAvailable() override;
    void onWhiteboardUnavailable() override;
    void onWhiteboardStart() override;
    void onWhiteboardStop() override;

    void onAudioDeviceStateChanged(const char deviceID[panortc::kMaxDeviceIDLength],
        panortc::AudioDeviceType deviceType,
        panortc::AudioDeviceState deviceState) override;
    void onVideoDeviceStateChanged(const char deviceID[panortc::kMaxDeviceIDLength],
        panortc::VideoDeviceType deviceType,
        panortc::VideoDeviceState deviceState) override;

    void onVideoSendStats(panortc::VideoSendStats &stats) override;
    void onVideoRecvStats(panortc::VideoRecvStats &stats) override;
    void onScreenSendStats(panortc::ScreenSendStats &stats) override;
    void onScreenRecvStats(panortc::ScreenRecvStats &stats) override;
    void onAudioSendStats(panortc::AudioSendStats &stats) override;
    void onAudioRecvStats(panortc::AudioRecvStats &stats) override;
    void onVideoSendBweStats(panortc::VideoSendBweStats &stats) override;
    void onVideoRecvBweStats(panortc::VideoRecvBweStats &stats) override;

    void onWhiteboardCreateDoc(panortc::QResult result, const char* fileId) override;
    void onWhiteboardDeleteDoc(panortc::QResult result, const char* fileId) override;
    void onWhiteboardSwitchDoc(panortc::QResult result, const char* fileId) override;

    void asyncRunOnUIThread(Task t) override
    {
        if (taskExecutor_) {
            taskExecutor_->async(std::move(t));
        }
    }

protected:
    void clearCallState();

protected:
    CMainDlg* mainView_;

    std::unique_ptr<UIThreadExecutor> taskExecutor_;

    std::string previewDeviceId_;
    std::string localVideoDeviceId_;
    PreviewVideoWindow previewVideoView_;
    LocalVideoWindow localVideoView_;
    VideoStats localVideoStats_;

    bool channelJoined_ = false;
    bool videoStarted_ = false;
    bool audioStarted_ = false;
    bool screenStarted_ = false;

    bool videoMuted_ = false;
    bool audioMuted_ = false;
    bool screenMuted_ = false;

    bool audioDumpStarted_ = false;
    bool audioTestStarted_ = false;
    bool audioMixingStarted_ = false;

    std::unordered_map<uint64_t, std::unique_ptr<RemoteVideoWindow>> remoteUserVideoViewMap_;
    std::unordered_map<uint64_t, std::unique_ptr<RemoteVideoWindow>> remoteUserScreenViewMap_;
    WBViewController whiteboardController_;
};

