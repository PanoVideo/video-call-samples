#include "stdafx.h"
#include "MainViewController.h"
#include "MainDlg.h"
#include "UIThreadExecutorWin.h"
#include "RtcTester.h"
#include "PanoRtcEngine.h"

const int kDefaultVideoViewWidth = 960;
const int kDefaultVideoViewHeight = 540;

const std::string kAppId = %%enter APPID of your APP%%;
const std::string kPanoToken = %%enter APP Token which is got from PANO%%;

using namespace pano::utils;
using namespace panortc;


MainViewController::MainViewController()
{
    
}

bool MainViewController::init(CMainDlg *mainView)
{
    mainView_ = mainView;
    auto *e = new UIThreadExecutorWin();
    taskExecutor_.reset(e);
    e->init(*mainView_);

    previewVideoView_.setOnDestroyObserver([this] {
        mainView_->notifyPreviewVideoStopped();
        stopPreviewVideo();
    });

    localVideoView_.setOnDestroyObserver([this] {
        stopLocalVideo();
		mainView_->notifyVideoStopped();
    });

    RtcTester::instance().init(this, kAppId);

    return true;
}

void MainViewController::onMainViewClosing()
{
    leaveChannel();
    whiteboardController_.close();
}

bool MainViewController::joinChannel()
{
    stopPreviewVideo();
    mainView_->notifyPreviewVideoStopped();

    auto appId = kAppId;
    auto token = kPanoToken;
    auto channelId = mainView_->GetChannelId();
    auto userId = mainView_->GetUserId();
    auto userName = mainView_->GetUserName1();
    bool mode_1v1 = mainView_->IsMode_1v1();

    return RtcTester::instance().start(appId, 
        token, 
        channelId, 
        userId, 
        userName,
        mode_1v1);
}

void MainViewController::leaveChannel()
{
    RtcTester::instance().stop();
    clearCallState();
}

void MainViewController::startPreviewVideo()
{
    auto deviceId = mainView_->GetCurrentVideoDevice();
    if (deviceId.empty()) {
        return;
    }

    if (!previewVideoView_.IsWindow()) {
        RECT rc{ 0, 0, kDefaultVideoViewWidth, kDefaultVideoViewHeight };
        previewVideoView_.Create(NULL, rc, _T("Preview Video"), WS_OVERLAPPEDWINDOW);
    }
    previewVideoView_.ShowWindow(SW_SHOW);
    HWND hWnd = previewVideoView_;

    previewDeviceId_ = deviceId;
    auto profile = mainView_->GetCurrentVideoProfile();
    RenderConfig renderConfig;
    renderConfig.profileType = profile;
    renderConfig.scalingMode = mainView_->GetCurrentVideoScalingMode();
    renderConfig.enableMirror = mainView_->IsMirror();
    RtcTester::instance().startPreview(previewDeviceId_, hWnd, renderConfig);
}

void MainViewController::stopPreviewVideo()
{
    RtcTester::instance().stopPreview(previewDeviceId_);
    previewDeviceId_.clear();
    if (previewVideoView_.IsWindow()) {
        previewVideoView_.ShowWindow(SW_HIDE);
    }
}

void MainViewController::startLocalVideo()
{
    auto yuvFile = mainView_->GetYuvFile();
    auto deviceId = mainView_->GetCurrentVideoDevice();
    if (yuvFile.empty() && deviceId.empty()) {
        return;
    }

    if (!localVideoView_.IsWindow()) {
        RECT rc{ 0, 0, kDefaultVideoViewWidth, kDefaultVideoViewHeight };
        localVideoView_.Create(NULL, rc, _T("Local Video"), WS_OVERLAPPEDWINDOW);
    }
    localVideoView_.ShowWindow(SW_SHOW);
    HWND hWnd = localVideoView_;

    localVideoDeviceId_ = deviceId;
    auto profile = mainView_->GetCurrentVideoProfile();
    RenderConfig renderConfig;
    renderConfig.profileType = profile;
    renderConfig.scalingMode = mainView_->GetCurrentVideoScalingMode();
    renderConfig.enableMirror = mainView_->IsMirror();
    if (!yuvFile.empty()) {
        renderConfig.enableMirror = false;
        RtcTester::instance().startExternalVideo(yuvFile, hWnd, renderConfig);
    }
    else {
        RtcTester::instance().startVideo(localVideoDeviceId_, hWnd, renderConfig);
    }
    videoStarted_ = true;
}

