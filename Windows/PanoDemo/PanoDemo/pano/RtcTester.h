#pragma once

#include "PanoRtcEngine.h"
#include "utils.h"
#include "RtcViewController.h"
#include "UserManager.h"

#include <memory>
#include <string>
#include <functional>

class TokenGetter;

class RtcTester : public panortc::RtcEngine::Callback
                , public panortc::RtcWhiteboard::Callback
                , public panortc::MediaStatsObserver
{
public:
    RtcTester();
    ~RtcTester();

    bool init(RtcViewController *vc, 
        const std::string &appId);
    bool start(std::string appId, 
        std::string token,
        std::string channelId,
        uint64_t userId, 
        std::string userName,
        bool mode_1v1);
    void stop();
    void uninit();

    bool startPreview(const std::string &deviceId, void *view, panortc::RenderConfig &config);
    void stopPreview(const std::string &deviceId);
    void startVideo(const std::string &deviceId, void *view, panortc::RenderConfig &config);
    void stopVideo();
    void startAudio(const std::string &recordDeviceId, const std::string &playoutDeviceId);
    void stopAudio();
    void setAudioRecordDevice(const std::string &recordDeviceId);
    void setAudioPlayoutDevice(const std::string &playoutDeviceId);
    void setVideoCaptureDevice(const std::string &cameraDeviceId);
    panortc::QResult subscribeVideo(uint64_t userId, void *view, panortc::RenderConfig &config);
    void unsubscribeVideo(uint64_t userId);

    void startScreen(panortc::ScreenSourceID sourceId, panortc::ScreenSourceType type);
    void updateScreenSource(panortc::ScreenSourceID sourceId, panortc::ScreenSourceType type);
    void stopScreen();
    panortc::QResult subscribeScreen(uint64_t userId, void *view);
    void unsubscribeScreen(uint64_t userId);
    void updateScreenOptimization(bool forMotion);

    void startWhiteboard(void *view);
    void stopWhiteboard();

    bool muteAudio();
    bool unmuteAudio();
    bool muteVideo();
    bool unmuteVideo();
    bool muteScreen();
    bool unmuteScreen();
    void snapshotVideo(const char* outputDir, uint64_t userId, bool mirror);

    void startAudioDump(const std::string &filePath);
    void stopAudioDump();
    void startAudioDeviceTest(const std::string &recordDeviceId, const std::string &playoutDeviceId);
    void stopAudioDeviceTest();

	//Audio Effect
    void setAudioFeatureOption(panortc::OptionType type,int option);
    void enableUploadLogs(bool enabled);
    void enableUploadAudioDump(bool enabled);
    void updateVideoFrameRate(bool lowFPS);

    template<typename DeviceCallback> // void(const std::string, const std::string)
    void enumRecordDevices(DeviceCallback &&cb)
    {
        if (rtcEngine_) {
            auto *audioDeviceMgr = rtcEngine_->getAudioDeviceManager();
            audioDeviceMgr->enumerateRecordDevices([](
                void *context,
                const char deviceName[kMaxDeviceNameLength],
                const char deviceId[kMaxDeviceNameLength]) {
                auto &cb = *static_cast<DeviceCallback*>(context);
                cb(deviceName, deviceId);
                return true;
            }, &cb);
        }
    }

    template<typename DeviceCallback> // void(const std::string, const std::string)
    void enumPlayoutDevices(DeviceCallback &&cb)
    {
        if (rtcEngine_) {
            auto *audioDeviceMgr = rtcEngine_->getAudioDeviceManager();
            audioDeviceMgr->enumeratePlayoutDevices([](
                void *context,
                const char deviceName[kMaxDeviceNameLength],
                const char deviceId[kMaxDeviceNameLength]) {
                auto &cb = *static_cast<DeviceCallback*>(context);
                cb(deviceName, deviceId);
                return true;
            }, &cb);
        }
    }

    template<typename DeviceCallback> // void(const std::string, const std::string)
    void enumVideoDevices(DeviceCallback &&cb)
    {
        if (rtcEngine_) {
            auto *videoDeviceMgr = rtcEngine_->getVideoDeviceManager();
            videoDeviceMgr->enumerateCaptureDevices([] (
                void *context, 
                const char deviceName[kMaxDeviceNameLength], 
                const char deviceId[kMaxDeviceNameLength]) {
                auto &cb = *static_cast<DeviceCallback*>(context);
                cb(deviceName, deviceId);
                return true;
            }, &cb);
        }
    }

    template<typename DisplayCallback> // void(ScreenSourceID, const std::string)
    void enumDisplays(DisplayCallback &&cb)
    {
        if (rtcEngine_) {
            auto *sourceMgr = rtcEngine_->getScreenSourceManager();
            sourceMgr->enumerateDisplays([](
                void *context,
                ScreenSourceID ssid,
                const char *name) {
                auto &cb = *static_cast<DisplayCallback*>(context);
                cb(ssid, name);
                return true;
            }, &cb);
        }
    }

    template<typename AppCallback> // void(ScreenSourceID, const std::string)
    void enumApplications(AppCallback &&cb)
    {
        if (rtcEngine_) {
            auto *sourceMgr = rtcEngine_->getScreenSourceManager();
            sourceMgr->enumerateApplications([](
                void *context,
                ScreenSourceID ssid,
                const char *name) {
                auto &cb = *static_cast<AppCallback*>(context);
                cb(ssid, name);
                return true;
            }, &cb);
        }
    }

    template<typename DocCallback> // void(const std::string)
    void enumDocs(DocCallback &&cb)
    {
        if (rtcEngine_) {
            auto *wbEngine = rtcEngine_->getWhiteboardEngine();
            wbEngine->enumerateFiles([](
                void *context,
                const char *fileId) {
                auto &cb = *static_cast<DocCallback*>(context);
                cb(fileId);
                return true;
            }, &cb);
        }
    }

    // Whiteboard command
    void setWbToolType(panortc::WBToolType type);
    void wbToolBoxUndo();
    void wbToolBoxRedo();
    void wbToolBoxBgImage(const char* imageUrl);
    void wbToolBoxCreateDoc(const char* docUrl);
    void wbToolBoxSwitchDoc(const char* docUrl);
    void wbToolBoxForeColor(int r, int g, int b);
    void wbToolBoxFontStyle(panortc::WBFontStyle style);
    void wbToolBoxFontSize(int size);
    void wbToolBoxLineWidth(int size);
    void wbToolBoxSnapshot(const char *outputDir);
    void wbToolBoxPrevPage();
    void wbToolBoxAddPage();
    void wbToolBoxDelPage();
    void wbToolBoxNextPage();
    const char* getCurrentDoc();

    bool createAudioMixingTask(int64_t taskId, const char *filename);
    void destroyAduioMixingTask(int64_t taskId);
    bool startAudioMixingTask(int64_t taskId);
    bool updateAudioMixingTask(int64_t taskId);
    bool stopAudioMixingTask(int64_t taskId);
    bool pauseAudioMixingTask(int64_t taskId);
    bool resumeAudioMixingTask(int64_t taskId);
    bool getAudioMixingDuration(int64_t taskId, int64_t *duration);
    bool getAudioMixingCurrentTimestamp(int64_t taskId, int64_t *timestamp);
    bool seekAudioMixingTask(int64_t taskId, int64_t timestamp);


    panortc::RtcEngine* getRtcEngine() { return rtcEngine_.get(); }
    UserManager& getUserManager() { return userMgr_; }

    static RtcTester& instance();

public: // RtcEngine::Callback
    void onChannelJoinConfirm(panortc::QResult result) override;
    void onChannelLeaveIndication(panortc::QResult result) override;
    void onChannelCountDown(uint32_t remain) override;
    void onUserJoinIndication(uint64_t userId, const char *userName) override;
    void onUserLeaveIndication(uint64_t userId, panortc::UserLeaveReason reason) override;
    void onUserAudioStart(uint64_t userId) override;
    void onUserAudioStop(uint64_t userId) override;
    void onUserAudioSubscribe(uint64_t userId, panortc::MediaSubscribeResult result) override;
    void onUserVideoStart(uint64_t userId, panortc::VideoProfileType maxProfile) override;
    void onUserVideoStop(uint64_t userId) override;
    void onUserVideoSubscribe(uint64_t userId, panortc::MediaSubscribeResult result) override;
    void onUserAudioMute(uint64_t userId) override;
    void onUserAudioUnmute(uint64_t userId) override;
    void onUserVideoMute(uint64_t userId) override;
    void onUserVideoUnmute(uint64_t userId) override;

    void onUserScreenStart(uint64_t userId) override;
    void onUserScreenStop(uint64_t userId) override;
    void onUserScreenSubscribe(uint64_t userId, panortc::MediaSubscribeResult result) override;
    void onUserScreenMute(uint64_t userId) override;
    void onUserScreenUnmute(uint64_t userId) override;

    void onWhiteboardAvailable() override;
    void onWhiteboardUnavailable() override;
    void onWhiteboardStart() override;
    void onWhiteboardStop() override;

    void onFirstAudioDataReceived(uint64_t userId) override;
    void onFirstVideoDataReceived(uint64_t userId) override;
    void onFirstScreenDataReceived(uint64_t userId) override;
    void onAudioDeviceStateChanged(const char deviceID[panortc::kMaxDeviceIDLength],
        panortc::AudioDeviceType deviceType,
        panortc::AudioDeviceState deviceState) override;
    void onVideoDeviceStateChanged(const char deviceID[panortc::kMaxDeviceIDLength],
        panortc::VideoDeviceType deviceType,
        panortc::VideoDeviceState deviceState) override;
    void onVideoSnapshotCompleted(bool succeed, uint64_t userId, const char *filename) override;

public: // RtcWhiteboard::Callback
    void onPageNumberChanged(panortc::WBPageNumber curPage, size_t totalPages) override;
    void onImageStateChanged(const char *url, panortc::WBImageState state) override;
    void onViewScaleChanged(float scale) override;
    void onRoleTypeChanged(panortc::WBRoleType newRole) override;
    void onContentUpdated() override;
    void onSnapshotComplete(panortc::QResult result, const char *filename) override;
    void onMessage(uint64_t userId, const char *msg, size_t size) override;
    void onAddBackgroundImages(panortc::QResult result, const char *fileId) override;
    void onDocTranscodeStatus(panortc::QResult result,
                                const char *fileId,
                                uint32_t progress,
                                uint32_t totalPage) override;
    void onCreateDoc(panortc::QResult result, const char *fileId) override;
    void onDeleteDoc(panortc::QResult result, const char *fileId) override;
    void onSwitchDoc(panortc::QResult result, const char *fileId) override;

public: // MediaStatsObserver
    void onVideoSendStats(panortc::VideoSendStats &stats) override;
    void onVideoRecvStats(panortc::VideoRecvStats &stats) override;
    void onAudioSendStats(panortc::AudioSendStats &stats) override;
    void onAudioRecvStats(panortc::AudioRecvStats &stats) override;
    void onScreenSendStats(panortc::ScreenSendStats &stats) override;
    void onScreenRecvStats(panortc::ScreenRecvStats &stats) override;
    void onVideoSendBweStats(panortc::VideoSendBweStats &stats) override;
    void onVideoRecvBweStats(panortc::VideoRecvBweStats &stats) override;
    void onSystemStats(panortc::SystemStats &stats) override;

protected:
    bool joinChannel();
    void leaveChannel();

protected:
    pano::utils::DUniquePtr<panortc::RtcEngine> rtcEngine_;

    UserManager userMgr_;

    RtcViewController *viewController_ = nullptr;

    std::string appId_;
    std::string channelId_;
    std::string panoToken_;
    uint64_t localUserId_;
    std::string userName_;
    panortc::ChannelMode chMode_ = panortc::ChannelMode::Mode_1v1;
    bool enableWhiteBoard_ = false;
};

