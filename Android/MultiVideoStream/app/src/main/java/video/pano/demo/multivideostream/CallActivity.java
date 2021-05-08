package video.pano.demo.multivideostream;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentTransaction;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.util.LongSparseArray;
import android.util.SparseArray;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import com.pano.rtc.api.Constants;
import com.pano.rtc.api.IVideoRender;
import com.pano.rtc.api.RtcChannelConfig;
import com.pano.rtc.api.RtcEngine;
import com.pano.rtc.api.RtcVideoStreamManager;
import com.pano.rtc.api.RtcView;
import com.pano.rtc.api.model.RtcDeviceInfo;

import video.pano.RendererCommon;

import static com.pano.rtc.api.IVideoRender.ScalingType.SCALE_ASPECT_FILL;

public class CallActivity extends AppCompatActivity implements PanoEventHandler,
        PanoStreamHandler, ControlPanel.OnControlPanelListener {
    private static final String TAG = "VideoCall";

    private IVideoRender.ScalingType mScalingType = SCALE_ASPECT_FILL;
    protected int mLocalProfile = 3; // HD720P
    protected int mRemoteProfile = 5; // Max

    protected boolean mLoudspeaker = false;

    protected String mAppToken = "";
    protected String mChannelId = "";
    protected long mUserId = 0;
    protected String mUserName = "";
    protected boolean mMode1v1 = false;

    protected RtcEngine mRtcEngine = null;
    protected boolean mIsChannelJoined = false;
    protected boolean mFrontCamera = true;

    private ControlPanel mControlPanel;
    protected boolean mIsControlPanelShowed = false;

    private final int[] mVideoStreams = new int[] {-1, -1};


    static class VideoInfo {
        int streamId;
        Constants.VideoProfileType maxProfile;
    }
    // 未订阅用户信息，因为总共有5个视图，所以最多能同时订阅5个用户视频
    static class VideoUserInfo {
        long userId;
        SparseArray<VideoInfo> videoInfos = new SparseArray<>();
    }
    private final LongSparseArray<VideoUserInfo> mUnsubscribedVideoUsers = new LongSparseArray<>();

    // 用于保存用户视图信息，
    class UserViewInfo {
        long userId;
        int streamId;
        RtcView view;
        TextView textView; // 打印用户ID信息
        boolean isFree = true; // 此视图是否空闲
        boolean isScreen = false; // 此视图是否正在显示桌面共享
        Constants.VideoProfileType maxProfile; // 此用户发布的最大视频能力
        Constants.VideoProfileType subProfile; // 此用户的当前订阅能力

        void initView(int index, RtcView v, TextView tv) {
            this.view = v;
            this.textView = tv;
            this.view.setScalingType(mScalingType);
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

        void setUser(long userId, int streamId) {
            this.userId = userId;
            this.streamId = streamId;
            textView.setText("" + userId + "-" + streamId);
        }
    }

    // 视图数组，总共5个视频视图，1个大图4个小图，0为大图，其他为小图
    private final int mUserViewCount = 5;
    UserViewInfo[] mUserViewArray = new UserViewInfo[mUserViewCount];
    {
        for (int i=0; i < mUserViewCount; i++) {
            mUserViewArray[i] = new UserViewInfo();
        }
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_call);

        setScreenOnFlag();
        try
        {
            this.getSupportActionBar().hide();
        }
        catch (NullPointerException e){}

        mControlPanel = ControlPanel.newInstance();
        configFromIntent(getIntent());

        // 设置点击view时显示或隐藏控制按钮
        View rootView = findViewById(android.R.id.content);
        rootView.setOnClickListener(v -> {
            if (mIsControlPanelShowed) {
                hideControlPanel();
            } else {
                showControlPanel();
            }
        });

        PanoApplication app = (PanoApplication)getApplication();
        app.registerEventHandler(this);
        app.registerStreamHandler(this);
        app.initPanoEngine();
        mRtcEngine = app.getPanoEngine();
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
        //mUserViewArray[0].isFree = false;
        //mUserViewArray[0].setUser(mUserId, kDefaultVideoStreamId);
        //mRtcEngine.setLocalVideoRender(mUserViewArray[0].view);
        //mRtcEngine.startPreview(ToVideoProfileType(mLocalProfile), mFrontCamera);

        joinChannel();
    }

    @Override
    public void onBackPressed() {
        leaveChannel();
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
        app.removeStreamHandler(this);
        mRtcEngine = null;
        super.onDestroy();
    }

    // 加入频道
    private boolean joinChannel() {
        RtcChannelConfig config = new RtcChannelConfig();
        config.userName = mUserName;
        config.mode_1v1 = mMode1v1;
        // NOTE: remove kChannelServiceWhiteboard if whiteboard is not used
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
        mRtcEngine.stopPreview();
        mRtcEngine.stopVideo();
        mRtcEngine.stopAudio();
        for (int i=0; i<mVideoStreams.length; i++) {
            if (mVideoStreams[i] > 0) {
                mRtcEngine.getVideoStreamManager().stopVideo(mVideoStreams[i]);
            }
            mVideoStreams[i] = -1;
        }
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
            //mRtcEngine.stopPreview();
            // 启动本地视频流
            RtcVideoStreamManager streamMgr = mRtcEngine.getVideoStreamManager();
            RtcDeviceInfo[] devices = mRtcEngine.getVideoDeviceManager().getCaptureDeviceList();
            for (int i=0; i<Math.min(devices.length, mVideoStreams.length); i++) {
                mVideoStreams[i] = streamMgr.createVideoStream(devices[i].deviceId);
                if (mVideoStreams[i] > 0) {
                    mUserViewArray[i].isFree = false;
                    mUserViewArray[i].setUser(mUserId, mVideoStreams[i]);
                    updateLocalVideoRender(mVideoStreams[i], mUserViewArray[i].view);
                    streamMgr.startVideo(mVideoStreams[i], ToVideoProfileType(mLocalProfile));
                }
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
        stopUserAllVideoStreams(userId);
    }


    // -------------------------- RTC Video Stream Manager Callbacks --------------------------
    @Override
    public void onUserVideoStart(long userId, int streamId, Constants.VideoProfileType maxProfile) {
        doUserVideoStart(userId, streamId, maxProfile);
    }
    @Override
    public void onUserVideoStop(long userId, int streamId) {
        doUserVideoStop(userId, streamId);
    }


    // 订阅用户视频
    private boolean subscribeUserVideo(long userId, int streamId, UserViewInfo viewInfo, Constants.VideoProfileType profile) {
        viewInfo.view.setMirror(false);
        viewInfo.setUser(userId, streamId);
        viewInfo.setVisible(true);
        viewInfo.isFree = false;
        viewInfo.isScreen = false;

        mRtcEngine.getVideoStreamManager().setRemoteVideoRender(userId, streamId, viewInfo.view);
        Constants.QResult ret = mRtcEngine.getVideoStreamManager().subscribeVideo(userId, streamId, profile);
        if(ret == Constants.QResult.OK){
            return true;
        }else {
            String msg = "subscribeUserVideo failed, userId=" + userId + ", result=" + ret;
            Log.w(TAG, msg);
            Toast.makeText(CallActivity.this, msg, Toast.LENGTH_LONG).show();
            return false;
        }
    }

    private void unsubscribeUserVideo(long userId, int streamId) {
        mRtcEngine.getVideoStreamManager().unsubscribeVideo(userId, streamId);
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
        int streamId = mUserViewArray[index].streamId;
        Constants.VideoProfileType maxProfile = mUserViewArray[index].maxProfile;
        boolean isScreen = mUserViewArray[index].isScreen;
        mUserViewArray[index].setUser(mUserViewArray[0].userId, mUserViewArray[0].streamId);
        mUserViewArray[index].maxProfile = mUserViewArray[0].maxProfile;
        mUserViewArray[index].isScreen = mUserViewArray[0].isScreen;
        if (mUserViewArray[index].userId == mUserId) {
            updateLocalVideoRender(mUserViewArray[index].streamId, mUserViewArray[index].view);
        } else {
            if (mUserViewArray[index].isScreen) {
                mRtcEngine.setRemoteScreenRender(mUserViewArray[index].userId, mUserViewArray[index].view);
            } else {
                subscribeUserVideo(mUserViewArray[index].userId, mUserViewArray[index].streamId,
                        mUserViewArray[index], Constants.VideoProfileType.Low);
            }
        }

        mUserViewArray[0].setUser(userId, streamId);
        mUserViewArray[0].maxProfile = maxProfile;
        mUserViewArray[0].isScreen = isScreen;
        if (mUserViewArray[0].userId == mUserId) {
            updateLocalVideoRender(mUserViewArray[0].streamId, mUserViewArray[0].view);
        } else {
            if (mUserViewArray[0].isScreen) {
                mRtcEngine.setRemoteScreenRender(mUserViewArray[0].userId, mUserViewArray[0].view);
            } else {
                Constants.VideoProfileType profile = ToVideoProfileType(Math.min(mRemoteProfile,
                        mUserViewArray[0].maxProfile.getValue()));
                subscribeUserVideo(mUserViewArray[0].userId, mUserViewArray[0].streamId,
                        mUserViewArray[0], profile);
            }
        }
    }

    // 取消订阅用户视频
    private void stopUserVideo(long userId, int streamId) {
        for (int i=0; i < mUserViewCount; i++) {
            if (userId == mUserViewArray[i].userId &&
                    streamId == mUserViewArray[i].streamId &&
                    !mUserViewArray[i].isScreen) {
                stopUserView(userId, streamId, i, false);
                break;
            }
        }
    }

    private void stopUserAllVideoStreams(long userId) {
        for (int i=0; i < mUserViewCount; i++) {
            if (userId == mUserViewArray[i].userId && !mUserViewArray[i].isScreen) {
                stopUserView(userId, mUserViewArray[i].streamId, i, true);
            }
        }
    }

    private void stopUserView(long userId, int streamId, int index, boolean forUser) {
        mUserViewArray[index].isScreen = false; // reset screen flag
        // check if there is unsubscribed video user, and subscribe it if any
        if (mUnsubscribedVideoUsers.size() > 0) {
            VideoUserInfo vui = mUnsubscribedVideoUsers.valueAt(0);
            VideoInfo vi = null;
            if (vui.videoInfos.size() > 0) {
                vi = vui.videoInfos.valueAt(0);
                vui.videoInfos.removeAt(0);
            }
            if (vui.videoInfos.size() == 0) {
                mUnsubscribedVideoUsers.removeAt(0);
            }
            assert vi != null;
            Constants.VideoProfileType profile = Constants.VideoProfileType.Low;
            if (index == 0) {
                profile = ToVideoProfileType(Math.min(mRemoteProfile, vi.maxProfile.getValue()));
            }
            if (subscribeUserVideo(vui.userId, vi.streamId, mUserViewArray[index], profile)) {
                mUserViewArray[index].maxProfile = vi.maxProfile;
                mUserViewArray[index].subProfile = profile;
                return;
            }
        }

        // if large view user left, then try to move other remote user to large view
        if (index == 0) { // is large view
            for (int j = mUserViewCount - 1; j > 0; j--) {
                if (!mUserViewArray[j].isFree && mUserViewArray[j].userId != mUserId &&
                        (!forUser || mUserViewArray[j].userId != userId)) {
                    mUserViewArray[0].setUser(mUserViewArray[j].userId, mUserViewArray[j].streamId);
                    mUserViewArray[0].isFree = false;
                    mUserViewArray[0].maxProfile = mUserViewArray[j].maxProfile;
                    mUserViewArray[0].subProfile = mUserViewArray[j].subProfile;
                    mUserViewArray[j].isFree = true;
                    mUserViewArray[j].setVisible(false);

                    Constants.VideoProfileType profile = ToVideoProfileType(Math.min(mRemoteProfile,
                            mUserViewArray[0].maxProfile.getValue()));
                    // resubscribe the video
                    subscribeUserVideo(mUserViewArray[0].userId, mUserViewArray[0].streamId, mUserViewArray[0], profile);
                    mUserViewArray[0].subProfile = profile;
                    return;
                }
            }
        }

        mUserViewArray[index].isFree = true;
        mUserViewArray[index].setVisible(false);
    }

    // 更新本地用户视频的视图
    private void updateLocalVideoRender(int streamId, RtcView view) {
        mRtcEngine.getVideoStreamManager().setLocalVideoRender(streamId, view);
    }

    void doUserVideoStart(long userId, int streamId, Constants.VideoProfileType maxProfile) {
        int viewIndex = -1;
        // 先检查大图是否空闲，如空闲则将此用户显示到大图
        if (mUserViewArray[0].isFree || mUserViewArray[0].userId == mUserId) {
            // large view is free or used by local user, then make this user to large view
            // 如果大图被本地用户占用，则将本地用户移到小图，如果没有空闲的小图，则不显示本地用户视频
            if (mUserViewArray[0].userId == mUserId) {
                // move local user to last view
                RtcView localView = null;
                if (mUserViewArray[mUserViewCount-1].isFree) {
                    localView = mUserViewArray[mUserViewCount-1].view;
                    mUserViewArray[mUserViewCount-1].setVisible(true);
                    mUserViewArray[mUserViewCount-1].isFree = false;
                    mUserViewArray[mUserViewCount-1].isScreen = false;
                    mUserViewArray[mUserViewCount-1].setUser(mUserId, mUserViewArray[0].streamId);
                }
                updateLocalVideoRender(mUserViewArray[0].streamId, localView);
            }
            mUserViewArray[0].setUser(userId, streamId);
            mUserViewArray[0].isFree = false;
            mUserViewArray[0].isScreen = false;
            mUserViewArray[0].maxProfile = maxProfile;
            viewIndex = 0;
        } else {
            // 如大图不空闲，则尝试找到一个空闲的小图
            // try to find a free small view
            for (int i=1; i < mUserViewCount; i++) {
                if (mUserViewArray[i].isFree) {
                    mUserViewArray[i].setUser(userId, streamId);
                    mUserViewArray[i].isFree = false;
                    mUserViewArray[i].isScreen = false;
                    viewIndex = i;
                    break;
                }
            }
        }

        if (viewIndex != -1) {
            Constants.VideoProfileType profile = Constants.VideoProfileType.Low;
            if (viewIndex == 0) {
                profile = ToVideoProfileType(Math.min(mRemoteProfile, maxProfile.getValue()));
            }
            // 订阅此用户视频到指定视图
            if (subscribeUserVideo(userId, streamId, mUserViewArray[viewIndex], profile)) {
                mUserViewArray[viewIndex].maxProfile = maxProfile;
                mUserViewArray[viewIndex].subProfile = profile;
            } else {
                mUserViewArray[viewIndex].isFree = true;
            }
        } else {
            // 如果没有空闲视图，将此用户加入未订阅视频用户列表，以等待空闲视图
            // no view available
            VideoUserInfo vui = mUnsubscribedVideoUsers.get(userId);
            if (vui == null) {
                vui = new VideoUserInfo();
                vui.userId = userId;
                mUnsubscribedVideoUsers.put(vui.userId, vui);
            }
            VideoInfo vi = new VideoInfo();
            vi.streamId = streamId;
            vi.maxProfile = maxProfile;
            vui.videoInfos.put(vi.streamId, vi);
        }
    }
    void doUserVideoStop(long userId, int streamId) {
        // 取消订阅此用户视频
        mRtcEngine.getVideoStreamManager().unsubscribeVideo(userId, streamId);
        stopUserVideo(userId, streamId);
    }


    protected void configFromIntent(Intent intent) {
        mUserId = intent.getLongExtra(MainActivity.kUserID, 0);
        mChannelId = intent.getStringExtra(MainActivity.kChannelID);
        mAppToken = intent.getStringExtra(MainActivity.kAppToken);
        mUserName = intent.getStringExtra(MainActivity.kUserName);
        mMode1v1 = intent.getBooleanExtra(MainActivity.kMode1v1, true);
    }

    protected void showControlPanel() {
        if (!mIsControlPanelShowed) {
            FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
            if (!mControlPanel.isAdded()) {
                ft.replace(R.id.fl_control_panel, mControlPanel)
                        .addToBackStack(null);
            }
            try {
                ft.setCustomAnimations(R.anim.panel_appear, R.anim.panel_disappear)
                        .show(mControlPanel)
                        .commitAllowingStateLoss();
            } catch (IllegalStateException e) {
                return;
            }
            mIsControlPanelShowed = true;
        }
    }

    protected void hideControlPanel() {
        if (mIsControlPanelShowed) {
            FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
            ft.setCustomAnimations(R.anim.panel_appear, R.anim.panel_disappear)
                    .remove(mControlPanel)
                    .commitAllowingStateLoss();
            mIsControlPanelShowed = false;
        }
    }

    Constants.VideoProfileType ToVideoProfileType(int profile) {
        switch (profile) {
            case 0:
                return Constants.VideoProfileType.Lowest;
            case 1:
                return Constants.VideoProfileType.Low;
            case 2:
                return Constants.VideoProfileType.Standard;
            case 3:
                return Constants.VideoProfileType.HD720P;
            case 4:
                return Constants.VideoProfileType.HD1080P;
            default:
                return Constants.VideoProfileType.HD720P;
        }
    }

    private void setScreenOnFlag() {
        final int keepScreenOnFlag =
                WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD |
                        WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED |
                        WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON |
                        WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON;
        Window w = getWindow();
        w.getAttributes().flags |= keepScreenOnFlag;
        w.addFlags(keepScreenOnFlag);
    }


    // 控制按钮回调处理
    // -------------------------- Control Panel Callbacks --------------------------
    @Override
    public void onMuteAudio()
    {
        mRtcEngine.muteAudio();
    }

    @Override
    public void onUnmuteAudio()
    {
        mRtcEngine.unmuteAudio();
    }

    @Override
    public void onMuteVideo()
    {
        for (int sid : mVideoStreams) {
            if (sid > 0) {
                mRtcEngine.getVideoStreamManager().muteVideo(sid);
            }
        }
    }

    @Override
    public void onUnmuteVideo()
    {
        for (int sid : mVideoStreams) {
            if (sid > 0) {
                mRtcEngine.getVideoStreamManager().unmuteVideo(sid);
            }
        }
    }

    @Override
    public void onEndCall()
    {
        onBackPressed();
    }
}