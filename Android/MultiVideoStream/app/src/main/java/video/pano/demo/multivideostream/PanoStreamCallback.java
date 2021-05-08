package video.pano.demo.multivideostream;

import com.pano.rtc.api.Constants;
import com.pano.rtc.api.RtcVideoStreamManager;

import java.util.ArrayList;

import static video.pano.rtc.base.thread.ThreadUtils.runOnUiThread;


public class PanoStreamCallback implements RtcVideoStreamManager.Callback {
    private final ArrayList<PanoStreamHandler> mHandler = new ArrayList<>();

    public void addHandler(PanoStreamHandler handler) {
        mHandler.add(handler);
    }

    public void removeHandler(PanoStreamHandler handler) {
        mHandler.remove(handler);
    }

    // -------------------------- RTC Video Stream Manager Callbacks --------------------------
    public void onUserVideoStart(long userId, int streamId, Constants.VideoProfileType maxProfile) {
        runOnUiThread(()-> {
            for (PanoStreamHandler handler : mHandler) {
                handler.onUserVideoStart(userId, streamId, maxProfile);
            }
        });
    }
    public void onUserVideoStop(long userId, int streamId) {
        runOnUiThread(()-> {
            for (PanoStreamHandler handler : mHandler) {
                handler.onUserVideoStop(userId, streamId);
            }
        });
    }
    public void onUserVideoSubscribe(long userId, int streamId, Constants.MediaSubscribeResult result) {
        runOnUiThread(()-> {
            for (PanoStreamHandler handler : mHandler) {
                handler.onUserVideoSubscribe(userId, streamId, result);
            }
        });
    }
    public void onUserVideoMute(long userId, int streamId) {
        runOnUiThread(()-> {
            for (PanoStreamHandler handler : mHandler) {
                handler.onUserVideoMute(userId, streamId);
            }
        });
    }
    public void onUserVideoUnmute(long userId, int streamId) {
        runOnUiThread(()-> {
            for (PanoStreamHandler handler : mHandler) {
                handler.onUserVideoUnmute(userId, streamId);
            }
        });
    }
    public void onFirstVideoDataReceived(long userId, int streamId) {
        runOnUiThread(()-> {
            for (PanoStreamHandler handler : mHandler) {
                handler.onFirstVideoDataReceived(userId, streamId);
            }
        });
    }
    public void onVideoSnapshotCompleted(long userId, int streamId, boolean succeed, String filename) {
        runOnUiThread(()-> {
            for (PanoStreamHandler handler : mHandler) {
                handler.onVideoSnapshotCompleted(userId, streamId, succeed, filename);
            }
        });
    }
}