void MainViewController::stopLocalVideo()
{
    RtcTester::instance().stopVideo();
    localVideoDeviceId_.clear();
    if (localVideoView_.IsWindow()) {
        localVideoView_.ShowWindow(SW_HIDE);
    }
    videoStarted_ = false;
}

void MainViewController::startLocalAudio()
{
    auto recordDeviceId = mainView_->GetCurrentRecordDevice();
    auto playoutDeviceId = mainView_->GetCurrentPlayoutDevice();
    if (recordDeviceId.empty() && playoutDeviceId.empty()) {
        return;
    }
    RtcTester::instance().startAudio(recordDeviceId, playoutDeviceId);
    audioStarted_ = true;
}

void MainViewController::stopLocalAudio()
{
    RtcTester::instance().stopAudio();
    audioStarted_ = false;
}

void MainViewController::startScreen()
{
    if (!isChannelJoined()) {
        return ;
    }
    //stopLocalVideo();
    bool forMotion = mainView_->IsOptimizedForMotion();
    RtcTester::instance().updateScreenOptimization(forMotion);
    auto source = mainView_->GetCurrentScreenSource();
    RtcTester::instance().startScreen(source.ssid, source.type);
    screenStarted_ = true;
}

void MainViewController::stopScreen()
{
    RtcTester::instance().stopScreen();
    screenStarted_ = false;
    if (isChannelJoined() && mainView_->IsVideoEnabled()) {
        //startLocalVideo();
    }
}

void MainViewController::updateScreenSource()
{
    if (!isChannelJoined() || !screenStarted_) {
        return;
    }

    auto source = mainView_->GetCurrentScreenSource();
    RtcTester::instance().updateScreenSource(source.ssid, source.type);
}

void MainViewController::updateScreenOptimization()
{
    if (!isChannelJoined() || !screenStarted_) {
        return;
    }
    bool forMotion = mainView_->IsOptimizedForMotion();
    RtcTester::instance().updateScreenOptimization(forMotion);
}

void MainViewController::updateAudioRecordDevice()
{
    if (!isChannelJoined() || !audioStarted_) {
        return;
    }
    auto recordDeviceId = mainView_->GetCurrentRecordDevice();
    RtcTester::instance().setAudioRecordDevice(recordDeviceId);
}

void MainViewController::updateAudioPlayoutDevice()
{
    if (!isChannelJoined() || !audioStarted_) {
        return;
    }
    auto playoutDeviceId = mainView_->GetCurrentPlayoutDevice();
    RtcTester::instance().setAudioPlayoutDevice(playoutDeviceId);
}

void MainViewController::updateVideoCaptureDevice()
{
    if (!isChannelJoined() || !audioStarted_) {
        return;
    }
    auto cameraDeviceId = mainView_->GetCurrentVideoDevice();
    localVideoDeviceId_ = cameraDeviceId;
    RtcTester::instance().setVideoCaptureDevice(cameraDeviceId);
}

void MainViewController::startWhiteboard()
{
    whiteboardController_.startWhiteboard();
}

void MainViewController::stopWhiteboard()
{
    whiteboardController_.stopWhiteboard();
}

