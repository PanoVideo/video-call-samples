
#include "RtcTester.h"
#include "utils.h"

const std::string kPanoServer = "api.pano.video";

using namespace panortc;


RtcTester::RtcTester()
{
    rtcEngine_.reset(createPanoRtcEngine());
    if (rtcEngine_) {
        auto wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->setCallback(this);
    }
}

RtcTester::~RtcTester()
{
    rtcEngine_.reset();
}

bool RtcTester::init(RtcViewController *vc, 
    const std::string &appId)
{
    viewController_ = vc;
    RtcEngine::Configuration config{ this, appId.c_str(), kPanoServer.c_str(), AudioAecType::Default, false };
    auto ret = rtcEngine_->initialize(config);
    return ret == QResult::OK;
}

bool RtcTester::start(std::string appId,
    std::string token,
    std::string channelId,
    uint64_t userId,
    std::string userName,
    bool mode_1v1)
{
    appId_ = std::move(appId);
    panoToken_ = std::move(token);
    channelId_ = std::move(channelId);
    localUserId_ = userId;
    userName_ = std::move(userName);
    chMode_ = mode_1v1 ? ChannelMode::Mode_1v1 : ChannelMode::Mode_Meeting;

    return joinChannel();
}

void RtcTester::stop()
{
    leaveChannel();
    userMgr_.clearAllUsers();
}

void RtcTester::uninit()
{
    rtcEngine_.reset();
    viewController_ = nullptr;
}

bool RtcTester::startPreview(const std::string &strDeviceId, void *view, RenderConfig &config)
{
    if (rtcEngine_) {
        auto *videoDeviceMgr = rtcEngine_->getVideoDeviceManager();
        char deviceId[kMaxDeviceIDLength];
        strncpy_s(deviceId, strDeviceId.c_str(), strDeviceId.size());
        videoDeviceMgr->startPreview(deviceId, view, config);
        return true;
    }
    return false;
}

void RtcTester::stopPreview(const std::string &strDeviceId)
{
    if (rtcEngine_) {
        auto *videoDeviceMgr = rtcEngine_->getVideoDeviceManager();
        char deviceId[kMaxDeviceIDLength];
        strncpy_s(deviceId, strDeviceId.c_str(), strDeviceId.size());
        videoDeviceMgr->stopPreview(deviceId);
    }
}

void RtcTester::startVideo(const std::string &strDeviceId, void *view, RenderConfig &config)
{
    if (rtcEngine_) {
        if (exCapturer_) {
            exCapturer_->stop();
            exCapturer_.reset();
        }
        auto *videoDeviceMgr = rtcEngine_->getVideoDeviceManager();
        char deviceId[kMaxDeviceIDLength];
        strncpy_s(deviceId, strDeviceId.c_str(), strDeviceId.size());
        videoDeviceMgr->setDevice(deviceId);
        rtcEngine_->startVideo(view, config);
    }
}

void RtcTester::startExternalVideo(const std::wstring &yuvFile, void *view, panortc::RenderConfig &config)
{
    if (rtcEngine_) {
        auto *videoDeviceMgr = rtcEngine_->getVideoDeviceManager();
        auto *ptr = videoDeviceMgr->createExternalCapturer();
        char deviceId[kMaxDeviceIDLength];
        ptr->getDeviceID(deviceId);
        videoDeviceMgr->setDevice(deviceId);
        rtcEngine_->startVideo(view, config);
        exCapturer_ = std::make_unique<ExternalVideoCapturer>(ptr);
        exCapturer_->start(yuvFile, 1280, 720, 30);
    }
}

void RtcTester::stopVideo()
{
    if (rtcEngine_) {
        rtcEngine_->stopVideo();
    }
    if (exCapturer_) {
        exCapturer_->stop();
        exCapturer_.reset();
    }
}

void RtcTester::startAudio(const std::string &recordDeviceId, const std::string &playoutDeviceId)
{
    if (rtcEngine_) {
        setAudioRecordDevice(recordDeviceId);
        setAudioPlayoutDevice(playoutDeviceId);
        rtcEngine_->startAudio();
    }
}

void RtcTester::stopAudio()
{
    if (rtcEngine_) {
        rtcEngine_->stopAudio();
    }
}

