package com.pano.rtc.demo.basicvideocall;


import android.util.Log;

import com.pano.rtc.api.Constants;

public interface PanoWhiteboardHandler {
    void onPageNumberChanged(int curPage, int totalPages);

    void onImageStateChanged(String url, Constants.WBImageState state);

    void onViewScaleChanged(float scale);

    void onRoleTypeChanged(Constants.WBRoleType newRole);

    void onContentUpdated();

    void onSnapshotComplete(Constants.QResult result, String filename);

    void onMessage(long userId, byte[] bytes);

    void onAddBackgroundImages(Constants.QResult result, String fileId);

    void onDocTranscodeStatus(Constants.QResult result, String fileId, int progress, int totalPages);

    void onCreateDoc(Constants.QResult result, String fileId);

    void onDeleteDoc(Constants.QResult result, String fileId);

    void onSwitchDoc(Constants.QResult result, String fileId);
}