void MainViewController::startAudioMixing(const char* filename) {
    if (audioMixingStarted_) {
        RtcTester::instance().destroyAduioMixingTask(0);
        audioMixingStarted_ = false;
    }
    bool ret = RtcTester::instance().createAudioMixingTask(0, filename);
    if (ret) {
        ret = RtcTester::instance().startAudioMixingTask(0);
        if (ret)
            audioMixingStarted_ = true;
    }
}

void MainViewController::stopAudioMixing() {
    RtcTester::instance().destroyAduioMixingTask(0);
    audioMixingStarted_ = false;
}

void MainViewController::snapshotVideo(const char* outputDir) {
    RtcTester::instance().snapshotVideo(outputDir, mainView_->GetUserId(), mainView_->IsMirror());
}

bool MainViewController::muteVideo()
{
    if (!videoStarted_) {
        return false;
    }
    RtcTester::instance().muteVideo();
    videoMuted_ = true;
    return true;
}

bool MainViewController::unmuteVideo()
{
    if (!videoStarted_) {
        return false;
    }
    RtcTester::instance().unmuteVideo();
    videoMuted_ = false;
    return true;
}

bool MainViewController::muteAudio()
{
    if (!audioStarted_) {
        return false;
    }
    RtcTester::instance().muteAudio();
    audioMuted_ = true;
    return true;
}

bool MainViewController::unmuteAudio()
{
    if (!audioStarted_) {
        return false;
    }
    RtcTester::instance().unmuteAudio();
    audioMuted_ = false;
    return true;
}

bool MainViewController::muteScreen()
{
    if (!screenStarted_) {
        return false;
    }
    RtcTester::instance().muteScreen();
    screenMuted_ = true;
    return true;
}

bool MainViewController::unmuteScreen()
{
    if (!screenStarted_) {
        return false;
    }
    RtcTester::instance().unmuteScreen();
    screenMuted_ = false;
    return true;
}

bool MainViewController::startAudioDump()
{
    if (!audioStarted_) {
        return false;
    }
    char tmp[MAX_PATH];
    if (GetTempPathA(MAX_PATH, tmp)) {
        std::string filePath = tmp;
        filePath += "\\pano_audio.dump";
        RtcTester::instance().startAudioDump(filePath);
        audioDumpStarted_ = true;
        return true;
    }
    return false;
}

bool MainViewController::stopAudioDump()
{
    if (!audioStarted_) {
        return false;
    }
    RtcTester::instance().stopAudioDump();
    audioDumpStarted_ = false;
    return true;
}

//Audio effect
bool MainViewController::setAudioEqualization(const int option)
{
    if (!audioStarted_) {
        return false;
    }
    RtcTester::instance().setAudioFeatureOption(OptionType::AudioEqualizationMode,option);
    return true;   
}

bool MainViewController::setAudioReverb(const int option)
{
	if (!audioStarted_) {
		return false;
	}
	RtcTester::instance().setAudioFeatureOption(OptionType::AudioReverbMode, option);
	return true;
}

bool MainViewController::startAudioDeviceTest()
{
    auto recordDeviceId = mainView_->GetCurrentRecordDevice();
    auto playoutDeviceId = mainView_->GetCurrentPlayoutDevice();
    if (recordDeviceId.empty() && playoutDeviceId.empty()) {
        return false;
    }
    RtcTester::instance().startAudioDeviceTest(recordDeviceId, playoutDeviceId);
    audioTestStarted_ = true;
    return true;
}

bool MainViewController::stopAudioDeviceTest()
{
    RtcTester::instance().stopAudioDeviceTest();
    audioTestStarted_ = false;
    return true;
}

void MainViewController::onAuthFailure(panortc::QResult result)
{
    if (result == QResult::OK) {
        result = QResult::FAILED;
    }
    onChannelJoinConfirm(result);
}