void RtcTester::setAudioRecordDevice(const std::string &recordDeviceId)
{
    if (rtcEngine_) {
        auto *audioDeviceMgr = rtcEngine_->getAudioDeviceManager();
        char deviceId[kMaxDeviceIDLength];
        strncpy_s(deviceId, recordDeviceId.c_str(), recordDeviceId.size());
        audioDeviceMgr->setRecordDevice(deviceId);
    }
}

void RtcTester::setAudioPlayoutDevice(const std::string &playoutDeviceId)
{
    if (rtcEngine_) {
        auto *audioDeviceMgr = rtcEngine_->getAudioDeviceManager();
        char deviceId[kMaxDeviceIDLength];
        strncpy_s(deviceId, playoutDeviceId.c_str(), playoutDeviceId.size());
        audioDeviceMgr->setPlayoutDevice(deviceId);
    }
}

void RtcTester::setVideoCaptureDevice(const std::string &cameraDeviceId)
{
    if (rtcEngine_) {
        auto *videoDeviceMgr = rtcEngine_->getVideoDeviceManager();
        char deviceId[kMaxDeviceIDLength];
        strncpy_s(deviceId, cameraDeviceId.c_str(), cameraDeviceId.size());
        videoDeviceMgr->setDevice(deviceId);
    }
}

QResult RtcTester::subscribeVideo(uint64_t userId, void *view, RenderConfig &config)
{
    if (rtcEngine_) {
        return rtcEngine_->subscribeVideo(userId, view, config);
    }

    return QResult::NOT_INITIALIZED;
}

void RtcTester::unsubscribeVideo(uint64_t userId)
{
    auto user = userMgr_.getUser(userId);
    if (user && rtcEngine_) {
        rtcEngine_->unsubscribeVideo(userId);
    }
}

void RtcTester::startScreen(panortc::ScreenSourceID sourceId, panortc::ScreenSourceType type)
{
    if (rtcEngine_) {
        auto sourceMgr = rtcEngine_->getScreenSourceManager();
        sourceMgr->setScreenSource(type, sourceId);
        rtcEngine_->startScreen();
    }
}

void RtcTester::updateScreenSource(panortc::ScreenSourceID sourceId, panortc::ScreenSourceType type)
{
    if (rtcEngine_) {
        auto sourceMgr = rtcEngine_->getScreenSourceManager();
        sourceMgr->setScreenSource(type, sourceId);
    }
}

void RtcTester::stopScreen()
{
    if (rtcEngine_) {
        rtcEngine_->stopScreen();
    }
}

QResult RtcTester::subscribeScreen(uint64_t userId, void *view)
{
    if (rtcEngine_) {
        return rtcEngine_->subscribeScreen(userId, view);
    }

    return QResult::NOT_INITIALIZED;
}

void RtcTester::unsubscribeScreen(uint64_t userId)
{
    auto user = userMgr_.getUser(userId);
    if (user && rtcEngine_) {
        rtcEngine_->unsubscribeScreen(userId);
    }
}

void RtcTester::updateScreenOptimization(bool forMotion)
{
    if (rtcEngine_) {
        auto sourceMgr = rtcEngine_->getScreenSourceManager();
        sourceMgr->setScreenOptimization(forMotion);
    }
}

void RtcTester::startWhiteboard(void *view)
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->open(view);

        wbEngine->setFillType(WBFillType::NONE);
        wbEngine->setFillColor({0.5f, 0.5f, 0.5f, 0.5f});

        wbEngine->setForegroundColor({192.f/255, 0, 123.f/255, 1});
        wbEngine->setBackgroundColor({ 1, 1, 1, 1 });
    }
}

void RtcTester::setWbToolType(WBToolType type)
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->setToolType(type);
    }
}

void RtcTester::wbToolBoxUndo()
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->undo();
    }
}

void RtcTester::wbToolBoxRedo()
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->redo();
    }
}

void RtcTester::wbToolBoxBgImage(const char* imageUrl)
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->setBackgroundImage(imageUrl);
    }
}

void RtcTester::wbToolBoxCreateDoc(const char* docUrl)
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->createDoc(docUrl);
    }
}

void RtcTester::wbToolBoxSwitchDoc(const char* docUrl)
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->switchDoc(docUrl);
    }
}

void RtcTester::wbToolBoxForeColor(int r, int g, int b)
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->setForegroundColor({ r / 255.f, g / 255.f, b / 255.f, 1 });
    }
}

