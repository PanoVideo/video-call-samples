package com.pano.rtc.demo.basicvideocall;

import android.os.Bundle;
import android.util.Log;
import android.util.LongSparseArray;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.widget.Toolbar;

import com.pano.rtc.api.Constants;
import com.pano.rtc.api.RtcChannelConfig;
import com.pano.rtc.api.RtcMediaStatsObserver;
import com.pano.rtc.api.RtcView;
import com.pano.rtc.api.model.stats.RtcAudioRecvStats;
import com.pano.rtc.api.model.stats.RtcAudioSendStats;
import com.pano.rtc.api.model.stats.RtcSystemStats;
import com.pano.rtc.api.model.stats.RtcVideoBweStats;
import com.pano.rtc.api.model.stats.RtcVideoRecvStats;
import com.pano.rtc.api.model.stats.RtcVideoSendStats;

import video.pano.RendererCommon;


public class CallActivity extends CallBaseActivity implements PanoEventHandler,
        RtcMediaStatsObserver {
    private static final String TAG = "VideoCall";

    private boolean mAudioSubscribed = false;
    private boolean mVideoSubscribed = false;

    private boolean mScreenSubscribed = false;
    private boolean mScreenStarted = false;

    // 为订阅用户信息，因为总共有5个视图，所以最多能同时订阅5个用户视频
    static class VideoUserInfo {
        long userId;
        Constants.VideoProfileType maxProfile;
    }
    private LongSparseArray<VideoUserInfo> mUnsubscribedVideoUsers = new LongSparseArray<>();

    // 用于保存用户视图信息，
    class UserViewInfo {
        long userId;
        RtcView view;
        TextView textView; // 打印用户ID信息
        boolean isFree = true; // 此视图是否空闲
        boolean isScreen = false; // 此视图是否正在显示桌面共享
        Constants.VideoProfileType maxProfile; // 此用户发布的最大视频能力
        Constants.VideoProfileType subProfile; // 此用户的当前订阅能力

        void initView(int index, RtcView v, TextView tv) {
            this.view = v;
            this.textView = tv;
            this.view.setScalingType(scalingType);
            this.view.init(new RendererCommon.RendererEvents() {
                @Override
                public void onFirstFrameRendered() {}
                @Override
                public void onFrameResolutionChanged(int i, int i1, int i2) {}
            });
            // 设置长按小图时将此小图的用户和大图用户交换视图
            this.view.setOnLongClickListener(vv -> {
                switchToLargeView(index);
                return true;
            });
            if (index != 0) {
                this.view.setZOrderMediaOverlay(true);
            }
        }

        void setVisible(boolean visible) {
            view.setVisibility(visible ? View.VISIBLE : View.GONE);
            textView.setVisibility(visible ? View.VISIBLE : View.GONE);
        }

        void setUser(long userId) {
            this.userId = userId;
            textView.setText("" + userId);
        }
    }

    // 视图数组，总共5个视频视图，1个大图4个小图，0为大图，其他为小图
    private int mUserViewCount = 5;
    UserViewInfo[] mUserViewArray = new UserViewInfo[mUserViewCount];
    {
        for (int i=0; i < mUserViewCount; i++) {
            mUserViewArray[i] = new UserViewInfo();
        }
    }

    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_call);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        PanoApplication app = (PanoApplication)getApplication();
        mRtcEngine = app.getPanoEngine();
        app.registerEventHandler(this);

        // 设置点击view时显示或隐藏控制按钮
        View rootView = findViewById(android.R.id.content);
        rootView.setOnClickListener(v -> {
            if (mIsControlPanelShowed) {
                hideControlPanel();
            } else {
                showControlPanel();
            }
        });

        try
        {
            this.getSupportActionBar().hide();
        }
        catch (NullPointerException e){}

        // 设置媒体统计信息的接收者
        mRtcEngine.setMediaStatsObserver(this);
        // 设置是否使用设备扬声器
        mRtcEngine.setLoudspeakerStatus(mLoudspeaker);

        // 配置大图的视图参数
        mUserViewArray[0].initView(0,
                findViewById(R.id.large_size_view),
                findViewById(R.id.tv_large_view_user));
        mUserViewArray[0].view.setOnLongClickListener(null);
        // 设置点击大图时显示或隐藏控制按钮
        mUserViewArray[0].view.setOnClickListener(v -> {
            if (mIsControlPanelShowed) {
                hideControlPanel();
            } else {
                showControlPanel();
            }
        });

        // 配置左上角小图的视图参数
        mUserViewArray[1].initView(1,
                findViewById(R.id.small_size_view_righttop),
                findViewById(R.id.tv_small_view_righttop_user));

        // 配置右上角小图的视图参数
        mUserViewArray[2].initView(2,
                findViewById(R.id.small_size_view_lefttop),
                findViewById(R.id.tv_small_view_lefttop_user));

        // 配置左下角小图的视图参数
        mUserViewArray[3].initView(3,
                findViewById(R.id.small_size_view_leftbottom),
                findViewById(R.id.tv_small_view_leftbottom_user));

        // 配置右下角小图的视图参数，此视图一般用于显示本地用户视频
        mUserViewArray[4].initView(4,
                findViewById(R.id.small_size_view_rightbottom),
                findViewById(R.id.tv_small_view_rightbottom_user));

        // 启动视频预览，并且显示到大图
        if (mVideoMode) {
            mUserViewArray[0].isFree = false;
            mUserViewArray[0].setUser(mUserId);
            updateLocalVideoRender(mUserViewArray[0].view);
            mRtcEngine.startPreview(ToVideoProfileType(mLocalProfile), mFrontCamera);
        }

        joinChannel();
    }

    @Override
    public void onBackPressed() {
        if (mIsChannelJoined) {
            leaveChannel();
        }
        int count = getSupportFragmentManager().getBackStackEntryCount();
        for (int i=0; i<count; i++) {
            getSupportFragmentManager().popBackStack();
        }
        super.onBackPressed();
    }

    @Override
    protected void onDestroy() {
        releaseView();
        PanoApplication app = (PanoApplication)getApplication();
        app.removeEventHandler(this);
        mRtcEngine = null;
        super.onDestroy();
    }

    // 加入频道
    private boolean joinChannel() {
        RtcChannelConfig config = new RtcChannelConfig();
        config.userName = mUserName;
        config.mode_1v1 = mMode1v1;
        // enable media service only
        config.serviceFlags = Constants.kChannelServiceMedia;
        // 设置自动订阅所有音频
        config.subscribeAudioAll = true;
        Constants.QResult ret = mRtcEngine.joinChannel(mAppToken, mChannelId, mUserId, config);
        if (ret != Constants.QResult.OK) {
            Toast.makeText(CallActivity.this, "joinChannel failed, result=" + ret, Toast.LENGTH_LONG).show();
            return false;
        }

        return true;
    }

    // 离开当前频道
    private void leaveChannel() {
        mRtcEngine.stopVideo();
        mRtcEngine.stopAudio();
        mRtcEngine.leaveChannel();
        mUnsubscribedVideoUsers.clear();
        for (int i=0; i < mUserViewCount; i++) {
            mUserViewArray[i].isFree = true;
        }
    }

    private void releaseView(){
        for (int i=0; i < mUserViewCount; i++) {
            mUserViewArray[i].view.release();
            mUserViewArray[i].view = null;
            mUserViewArray[i].isFree = true;
        }
    }


    // -------------------------- RTC Engine Callbacks --------------------------
    public void onChannelJoinConfirm(Constants.QResult result) {
        Log.i(TAG, "onChannelJoinConfirm, result="+result);
        if (result == Constants.QResult.OK) {
            mIsChannelJoined = true;
            Toast.makeText(CallActivity.this, "onChannelJoinConfirm success", Toast.LENGTH_LONG).show();
            mRtcEngine.stopPreview();
            // 启动本地视频
            if(mVideoMode){
                updateLocalVideoRender(mUserViewArray[0].view);
                mRtcEngine.startVideo(ToVideoProfileType(mLocalProfile), mFrontCamera);
            }
            // 启动本地音频
            mRtcEngine.startAudio();
        } else {
            mIsChannelJoined = false;
            Toast.makeText(CallActivity.this, "onChannelJoinConfirm result=" + result, Toast.LENGTH_LONG).show();
        }
    }

    public void onChannelLeaveIndication(Constants.QResult result) {
        Log.i(TAG, "onChannelLeaveIndication, result="+result);
        mIsChannelJoined = false;
        Toast.makeText(CallActivity.this, "onChannelLeaveIndication result=" + result, Toast.LENGTH_LONG).show();
        finish();
    }
    public void onChannelCountDown(long remain) {
        Log.i(TAG, "onChannelCountDown, remain="+remain);
    }
    public void onUserJoinIndication(long userId, String userName) {
        Log.i(TAG, "onUserJoinIndication, userId="+userId+", userName="+userName);
        Toast.makeText(CallActivity.this, "onUserJoinIndication userId=" + userId, Toast.LENGTH_LONG).show();
    }
    public void onUserLeaveIndication(long userId, Constants.UserLeaveReason reason) {
        Log.i(TAG, "onUserLeaveIndication, userId="+userId);
        Toast.makeText(CallActivity.this, "onUserLeaveIndication userId=" + userId + ", reason=" + reason, Toast.LENGTH_LONG).show();
        // 取消订阅此用户视频
        stopUserVideo(userId);
        stopUserScreen(userId);
    }
    public void onUserAudioStart(long userId) {
        Log.i(TAG, "onUserAudioStart, userId="+userId);
        // 设置了自动订阅用户音频，所以无需手动订阅
    }
    public void onUserAudioStop(long userId) {
        Log.i(TAG, "onUserAudioStop, userId="+userId);
    }
    public void onUserAudioSubscribe(long userId, Constants.MediaSubscribeResult result) {
        Log.i(TAG, "onUserAudioSubscribe, userId="+userId + ", result=" + result);
    }
    public void onUserVideoStart(long userId, Constants.VideoProfileType maxProfile) {
        Log.i(TAG, "onUserVideoStart, userId="+userId+", profile="+maxProfile);
        if (!mVideoMode) {
            return;
        }

        int viewIndex = -1;
        // 先检查大图是否空闲，如空闲则将此用户显示到大图
        if (mUserViewArray[0].isFree || mUserViewArray[0].userId == mUserId) {
            // large view is free or used by local user, then make this user to large view
            // 如果大图被本地用户占用，则将本地用户移到小图，如果没有空闲的小图，则不显示本地用户视频
            if (mUserViewArray[0].userId == mUserId) {
                // move local user to last view
                mLocalView = null;
                if (mUserViewArray[mUserViewCount-1].isFree) {
                    mLocalView = mUserViewArray[mUserViewCount-1].view;
                    mUserViewArray[mUserViewCount-1].setVisible(true);
                    mUserViewArray[mUserViewCount-1].isFree = false;
                    mUserViewArray[mUserViewCount-1].isScreen = false;
                    mUserViewArray[mUserViewCount-1].setUser(mUserId);
                }
                updateLocalVideoRender(mLocalView);
            }
            mUserViewArray[0].setUser(userId);
            mUserViewArray[0].isFree = false;
            mUserViewArray[0].isScreen = false;
            mUserViewArray[0].maxProfile = maxProfile;
            viewIndex = 0;
        } else {
            // 如大图不空闲，则尝试找到一个空闲的小图
            // try to find a free small view
            for (int i=1; i < mUserViewCount; i++) {
                if (mUserViewArray[i].isFree) {
                    mUserViewArray[i].setUser(userId);
                    mUserViewArray[i].isFree = false;
                    mUserViewArray[i].isScreen = false;
                    viewIndex = i;
                    break;
                }
            }
        }

        if (viewIndex != -1) {
            Constants.VideoProfileType profile = Constants.VideoProfileType.Lowest;
            if (viewIndex == 0) {
                profile = ToVideoProfileType(Math.min(mRemoteProfile, maxProfile.getValue()));
            }
            // 订阅此用户视频到指定视图
            if (subscribeUserVideo(userId, mUserViewArray[viewIndex], profile)) {
                mUserViewArray[viewIndex].maxProfile = maxProfile;
                mUserViewArray[viewIndex].subProfile = profile;
            } else {
                mUserViewArray[viewIndex].isFree = true;
            }
        } else {
            // 如果没有空闲视图，将此用户加入未订阅视频用户列表，以等待空闲视图
            // no view available
            VideoUserInfo vui = new VideoUserInfo();
            vui.userId = userId;
            vui.maxProfile = maxProfile;
            mUnsubscribedVideoUsers.put(userId, vui);
        }
    }
    public void onUserVideoStop(long userId) {
        Log.i(TAG, "onUserVideoStop, userId="+userId);
        // 取消订阅此用户视频
        if(mRtcEngine.unsubscribeVideo(userId) == Constants.QResult.OK){
            stopUserVideo(userId);
        }
    }
    public void onUserVideoSubscribe(long userId, Constants.MediaSubscribeResult result) {
        Log.i(TAG, "onUserVideoSubscribe, userId="+userId + ", result=" + result);
    }
    public void onUserAudioMute(long userId) {}
    public void onUserAudioUnmute(long userId) {}
    public void onUserVideoMute(long userId) {}
    public void onUserVideoUnmute(long userId) {}

    public void onUserScreenStart(long userId) {
        Log.i(TAG, "onUserScreenStart, userId="+userId);
        mScreenStarted = true;
        if (!mVideoMode) {
            return;
        }
        // 有用户开启桌面共享，始终显示桌面共享到大图
        if (!mUserViewArray[0].isFree) {
            // 如果大图已经在显示桌面共享，则首先取消订阅之前的共享
            if (mUserViewArray[0].isScreen) {
                mRtcEngine.unsubscribeScreen(mUserViewArray[0].userId);
            } else if (mUserViewArray[0].userId == mUserId) {
                // 如果大图被本地用户占用，则将本地用户移到小图，如果没有空闲的小图，则不显示本地用户视频
                // large view is used by local user, move local user to last view
                mLocalView = null;
                if (mUserViewArray[mUserViewCount-1].isFree) {
                    mLocalView = mUserViewArray[mUserViewCount-1].view;
                    mUserViewArray[mUserViewCount-1].setVisible(true);
                    mUserViewArray[mUserViewCount-1].isFree = false;
                    mUserViewArray[mUserViewCount-1].isScreen = false;
                    mUserViewArray[mUserViewCount-1].setUser(mUserId);
                }
                updateLocalVideoRender(mLocalView);
            } else {
                // 尝试找到一个空闲的小图给当前大图用户
                // try to find a free small view
                int viewIndex = -1;
                for (int i=1; i < mUserViewCount; i++) {
                    if (mUserViewArray[i].isFree) {
                        mUserViewArray[i].setUser(mUserViewArray[0].userId);
                        mUserViewArray[i].isFree = false;
                        mUserViewArray[i].isScreen = false;
                        viewIndex = i;
                        break;
                    }
                }
                if (viewIndex != -1) {
                    // 找到了一个空闲视图，将大图用户迁移到此空闲视图
                    Constants.VideoProfileType profile = Constants.VideoProfileType.Lowest;
                    if (subscribeUserVideo(mUserViewArray[0].userId, mUserViewArray[viewIndex], profile)) {
                        mUserViewArray[viewIndex].maxProfile = mUserViewArray[0].maxProfile;
                        mUserViewArray[viewIndex].subProfile = profile;
                    } else {
                        mRtcEngine.unsubscribeVideo(mUserViewArray[0].userId);
                        mUserViewArray[viewIndex].isFree = true;
                    }
                } else {
                    // 找不到空闲视图，取消订阅此用户，并将此用户加入未订阅视频用户列表，以等待空闲视图
                    // no view available
                    mRtcEngine.unsubscribeVideo(mUserViewArray[0].userId);
                    VideoUserInfo vui = new VideoUserInfo();
                    vui.userId = mUserViewArray[0].userId;
                    vui.maxProfile = mUserViewArray[0].maxProfile;
                    mUnsubscribedVideoUsers.put(vui.userId, vui);
                }
            }
            mUserViewArray[0].isFree = true;
            mUserViewArray[0].isScreen = false;
        }

        // 订阅此用户桌面共享，并显示到大图
        if (subscribeUserScreen(userId, mUserViewArray[0])) {
            //mUserViewArray[0].maxProfile = Constants.VideoProfileType.HD1080P;
            //mUserViewArray[0].subProfile = Constants.VideoProfileType.HD1080P;
        } else {
            mUserViewArray[0].isFree = true;
        }
    }
    public void onUserScreenStop(long userId) {
        Log.i(TAG, "onUserScreenStop, userId="+userId);
        // 取消订阅此用户桌面共享
        mRtcEngine.unsubscribeScreen(userId);
        stopUserScreen(userId);
    }
    public void onUserScreenSubscribe(long userId, Constants.MediaSubscribeResult result) {
        Log.i(TAG, "onUserScreenSubscribe, userId="+userId + ", result=" + result);
    }
    public void onUserScreenMute(long userId) {}
    public void onUserScreenUnmute(long userId) {}

    @Override
    public void onWhiteboardAvailable() {

    }

    @Override
    public void onWhiteboardUnavailable() {

    }

    @Override
    public void onWhiteboardStart() {

    }

    @Override
    public void onWhiteboardStop() {

    }

    @Override
    public void onFirstAudioDataReceived(long userId) {
        Log.i(TAG, "+++++ onFirstAudioDataReceived: userId="+userId);
    }

    @Override
    public void onFirstVideoDataReceived(long userId) {
        Log.i(TAG, "+++++ onFirstVideoDataReceived: userId="+userId);
    }

    @Override
    public void onFirstScreenDataReceived(long userId) {
        Log.i(TAG, "+++++ onFirstScreenDataReceived: userId="+userId);
    }

    @Override
    public void onAudioDeviceStateChanged(String deviceId,
                                          Constants.AudioDeviceType deviceType,
                                          Constants.AudioDeviceState deviceState) {
        Log.i(TAG, "+++++ onAudioDeviceStateChanged: "+deviceId);
    }

    @Override
    public void onVideoDeviceStateChanged(String deviceId,
                                          Constants.VideoDeviceType deviceType,
                                          Constants.VideoDeviceState deviceState) {
        Log.i(TAG, "+++++ onVideoDeviceStateChanged: "+deviceId);
    }

    @Override
    public void onChannelFailover(Constants.FailoverState state) {
        Log.i(TAG, "+++++ onChannelFailover: state="+state.getValue());
    }

    // 订阅用户视频
    private boolean subscribeUserVideo(long userId, UserViewInfo viewInfo, Constants.VideoProfileType profile) {
        viewInfo.view.setMirror(false);
        viewInfo.setUser(userId);
        viewInfo.setVisible(true);
        viewInfo.isFree = false;
        viewInfo.isScreen = false;
        mRtcEngine.setRemoteVideoRender(userId, viewInfo.view);

        Constants.QResult ret = mRtcEngine.subscribeVideo(userId, profile);
        if(ret == Constants.QResult.OK){
            return true;
        }else {
            String msg = "subscribeUserVideo failed, userId=" + userId + ", result=" + ret;
            Log.w(TAG, msg);
            Toast.makeText(CallActivity.this, msg, Toast.LENGTH_LONG).show();
            return false;
        }
    }

    // 订阅用户桌面共享
    private boolean subscribeUserScreen(long userId, UserViewInfo viewInfo) {
        viewInfo.view.setMirror(false);
        viewInfo.setUser(userId);
        viewInfo.setVisible(true);
        viewInfo.isFree = false;
        viewInfo.isScreen = true;
        mRtcEngine.setRemoteScreenRender(userId, viewInfo.view);

        Constants.QResult ret = mRtcEngine.subscribeScreen(userId);
        if(ret == Constants.QResult.OK){
            return true;
        }else {
            String msg = "subscribeUserScreen failed, userId=" + userId + ", result=" + ret;
            Log.w(TAG, msg);
            Toast.makeText(CallActivity.this, msg, Toast.LENGTH_LONG).show();
            return false;
        }
    }

    // 将index指定的视图用户和大图用户交换
    private void switchToLargeView(int index) {
        if (index < 1 || index > 4) {
            return;
        }
        if (mUserViewArray[index].isFree) {
            return;
        }
        if (!mUserViewArray[0].isFree && mUserViewArray[0].isScreen) {
            return;
        }

        long userId = mUserViewArray[index].userId;
        Constants.VideoProfileType maxProfile = mUserViewArray[index].maxProfile;
        boolean isScreen = mUserViewArray[index].isScreen;
        mUserViewArray[index].setUser(mUserViewArray[0].userId);
        mUserViewArray[index].maxProfile = mUserViewArray[0].maxProfile;
        mUserViewArray[index].isScreen = mUserViewArray[0].isScreen;
        if (mUserViewArray[index].userId == mUserId) {
            updateLocalVideoRender(mUserViewArray[index].view);
        } else {
            if (mUserViewArray[index].isScreen) {
                mRtcEngine.setRemoteScreenRender(mUserViewArray[index].userId, mUserViewArray[index].view);
            } else {
                mRtcEngine.setRemoteVideoRender(mUserViewArray[index].userId, mUserViewArray[index].view);
                mRtcEngine.subscribeVideo(mUserViewArray[index].userId, Constants.VideoProfileType.Lowest);
            }
        }

        mUserViewArray[0].setUser(userId);
        mUserViewArray[0].maxProfile = maxProfile;
        mUserViewArray[0].isScreen = isScreen;
        if (mUserViewArray[0].userId == mUserId) {
            updateLocalVideoRender(mUserViewArray[0].view);
        } else {
            if (mUserViewArray[0].isScreen) {
                mRtcEngine.setRemoteScreenRender(mUserViewArray[0].userId, mUserViewArray[0].view);
            } else {
                mRtcEngine.setRemoteVideoRender(mUserViewArray[0].userId, mUserViewArray[0].view);
                Constants.VideoProfileType profile = ToVideoProfileType(Math.min(mRemoteProfile,
                        mUserViewArray[0].maxProfile.getValue()));
                mRtcEngine.subscribeVideo(mUserViewArray[0].userId, profile);
            }
        }
    }

    // 取消订阅用户视频
    private void stopUserVideo(long userId) {
        for (int i=0; i < mUserViewCount; i++) {
            if (userId == mUserViewArray[i].userId && !mUserViewArray[i].isScreen) {
                stopUserView(userId, i);
                break;
            }
        }
    }

    // 取消订阅用户桌面共享
    private void stopUserScreen(long userId) {
        if (userId == mUserViewArray[0].userId && mUserViewArray[0].isScreen) {
            stopUserView(userId, 0);
        }
    }

    private void stopUserView(long userId, int index) {
        mUserViewArray[index].isScreen = false; // reset screen flag
        // check if there is unsubscribed video user, and subscribe it if any
        if (mUnsubscribedVideoUsers.size() > 0) {
            VideoUserInfo vui = mUnsubscribedVideoUsers.valueAt(0);
            mUnsubscribedVideoUsers.removeAt(0);

            Constants.VideoProfileType profile = Constants.VideoProfileType.Lowest;
            if (index == 0) {
                profile = ToVideoProfileType(Math.min(mRemoteProfile, vui.maxProfile.getValue()));
            }
            if (subscribeUserVideo(vui.userId, mUserViewArray[index], profile)) {
                mUserViewArray[index].maxProfile = vui.maxProfile;
                mUserViewArray[index].subProfile = profile;
                return;
            }
        }

        // if large view user left, then try to move other remote user to large view
        if (index == 0) { // is large view
            for (int j = mUserViewCount - 1; j > 0; j--) {
                if (!mUserViewArray[j].isFree && mUserViewArray[j].userId != mUserId) {
                    mUserViewArray[0].userId = mUserViewArray[j].userId;
                    mUserViewArray[0].isFree = false;
                    mUserViewArray[0].maxProfile = mUserViewArray[j].maxProfile;
                    mUserViewArray[0].subProfile = mUserViewArray[j].subProfile;
                    mUserViewArray[j].isFree = true;
                    mUserViewArray[j].setVisible(false);

                    Constants.VideoProfileType profile = ToVideoProfileType(Math.min(mRemoteProfile,
                            mUserViewArray[0].maxProfile.getValue()));
                    // resubscribe the video
                    subscribeUserVideo(mUserViewArray[0].userId, mUserViewArray[0], profile);
                    mUserViewArray[0].subProfile = profile;
                    return;
                }
            }
        }

        mUserViewArray[index].isFree = true;
        mUserViewArray[index].setVisible(false);
    }

    // 更新本地用户视频的视图
    private void updateLocalVideoRender(RtcView view) {
        mLocalView = view;
        if (mLocalView != null) {
            mLocalView.setMirror(mFrontCamera);
        }
        mRtcEngine.setLocalVideoRender(mLocalView);
    }

    // 统计信息接收
    // -------------------------- RTC Media Stats Callbacks --------------------------
    @Override
    public void onVideoSendStats(RtcVideoSendStats stats) {
        runOnUiThread(()-> {
            mMediaStats.onVideoSendStats(stats);
        });
    }

    @Override
    public void onVideoRecvStats(RtcVideoRecvStats stats) {
        runOnUiThread(()-> {
            if (stats.userId == mUserViewArray[0].userId) {
                mMediaStats.onVideoRecvStats(stats);
            }
        });
    }

    @Override
    public void onAudioSendStats(RtcAudioSendStats stats) {
        runOnUiThread(()-> {
            mMediaStats.onAudioSendStats(stats);
        });
    }

    @Override
    public void onAudioRecvStats(RtcAudioRecvStats stats) {
        runOnUiThread(()-> {
            if (stats.userId == mUserViewArray[0].userId) {
                mMediaStats.onAudioRecvStats(stats);
            }
        });
    }

    @Override
    public void onScreenSendStats(RtcVideoSendStats stats) {
        runOnUiThread(()-> {
            mMediaStats.onVideoSendStats(stats);
        });
    }

    @Override
    public void onScreenRecvStats(RtcVideoRecvStats stats) {
        runOnUiThread(()-> {
            if (stats.userId == mUserViewArray[0].userId && mUserViewArray[0].isScreen) {
                mMediaStats.onVideoRecvStats(stats);
            }
        });
    }

    @Override
    public void onVideoBweStats(RtcVideoBweStats stats) {
        runOnUiThread(()-> {
            mMediaStats.onVideoBweStats(stats);
        });
    }

    @Override
    public void onSystemStats(RtcSystemStats stats) {
        runOnUiThread(()-> {
            mMediaStats.onSystemStats(stats);
        });
    }

}
