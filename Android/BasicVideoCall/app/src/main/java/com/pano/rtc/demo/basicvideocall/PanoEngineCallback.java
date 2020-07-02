package com.pano.rtc.demo.basicvideocall;

import com.pano.rtc.api.Constants;
import com.pano.rtc.api.RtcEngineCallback;

import java.util.ArrayList;

import static com.pano.rtc.demo.basicvideocall.ThreadUtils.runOnUiThread;


public class PanoEngineCallback implements RtcEngineCallback {
    private ArrayList<PanoEventHandler> mHandler = new ArrayList<>();

    public void addHandler(PanoEventHandler handler) {
        mHandler.add(handler);
    }

    public void removeHandler(PanoEventHandler handler) {
        mHandler.remove(handler);
    }

    // -------------------------- RTC Engine Callbacks --------------------------
    public void onChannelJoinConfirm(Constants.QResult result) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onChannelJoinConfirm(result);
            }
        });
    }

    public void onChannelLeaveIndication(Constants.QResult result) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onChannelLeaveIndication(result);
            }
        });
    }
    public void onChannelCountDown(long remain) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onChannelCountDown(remain);
            }
        });
    }
    public void onUserJoinIndication(long userId, String userName) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserJoinIndication(userId, userName);
            }
        });
    }
    public void onUserLeaveIndication(long userId, Constants.UserLeaveReason reason) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserLeaveIndication(userId, reason);
            }
        });
    }
    public void onUserAudioStart(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserAudioStart(userId);
            }
        });
    }
    public void onUserAudioStop(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserAudioStop(userId);
            }
        });
    }
    public void onUserAudioSubscribe(long userId, Constants.MediaSubscribeResult result) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserAudioSubscribe(userId, result);
            }
        });
    }
    public void onUserVideoStart(long userId, Constants.VideoProfileType maxProfile) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserVideoStart(userId, maxProfile);
            }
        });
    }
    public void onUserVideoStop(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserVideoStop(userId);
            }
        });
    }
    public void onUserVideoSubscribe(long userId, Constants.MediaSubscribeResult result) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserVideoSubscribe(userId, result);
            }
        });
    }
    public void onUserAudioMute(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserAudioMute(userId);
            }
        });
    }
    public void onUserAudioUnmute(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserAudioUnmute(userId);
            }
        });
    }
    public void onUserVideoMute(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserVideoMute(userId);
            }
        });
    }
    public void onUserVideoUnmute(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserVideoUnmute(userId);
            }
        });
    }

    public void onUserScreenStart(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserScreenStart(userId);
            }
        });
    }
    public void onUserScreenStop(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserScreenStop(userId);
            }
        });
    }
    public void onUserScreenSubscribe(long userId, Constants.MediaSubscribeResult result) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserScreenSubscribe(userId, result);
            }
        });
    }
    public void onUserScreenMute(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserScreenMute(userId);
            }
        });
    }
    public void onUserScreenUnmute(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onUserScreenUnmute(userId);
            }
        });
    }

    @Override
    public void onWhiteboardAvailable() {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onWhiteboardAvailable();
            }
        });
    }

    @Override
    public void onWhiteboardUnavailable() {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onWhiteboardUnavailable();
            }
        });
    }

    @Override
    public void onWhiteboardStart() {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onWhiteboardStart();
            }
        });
    }

    @Override
    public void onWhiteboardStop() {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onWhiteboardStop();
            }
        });
    }

    @Override
    public void onFirstAudioDataReceived(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onFirstAudioDataReceived(userId);
            }
        });
    }

    @Override
    public void onFirstVideoDataReceived(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onFirstVideoDataReceived(userId);
            }
        });
    }

    @Override
    public void onFirstScreenDataReceived(long userId) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onFirstScreenDataReceived(userId);
            }
        });
    }

    @Override
    public void onAudioDeviceStateChanged(String deviceId,
                                          Constants.AudioDeviceType deviceType,
                                          Constants.AudioDeviceState deviceState) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onAudioDeviceStateChanged(deviceId, deviceType, deviceState);
            }
        });
    }

    @Override
    public void onVideoDeviceStateChanged(String deviceId,
                                          Constants.VideoDeviceType deviceType,
                                          Constants.VideoDeviceState deviceState) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onVideoDeviceStateChanged(deviceId, deviceType, deviceState);
            }
        });
    }

    @Override
    public void onChannelFailover(Constants.FailoverState state) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onChannelFailover(state);
            }
        });
    }

    @Override
    public void onActiveSpeakerListUpdated(long[] userIds) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onActiveSpeakerListUpdated(userIds);
            }
        });
    }

    @Override
    public void onVideoSnapshotCompleted(boolean succeed, long userId, String filename) {
        runOnUiThread(()-> {
            for (PanoEventHandler handler : mHandler) {
                handler.onVideoSnapshotCompleted(succeed, userId, filename);
            }
        });
    }
}
