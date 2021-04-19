#pragma once

#include "PanoRtcDefs.h"
#include "PanoRtcStats.h"
#include "TestDefs.h"
#include "UIThreadExecutor.h"

class RtcViewController
{
public:
    using Task = UIThreadExecutor::Task;

    virtual ~RtcViewController() {}

    virtual void onAuthFailure(panortc::QResult result) = 0;
    virtual void onChannelJoinConfirm(panortc::QResult result) = 0;
    virtual void onChannelLeaveIndication(panortc::QResult result) = 0;
    virtual void onUserJoinIndication(uint64_t userId) = 0;
    virtual void onUserLeaveIndication(uint64_t userId, panortc::UserLeaveReason reason) = 0;
    virtual void onUserVideoStart(uint64_t userId, panortc::VideoProfileType maxProfile) = 0;
    virtual void onUserVideoStop(uint64_t userId) = 0;
    virtual void onUserScreenStart(uint64_t userId) = 0;
    virtual void onUserScreenStop(uint64_t userId) = 0;

    virtual void onWhiteboardAvailable() {}
    virtual void onWhiteboardUnavailable() {}
    virtual void onWhiteboardStart(const char *whiteboardId) {};
    virtual void onWhiteboardStop(const char *whiteboardId) {};

    virtual void onAudioDeviceStateChanged(const char deviceID[panortc::kMaxDeviceIDLength],
        panortc::AudioDeviceType deviceType,
        panortc::AudioDeviceState deviceState) = 0;
    virtual void onVideoDeviceStateChanged(const char deviceID[panortc::kMaxDeviceIDLength],
        panortc::VideoDeviceType deviceType,
        panortc::VideoDeviceState deviceState) = 0;

    virtual void onVideoSendStats(panortc::VideoSendStats &stats) = 0;
    virtual void onVideoRecvStats(panortc::VideoRecvStats &stats) = 0;
    virtual void onScreenSendStats(panortc::ScreenSendStats &stats) = 0;
    virtual void onScreenRecvStats(panortc::ScreenRecvStats &stats) = 0;
    virtual void onAudioSendStats(panortc::AudioSendStats &stats) = 0;
    virtual void onAudioRecvStats(panortc::AudioRecvStats &stats) = 0;
    virtual void onVideoSendBweStats(panortc::VideoSendBweStats &stats) = 0;
    virtual void onVideoRecvBweStats(panortc::VideoRecvBweStats &stats) = 0;

    virtual void asyncRunOnUIThread(Task t) = 0;
};

