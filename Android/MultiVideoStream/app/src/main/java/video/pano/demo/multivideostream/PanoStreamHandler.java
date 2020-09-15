package video.pano.demo.multivideostream;


import com.pano.rtc.api.Constants;

public interface PanoStreamHandler {
    default void onUserVideoStart(long userId, int streamId, Constants.VideoProfileType maxProfile) {}
    default void onUserVideoStop(long userId, int streamId) {}
    default void onUserVideoSubscribe(long userId, int streamId, Constants.MediaSubscribeResult result) {}
    default void onUserVideoMute(long userId, int streamId) {}
    default void onUserVideoUnmute(long userId, int streamId) {}
    default void onFirstVideoDataReceived(long userId, int streamId) {}
    default void onVideoSnapshotCompleted(long userId, int streamId, boolean succeed, String filename) {}
}