void RtcTester::wbToolBoxFontStyle(WBFontStyle style)
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->setFontStyle(style);
    }
}

void RtcTester::wbToolBoxFontSize(int size)
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->setFontSize(size);
    }
}

void RtcTester::wbToolBoxLineWidth(int size)
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->setLineWidth(size);
    }
}

void RtcTester::wbToolBoxSnapshot(const char *outputDir)
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->snapshot(WBSnapshotMode::VIEW, outputDir);
    }
}

const char* RtcTester::getCurrentDoc()
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        return wbEngine->getCurrentFileId();
    }
    return nullptr;
}

bool RtcTester::createAudioMixingTask(int64_t taskId, const char *filename) {
    if (rtcEngine_) {
        auto *mix = rtcEngine_->getAudioMixingManager();
        if (mix) {
            auto ret = mix->createAudioMixingTask(taskId, filename);
            return ret == QResult::OK;
        }
    }
    return false;
}

void RtcTester::destroyAduioMixingTask(int64_t taskId) {
    if (rtcEngine_) {
        auto *mix = rtcEngine_->getAudioMixingManager();
        if (mix) {
            mix->destroyAudioMixingTask(taskId);
        }
    }
}

bool RtcTester::startAudioMixingTask(int64_t taskId) {
    if (rtcEngine_) {
        auto *mix = rtcEngine_->getAudioMixingManager();
        if (mix) {
            AudioMixingConfig config;
            config.cycle = 0;
            auto ret = mix->start(taskId, config);
            return ret == QResult::OK;
        }
    }
    return false;
}

bool RtcTester::updateAudioMixingTask(int64_t taskId) {
    return true;
}

bool RtcTester::stopAudioMixingTask(int64_t taskId) {
    if (rtcEngine_) {
        auto *mix = rtcEngine_->getAudioMixingManager();
        if (mix) {
            mix->stop(taskId);
            return true;
        }
    }
    return false;
}

bool RtcTester::pauseAudioMixingTask(int64_t taskId) {
    if (rtcEngine_) {
        auto *mix = rtcEngine_->getAudioMixingManager();
        if (mix) {
            auto ret = mix->pause(taskId);
            return ret == QResult::OK;
        }
    }
    return false;
}

bool RtcTester::resumeAudioMixingTask(int64_t taskId) {
    if (rtcEngine_) {
        auto *mix = rtcEngine_->getAudioMixingManager();
        if (mix) {
            auto ret = mix->resume(taskId);
            return ret == QResult::OK;
        }
    }
    return false;
}

bool RtcTester::getAudioMixingDuration(int64_t taskId, int64_t *duration) {
    if (rtcEngine_) {
        auto *mix = rtcEngine_->getAudioMixingManager();
        if (mix) {
            auto ret = mix->getDuration(taskId, duration);
            return ret == QResult::OK;
        }
    }
    return false;
}

bool RtcTester::getAudioMixingCurrentTimestamp(int64_t taskId, int64_t *timestamp) {
    if (rtcEngine_) {
        auto *mix = rtcEngine_->getAudioMixingManager();
        if (mix) {
            auto ret = mix->getCurrentTimestamp(taskId, timestamp);
            return ret == QResult::OK;
        }
    }
    return false;
}

bool RtcTester::seekAudioMixingTask(int64_t taskId, int64_t timestamp) {
    if (rtcEngine_) {
        auto *mix = rtcEngine_->getAudioMixingManager();
        if (mix) {
            auto ret = mix->seek(taskId, timestamp);
            return ret == QResult::OK;
        }
    }
    return false;
}

void RtcTester::wbToolBoxPrevPage()
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->prevPage();
    }
}

void RtcTester::wbToolBoxAddPage()
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        auto pageNum = wbEngine->getCurrentPageNumber();
        wbEngine->insertPage(pageNum, true);
    }
}

void RtcTester::wbToolBoxDelPage()
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        auto pageNum = wbEngine->getCurrentPageNumber();
        wbEngine->removePage(pageNum);
    }
}

void RtcTester::wbToolBoxNextPage()
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->nextPage();
    }
}

void RtcTester::stopWhiteboard()
{
    if (rtcEngine_) {
        auto *wbEngine = rtcEngine_->getWhiteboardEngine();
        wbEngine->close();
    }
}