void MainViewController::onChannelJoinConfirm(panortc::QResult result)
{
    taskExecutor_->async([=] {
        if (result == QResult::OK) {
            channelJoined_ = true;
            if (mainView_->IsVideoEnabled()) {
                startLocalVideo();
            }
            if (mainView_->IsAudioEnabled()) {
                startLocalAudio();
            }
            
            mainView_->notifyChannelJoined();
        }
        else {
            mainView_->notifyChannelLeft();
            RtcTester::instance().stop();
            clearCallState();
            std::wstring err_msg = _T("Join channel failed, code=");
            err_msg += std::to_wstring(int(result));
            mainView_->MessageBox(err_msg.c_str(), _T("Error"), 0);
        }
    });
}

void MainViewController::onChannelLeaveIndication(panortc::QResult result)
{
    taskExecutor_->async([=] {
        mainView_->notifyChannelLeft();
        RtcTester::instance().stop();
        clearCallState();
    });
}

void MainViewController::onUserJoinIndication(uint64_t userId)
{
    taskExecutor_->async([=] {
        auto user = RtcTester::instance().getUserManager().getUser(userId);
        if (user) {
            std::unique_ptr<RemoteVideoWindow> view(new RemoteVideoWindow());
            RECT rc{ 0, 0, kDefaultVideoViewWidth, kDefaultVideoViewHeight };
            view->Create(NULL, rc, _T("Remote Video"), WS_OVERLAPPEDWINDOW);
            view->setUserInfo(userId, "Remote User");
            view->setOnDestroyObserver([this, userId] {
                RtcTester::instance().unsubscribeVideo(userId);
                // don't release the remote video view
                //remoteUserVideoViewMap_.erase(userId);
            });
            remoteUserVideoViewMap_.emplace(userId, std::move(view));
        }
    });
}

void MainViewController::onUserLeaveIndication(uint64_t userId, UserLeaveReason reason)
{
    taskExecutor_->async([=] {
        auto it = remoteUserVideoViewMap_.find(userId);
        if (it != remoteUserVideoViewMap_.end()) {
            remoteUserVideoViewMap_.erase(it);
        }
        auto it2 = remoteUserScreenViewMap_.find(userId);
        if (it2 != remoteUserScreenViewMap_.end()) {
            remoteUserScreenViewMap_.erase(it2);
        }
    });
}

void MainViewController::onUserVideoStart(uint64_t userId, panortc::VideoProfileType maxProfile)
{
    taskExecutor_->async([=] {
        auto user = RtcTester::instance().getUserManager().getUser(userId);
        if (user) {
            auto it = remoteUserVideoViewMap_.find(userId);
            if (it != remoteUserVideoViewMap_.end()) {
                if (it->second->IsWindow()) {
                    RenderConfig config;
                    config.profileType = user->getMaxVideoProfile();
                    config.scalingMode = mainView_->GetCurrentVideoScalingMode();
                    config.enableMirror = false;
                    auto ret = RtcTester::instance().subscribeVideo(userId, it->second->m_hWnd, config);
                    if (ret == QResult::OK) {
                        it->second->ShowWindow(SW_SHOW);
                    }
                }
            }
        }
    });
}

void MainViewController::onUserVideoStop(uint64_t userId)
{
    taskExecutor_->async([=] {
        auto user = RtcTester::instance().getUserManager().getUser(userId);
        if (user) {
            auto it = remoteUserVideoViewMap_.find(userId);
            if (it != remoteUserVideoViewMap_.end()) {
                if (it->second->IsWindow()) {
                    it->second->ShowWindow(SW_HIDE);
                }
            }
        }
    });
}

