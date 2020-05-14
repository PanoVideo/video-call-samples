package com.pano.rtc.demo.basicvideocall;


import com.pano.rtc.api.Constants;

public interface PanoEventHandler {
    void onChannelJoinConfirm(Constants.QResult result);

    void onChannelLeaveIndication(Constants.QResult result);

    void onChannelCountDown(long remain);

    void onUserJoinIndication(long userId, String userName);

    void onUserLeaveIndication(long userId, Constants.UserLeaveReason reason);

    void onUserAudioStart(long userId);

    void onUserAudioStop(long userId);

    void onUserAudioSubscribe(long userId, Constants.MediaSubscribeResult result);

    void onUserAudioMute(long userId);

    void onUserAudioUnmute(long userId);

    void onUserVideoStart(long userId, Constants.VideoProfileType maxProfile);

    void onUserVideoStop(long userId);

    void onUserVideoSubscribe(long userId, Constants.MediaSubscribeResult result);

    void onUserVideoMute(long userId);

    void onUserVideoUnmute(long userId);

    void onUserScreenStart(long userId);

    void onUserScreenStop(long userId);

    void onUserScreenSubscribe(long userId, Constants.MediaSubscribeResult result);

    void onUserScreenMute(long userId);

    void onUserScreenUnmute(long userId);

    void onWhiteboardAvailable();

    void onWhiteboardUnavailable();

    void onWhiteboardStart();

    void onWhiteboardStop();

    void onFirstAudioDataReceived(long userId);

    void onFirstVideoDataReceived(long userId);

    void onFirstScreenDataReceived(long userId);

    void onAudioDeviceStateChanged(String deviceId,
                                   Constants.AudioDeviceType deviceType,
                                   Constants.AudioDeviceState deviceState);
    void onVideoDeviceStateChanged(String deviceId,
                                   Constants.VideoDeviceType deviceType,
                                   Constants.VideoDeviceState deviceState);

    void onChannelFailover(Constants.FailoverState state);
}
