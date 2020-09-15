package video.pano.demo.multivideostream;


import com.pano.rtc.api.Constants;

public interface PanoEventHandler {
    void onChannelJoinConfirm(Constants.QResult result);

    void onChannelLeaveIndication(Constants.QResult result);

    default void onChannelCountDown(long remain) {}

    default void onUserJoinIndication(long userId, String userName) {}

    default void onUserLeaveIndication(long userId, Constants.UserLeaveReason reason) {}

    default void onUserAudioStart(long userId) {}

    default void onUserAudioStop(long userId) {}

    default void onUserAudioSubscribe(long userId, Constants.MediaSubscribeResult result) {}

    default void onUserAudioMute(long userId) {}

    default void onUserAudioUnmute(long userId) {}

    default void onUserVideoStart(long userId, Constants.VideoProfileType maxProfile) {}

    default void onUserVideoStop(long userId) {}

    default void onUserVideoSubscribe(long userId, Constants.MediaSubscribeResult result) {}

    default void onUserVideoMute(long userId) {}

    default void onUserVideoUnmute(long userId) {}

    default void onUserScreenStart(long userId) {}

    default void onUserScreenStop(long userId) {}

    default void onUserScreenSubscribe(long userId, Constants.MediaSubscribeResult result) {}

    default void onUserScreenMute(long userId) {}

    default void onUserScreenUnmute(long userId) {}

    default void onWhiteboardAvailable() {}

    default void onWhiteboardUnavailable() {}

    default void onWhiteboardStart() {}

    default void onWhiteboardStop() {}

    default void onWhiteboardStart(String whiteboardId) {}

    default void onWhiteboardStop(String whiteboardId) {}

    default void onFirstAudioDataReceived(long userId) {}

    default void onFirstVideoDataReceived(long userId) {}

    default void onFirstScreenDataReceived(long userId) {}

    default void onAudioDeviceStateChanged(String deviceId,
                                           Constants.AudioDeviceType deviceType,
                                           Constants.AudioDeviceState deviceState) {}
    default void onVideoDeviceStateChanged(String deviceId,
                                           Constants.VideoDeviceType deviceType,
                                           Constants.VideoDeviceState deviceState) {}

    default void onChannelFailover(Constants.FailoverState state) {}

    default void onActiveSpeakerListUpdated(long[] userIds) {}

    default void onVideoSnapshotCompleted(boolean succeed, long userId, String filename) {}

    default void onNetworkQuality(long userId, Constants.QualityRating quality) {}
}