void MainViewController::onUserScreenStart(uint64_t userId)
{
    taskExecutor_->async([=] {
        auto user = RtcTester::instance().getUserManager().getUser(userId);
        if (user) {
            RemoteVideoWindow *screenView = nullptr;
            auto it = remoteUserScreenViewMap_.find(userId);
            if (it == remoteUserScreenViewMap_.end()) {
                std::unique_ptr<RemoteVideoWindow> view(new RemoteVideoWindow());
                RECT rc{ 0, 0, kDefaultVideoViewWidth, kDefaultVideoViewHeight };
                view->Create(NULL, rc, _T("Remote Screen"), WS_OVERLAPPEDWINDOW);
                view->setUserInfo(userId, "Remote User");
                view->setOnDestroyObserver([this, userId] {
                    RtcTester::instance().unsubscribeScreen(userId);
                    // don't release the remote video view
                    //remoteUserVideoViewMap_.erase(userId);
                });
                screenView = view.get();
                remoteUserScreenViewMap_.emplace(userId, std::move(view));
            } else {
                screenView = it->second.get();
            }
            if (screenView && screenView->IsWindow()) {
                auto ret = RtcTester::instance().subscribeScreen(userId, screenView->m_hWnd);
                if (ret == QResult::OK) {
                    screenView->ShowWindow(SW_SHOW);
                }
            }
        }
    });
}

void MainViewController::onUserScreenStop(uint64_t userId)
{
    taskExecutor_->async([=] {
        auto user = RtcTester::instance().getUserManager().getUser(userId);
        if (user) {
            auto it = remoteUserScreenViewMap_.find(userId);
            if (it != remoteUserScreenViewMap_.end()) {
                if (it->second->IsWindow()) {
                    it->second->ShowWindow(SW_HIDE);
                }
            }
        }
    });
}

void MainViewController::onWhiteboardAvailable()
{
    taskExecutor_->async([=] {
        if (mainView_->IsWhiteboardEnabled()) {
            startWhiteboard();
        }
    });
}

void MainViewController::onWhiteboardUnavailable()
{
    taskExecutor_->async([=] {
        stopWhiteboard();
    });
}

void MainViewController::onWhiteboardStart()
{
}

void MainViewController::onWhiteboardStop()
{
}

void MainViewController::onAudioDeviceStateChanged(const char deviceID[kMaxDeviceIDLength],
    AudioDeviceType deviceType,
    AudioDeviceState deviceState)
{
    taskExecutor_->async([this] {
        mainView_->notifyAudioDeviceChanged();
        if (!audioStarted_ && mainView_->IsAudioEnabled()) {
            startLocalAudio();
        }
    });
}

void MainViewController::onVideoDeviceStateChanged(const char deviceID[kMaxDeviceIDLength],
    VideoDeviceType deviceType,
    VideoDeviceState deviceState)
{
    taskExecutor_->async([this] {
        mainView_->notifyVideoDeviceChanged();
        if (!videoStarted_ && mainView_->IsVideoEnabled()) {
            startLocalVideo();
        }
    });
}

void MainViewController::onVideoSendStats(panortc::VideoSendStats &stats)
{
    taskExecutor_->async([=] {
        if (localVideoStats_.width != stats.width 
            || localVideoStats_.height != stats.height
            || localVideoStats_.fps != stats.framerate
            || localVideoStats_.bitrate != stats.sendBitrate
            || localVideoStats_.loss != stats.lossRatio) {
            localVideoStats_.width = stats.width;
            localVideoStats_.height = stats.height;
            localVideoStats_.fps = stats.framerate;
            localVideoStats_.bitrate = stats.sendBitrate;
            localVideoStats_.loss = stats.lossRatio;
            if (localVideoView_.IsWindow()) {
                auto userInfo = "Local User ["
                    + std::to_string(stats.width) + "x"
                    + std::to_string(stats.height) + "]"
                    + " br: " + std::to_string(stats.sendBitrate / 1000)
                    + " fps: " + std::to_string(stats.framerate);
                localVideoView_.SetWindowTitle(userInfo);
            }
        }
    });
}