bool RtcTester::muteAudio()
{
    if (rtcEngine_) {
        return rtcEngine_->muteAudio() == QResult::OK;
    }
    return false;
}

bool RtcTester::unmuteAudio()
{
    if (rtcEngine_) {
        return rtcEngine_->unmuteAudio() == QResult::OK;
    }
    return false;
}

bool RtcTester::muteVideo()
{
    if (rtcEngine_) {
        return rtcEngine_->muteVideo() == QResult::OK;
    }
    return false;
}

bool RtcTester::unmuteVideo()
{
    if (rtcEngine_) {
        return rtcEngine_->unmuteVideo() == QResult::OK;
    }
    return false;
}

bool RtcTester::muteScreen()
{
    if (rtcEngine_) {
        return rtcEngine_->muteScreen() == QResult::OK;
    }
    return false;
}

bool RtcTester::unmuteScreen()
{
    if (rtcEngine_) {
        return rtcEngine_->unmuteScreen() == QResult::OK;
    }
    return false;
}

void RtcTester::snapshotVideo(const char* outputDir, uint64_t userId, bool mirror) {
    if (rtcEngine_) {
        SnapshotVideoOption option;
        option.mirror = mirror;
        rtcEngine_->snapshotVideo(outputDir, userId, option);
    }
}

void RtcTester::startAudioDump(const std::string &filePath)
{
    if (rtcEngine_) {
        rtcEngine_->startAudioDump(filePath.c_str());
    }
}

void RtcTester::stopAudioDump()
{
    if (rtcEngine_) {
        rtcEngine_->stopAudioDump();
    }
}

//Audio Effect
void RtcTester::setAudioFeatureOption(OptionType type, int option)
{
    if (rtcEngine_) {
        switch (type) {
            case OptionType::AudioEqualizationMode:
            {
                AudioEqualizationOption eqOption = (AudioEqualizationOption)option;
                rtcEngine_->setOption(OptionType::AudioEqualizationMode, &eqOption, sizeof(eqOption));
                break;
            }
            case OptionType::AudioReverbMode:
            {
                 AudioReverbOption rbOption = (AudioReverbOption)option;
                 rtcEngine_->setOption(OptionType::AudioReverbMode, &rbOption, sizeof(rbOption));
                 break;
            }
            default:
                 break;
        }
    }
}

void RtcTester::startAudioDeviceTest(const std::string &recordDeviceId, const std::string &playoutDeviceId)
{
    if (rtcEngine_) {
        auto *audioDeviceMgr = rtcEngine_->getAudioDeviceManager();
        char szRecordDeviceId[kMaxDeviceIDLength];
        strncpy_s(szRecordDeviceId, recordDeviceId.c_str(), recordDeviceId.size());
        char szPlayoutDeviceId[kMaxDeviceIDLength];
        strncpy_s(szPlayoutDeviceId, playoutDeviceId.c_str(), playoutDeviceId.size());
        audioDeviceMgr->startDeviceTest(szRecordDeviceId, szPlayoutDeviceId);
    }
}

void RtcTester::stopAudioDeviceTest()
{
    if (rtcEngine_) {
        auto *audioDeviceMgr = rtcEngine_->getAudioDeviceManager();
        audioDeviceMgr->stopDeviceTest();
    }
}

void RtcTester::enableUploadLogs(bool enabled)
{
    if (rtcEngine_) {
        rtcEngine_->setOption(OptionType::EnableUploadDebugLogs, &enabled, sizeof(enabled));
    }
}

void RtcTester::enableUploadAudioDump(bool enabled)
{
    if (rtcEngine_) {
        rtcEngine_->setOption(OptionType::EnableUploadAudioDump, &enabled, sizeof(enabled));
    }
}

void RtcTester::updateVideoFrameRate(bool lowFPS)
{
    if (rtcEngine_) {
        auto value = lowFPS ? VideoFrameRateType::Low : VideoFrameRateType::Standard;
        rtcEngine_->setOption(OptionType::VideoFrameRate, &value, sizeof(value));
    }
}

