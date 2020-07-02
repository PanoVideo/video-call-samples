package com.pano.rtc.demo.basicvideocall;

import android.util.Log;
import com.pano.rtc.api.Constants;
import com.pano.rtc.api.RtcWhiteboard;

import java.util.ArrayList;

import static com.pano.rtc.base.thread.ThreadUtils.runOnUiThread;


public class PanoWhiteboardCallback implements RtcWhiteboard.Callback {
    private static final String TAG = "PanoCall";
    private ArrayList<PanoWhiteboardHandler> mHandler = new ArrayList<>();

    public void addHandler(PanoWhiteboardHandler handler) {
        mHandler.add(handler);
    }

    public void removeHandler(PanoWhiteboardHandler handler) {
        mHandler.remove(handler);
    }

    // -------------------------- RTC Whiteboard Callbacks --------------------------
    @Override
    public void onPageNumberChanged(int curPage, int totalPages) {
        runOnUiThread(()-> {
            for (PanoWhiteboardHandler handler : mHandler) {
                handler.onPageNumberChanged(curPage, totalPages);
            }
        });
    }
    @Override
    public void onImageStateChanged(String url, Constants.WBImageState state) {
        runOnUiThread(()-> {
            for (PanoWhiteboardHandler handler : mHandler) {
                handler.onImageStateChanged(url, state);
            }
        });
    }
    @Override
    public void onViewScaleChanged(float scale) {
        runOnUiThread(()-> {
            for (PanoWhiteboardHandler handler : mHandler) {
                handler.onViewScaleChanged(scale);
            }
        });
    }
    @Override
    public void onRoleTypeChanged(Constants.WBRoleType newRole) {
        runOnUiThread(()-> {
            for (PanoWhiteboardHandler handler : mHandler) {
                handler.onRoleTypeChanged(newRole);
            }
        });
    }
    @Override
    public void onContentUpdated() {
        runOnUiThread(()-> {
            for (PanoWhiteboardHandler handler : mHandler) {
                handler.onContentUpdated();
            }
        });
    }

    @Override
    public void onSnapshotComplete(Constants.QResult result, String filename) {
        runOnUiThread(()-> {
            for (PanoWhiteboardHandler handler : mHandler) {
                handler.onSnapshotComplete(result, filename);
            }
        });
    }

    @Override
    public void onMessage(long userId, byte[] bytes) {
        runOnUiThread(()-> {
            for (PanoWhiteboardHandler handler : mHandler) {
                handler.onMessage(userId, bytes);
            }
        });
    }

    @Override
    public void onAddBackgroundImages(Constants.QResult result, String fileId) {
        Log.i(TAG, "onAddBackgroundImages " + result + ", " + fileId);
    }

    @Override
    public void onDocTranscodeStatus(Constants.QResult result, String fileId, int progress, int totalPages) {
        Log.i(TAG, "onDocTranscodeStatus " + result + ", " + fileId);
    }

    @Override
    public void onCreateDoc(Constants.QResult result, String fileId) {
        runOnUiThread(()-> {
            for (PanoWhiteboardHandler handler : mHandler) {
                handler.onCreateDoc(result, fileId);
            }
        });
    }

    @Override
    public void onDeleteDoc(Constants.QResult result, String fileId) {
        runOnUiThread(()-> {
            for (PanoWhiteboardHandler handler : mHandler) {
                handler.onDeleteDoc(result, fileId);
            }
        });
    }

    @Override
    public void onSwitchDoc(Constants.QResult result, String fileId) {
        runOnUiThread(()-> {
            for (PanoWhiteboardHandler handler : mHandler) {
                handler.onSwitchDoc(result, fileId);
            }
        });
    }
}