void MainViewController::onVideoRecvStats(panortc::VideoRecvStats &stats)
{
    taskExecutor_->async([=] {
        auto user = RtcTester::instance().getUserManager().getUser(stats.userId);
        if (user && (user->videoStats_.width != stats.width 
            || user->videoStats_.height != stats.height
            || user->videoStats_.fps != stats.framerate
            || user->videoStats_.bitrate != stats.recvBitrate
            || user->videoStats_.loss != stats.lossRatio)) {
            user->videoStats_.width = stats.width;
            user->videoStats_.height = stats.height;
            auto it = remoteUserVideoViewMap_.find(stats.userId);
            if (it != remoteUserVideoViewMap_.end()) {
                if (it->second->IsWindow()) {
                    auto userInfo = "Remote User (" 
                        + std::to_string(stats.userId) + ") ["
                        + std::to_string(stats.width) + "x" 
                        + std::to_string(stats.height) + "]"
                        + " br: " + std::to_string(stats.recvBitrate / 1000)
                        + " fps: " + std::to_string(stats.framerate);
                    it->second->SetWindowTitle(userInfo);
                }
            }
        }
    });
}

void MainViewController::onScreenSendStats(panortc::ScreenSendStats &stats)
{
    
}

void MainViewController::onScreenRecvStats(panortc::ScreenRecvStats &stats)
{
    taskExecutor_->async([=] {
        auto user = RtcTester::instance().getUserManager().getUser(stats.userId);
        if (user && (user->screenStats_.width != stats.width
            || user->screenStats_.height != stats.height
            || user->screenStats_.fps != stats.framerate
            || user->screenStats_.bitrate != stats.recvBitrate
            || user->screenStats_.loss != stats.lossRatio)) {
            user->screenStats_.width = stats.width;
            user->screenStats_.height = stats.height;
            auto it = remoteUserScreenViewMap_.find(stats.userId);
            if (it != remoteUserScreenViewMap_.end()) {
                if (it->second->IsWindow()) {
                    auto userInfo = "Remote User ("
                        + std::to_string(stats.userId) + ") ["
                        + std::to_string(stats.width) + "x"
                        + std::to_string(stats.height) + "]"
                        + " br: " + std::to_string(stats.recvBitrate / 1000)
                        + " fps: " + std::to_string(stats.framerate);
                    it->second->SetWindowTitle(userInfo);
                }
            }
        }
    });
}

void MainViewController::onAudioSendStats(panortc::AudioSendStats &stats)
{

}

void MainViewController::onAudioRecvStats(panortc::AudioRecvStats &stats)
{

}

void MainViewController::onVideoSendBweStats(panortc::VideoSendBweStats &stats)
{

}

void MainViewController::onVideoRecvBweStats(panortc::VideoRecvBweStats &stats)
{

}

void MainViewController::onWhiteboardCreateDoc(panortc::QResult result, const char* fileId)
{
    taskExecutor_->async([=, str=std::string(fileId)] {
        whiteboardController_.onCreateDoc(str.c_str());
    });
}

void MainViewController::onWhiteboardDeleteDoc(panortc::QResult result, const char* fileId)
{
    taskExecutor_->async([=, str=std::string(fileId)] {
        whiteboardController_.onDeleteDoc(str.c_str());
    });
}

void MainViewController::onWhiteboardSwitchDoc(panortc::QResult result, const char* fileId)
{
    taskExecutor_->async([=, str = std::string(fileId)]{
        whiteboardController_.onSwitchDoc(str.c_str());
    });
}

void MainViewController::clearCallState()
{
    if (localVideoView_.IsWindow()) {
        localVideoView_.ShowWindow(SW_HIDE);
    }
    stopWhiteboard();
    remoteUserVideoViewMap_.clear();
    remoteUserScreenViewMap_.clear();
    channelJoined_ = false;
    videoStarted_ = false;
    audioStarted_ = false;
    screenStarted_ = false;
    videoMuted_ = false;
    audioMuted_ = false;
    screenMuted_ = false;
    audioDumpStarted_ = false;
}

void MainViewController::uninit()
{
    RtcTester::instance().uninit();
    mainView_ = nullptr;
    taskExecutor_.reset();
}