bool RtcTester::joinChannel()
{
    RtcEngine::Configuration config{ this, appId_.c_str(), kPanoServer.c_str(), AudioAecType::Default, false };
    auto ret = rtcEngine_->initialize(config);
    if (ret != QResult::OK) {
        if (viewController_) {
            viewController_->onAuthFailure(ret);
        }
        return false;
    }
    RtcEngine::ChannelConfiguration channelConfig;
    channelConfig.mode = chMode_;
    channelConfig.subscribeAudioAll = true;
    channelConfig.userName = userName_.c_str();
    ret = rtcEngine_->joinChannel(panoToken_.c_str(), channelId_.c_str(), localUserId_, channelConfig);
    if (ret != QResult::OK) {
        if (viewController_) {
            viewController_->onAuthFailure(ret);
        }
        return false;
    }

    return true;
}

void RtcTester::leaveChannel()
{
    if (rtcEngine_) {
        rtcEngine_->leaveChannel();
        rtcEngine_->setMediaStatsObserver(nullptr);
    }
    if (exCapturer_) {
        exCapturer_->stop();
        exCapturer_.reset();
    }
}

void RtcTester::onChannelJoinConfirm(QResult result)
{
    printf("onChannelJoinConfirm, result=%d\n", int(result));
    if (result == QResult::OK) {
        rtcEngine_->setMediaStatsObserver(this);
    }
    if (viewController_) {
        viewController_->onChannelJoinConfirm(result);
    }
}

void RtcTester::onChannelLeaveIndication(QResult result)
{
    userMgr_.clearAllUsers();
    if (viewController_) {
        viewController_->onChannelLeaveIndication(result);
    }
}

void RtcTester::onChannelCountDown(uint32_t remain)
{

}

void RtcTester::onUserJoinIndication(uint64_t userId, const char *userName)
{
    UserInfo::Ptr user(new UserInfo(userId));
    userMgr_.addUser(userId, std::move(user));
    if (viewController_) {
        viewController_->onUserJoinIndication(userId);
    }
}

void RtcTester::onUserLeaveIndication(uint64_t userId, UserLeaveReason reason)
{
    userMgr_.removeUser(userId);
    if (viewController_) {
        viewController_->onUserLeaveIndication(userId, reason);
    }
}

void RtcTester::onUserAudioStart(uint64_t userId)
{
    // subscribeAudioAll is true
    /*if (rtcEngine_) {
        rtcEngine_->subscribeAudio(userId);
    }*/
}

void RtcTester::onUserAudioStop(uint64_t userId)
{
    /*if (rtcEngine_) {
        rtcEngine_->unsubscribeAudio(userId);
    }*/
}

void RtcTester::onUserAudioSubscribe(uint64_t userId, MediaSubscribeResult result)
{

}

void RtcTester::onUserVideoStart(uint64_t userId, VideoProfileType maxProfile)
{
    auto user = userMgr_.getUser(userId);
    if (user) {
        user->setMaxVideoProfile(maxProfile);
        
        if (viewController_) {
            viewController_->onUserVideoStart(userId, maxProfile);
        }
    }
}

void RtcTester::onUserVideoStop(uint64_t userId)
{
    if (viewController_) {
        viewController_->onUserVideoStop(userId);
    }
}

void RtcTester::onUserVideoSubscribe(uint64_t userId, MediaSubscribeResult result)
{

}

void RtcTester::onUserAudioMute(uint64_t userId)
{

}

void RtcTester::onUserAudioUnmute(uint64_t userId)
{

}

void RtcTester::onUserVideoMute(uint64_t userId)
{

}

void RtcTester::onUserVideoUnmute(uint64_t userId)
{

}

void RtcTester::onUserScreenStart(uint64_t userId)
{
    if (viewController_) {
        viewController_->onUserScreenStart(userId);
    }
}

void RtcTester::onUserScreenStop(uint64_t userId)
{
    if (viewController_) {
        viewController_->onUserScreenStop(userId);
    }
}

void RtcTester::onUserScreenSubscribe(uint64_t userId, MediaSubscribeResult result)
{

}

void RtcTester::onUserScreenMute(uint64_t userId)
{

}

void RtcTester::onUserScreenUnmute(uint64_t userId)
{

}

void RtcTester::onWhiteboardAvailable()
{
    if (viewController_) {
        viewController_->onWhiteboardAvailable();
    }
}

void RtcTester::onWhiteboardUnavailable()
{
    if (viewController_) {
        viewController_->onWhiteboardUnavailable();
    }
}

void RtcTester::onWhiteboardStart()
{
    if (viewController_) {
        viewController_->onWhiteboardStart();
    }
}

void RtcTester::onWhiteboardStop()
{
    if (viewController_) {
        viewController_->onWhiteboardStop();
    }
}

void RtcTester::onFirstAudioDataReceived(uint64_t userId)
{

}

void RtcTester::onFirstVideoDataReceived(uint64_t userId)
{

}

void RtcTester::onFirstScreenDataReceived(uint64_t userId)
{

}

void RtcTester::onAudioDeviceStateChanged(const char deviceID[kMaxDeviceIDLength],
    AudioDeviceType deviceType,
    AudioDeviceState deviceState)
{
    if (viewController_) {
        viewController_->onAudioDeviceStateChanged(deviceID, deviceType, deviceState);
    }
}

void RtcTester::onVideoDeviceStateChanged(const char deviceID[kMaxDeviceIDLength],
    VideoDeviceType deviceType,
    VideoDeviceState deviceState)
{
    if (viewController_) {
        viewController_->onVideoDeviceStateChanged(deviceID, deviceType, deviceState);
    }
}

void RtcTester::onVideoSnapshotCompleted(bool succeed, uint64_t userId, const char *filename) {
}

//////////////////////////////////////////////////////////////////
// RtcWhiteboard::Callback
void RtcTester::onPageNumberChanged(panortc::WBPageNumber curPage, size_t totalPages)
{}

void RtcTester::onImageStateChanged(const char *url, panortc::WBImageState state)
{}

void RtcTester::onViewScaleChanged(float scale)
{}

void RtcTester::onRoleTypeChanged(panortc::WBRoleType newRole)
{}

void RtcTester::onContentUpdated()
{}

void RtcTester::onSnapshotComplete(panortc::QResult result, const char *filename)
{}

void RtcTester::onMessage(uint64_t userId, const char *msg, size_t size)
{}

void RtcTester::onAddBackgroundImages(panortc::QResult result, const char *fileId)
{}

void RtcTester::onDocTranscodeStatus(panortc::QResult result,
                                const char *filePath,
                                uint32_t progress,
                                uint32_t totalPage)
{}

void RtcTester::onCreateDoc(panortc::QResult result, const char *fileId)
{
    if (panortc::QResult::OK == result && fileId) {
        if (viewController_) {
            viewController_->onWhiteboardCreateDoc(result, fileId);
        }
    }
}

void RtcTester::onDeleteDoc(panortc::QResult result, const char *fileId)
{
    if (panortc::QResult::OK == result && fileId) {
        if (viewController_) {
            viewController_->onWhiteboardDeleteDoc(result, fileId);
        }
    }
}

void RtcTester::onSwitchDoc(panortc::QResult result, const char *fileId)
{
    if (panortc::QResult::OK == result && fileId) {
        if (viewController_) {
            viewController_->onWhiteboardSwitchDoc(result, fileId);
        }
    }
}

//////////////////////////////////////////////////////////////////
// MediaStatsObserver
void RtcTester::onVideoSendStats(VideoSendStats &stats)
{
    if (viewController_) {
        viewController_->onVideoSendStats(stats);
    }
}

void RtcTester::onVideoRecvStats(VideoRecvStats &stats)
{
    if (viewController_) {
        viewController_->onVideoRecvStats(stats);
    }
}

void RtcTester::onAudioSendStats(AudioSendStats &stats)
{
    if (viewController_) {
        viewController_->onAudioSendStats(stats);
    }
}

void RtcTester::onAudioRecvStats(AudioRecvStats &stats)
{
    if (viewController_) {
        viewController_->onAudioRecvStats(stats);
    }
}

void RtcTester::onScreenSendStats(ScreenSendStats &stats)
{
    if (viewController_) {
        viewController_->onScreenSendStats(stats);
    }
}

void RtcTester::onScreenRecvStats(ScreenRecvStats &stats)
{
    if (viewController_) {
        viewController_->onScreenRecvStats(stats);
    }
}

void RtcTester::onVideoSendBweStats(VideoSendBweStats &stats)
{
    if (viewController_) {
        viewController_->onVideoSendBweStats(stats);
    }
}

void RtcTester::onVideoRecvBweStats(VideoRecvBweStats &stats)
{
    if (viewController_) {
        viewController_->onVideoRecvBweStats(stats);
    }
}

void RtcTester::onSystemStats(SystemStats &stats)
{

}


RtcTester& RtcTester::instance()
{
    static RtcTester s_instance;
    return s_instance;
}
