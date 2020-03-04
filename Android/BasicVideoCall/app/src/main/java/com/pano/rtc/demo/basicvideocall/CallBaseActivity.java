package com.pano.rtc.demo.basicvideocall;


import android.app.Dialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;

import androidx.fragment.app.FragmentTransaction;
import androidx.preference.PreferenceManager;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.pano.rtc.api.Constants;
import com.pano.rtc.api.IVideoRender;
import com.pano.rtc.api.RtcEngine;
import com.pano.rtc.api.RtcView;
import com.pano.rtc.impl.PLogger;

import static com.pano.rtc.api.IVideoRender.ScalingType.SCALE_ASPECT_BALANCED;
import static com.pano.rtc.api.IVideoRender.ScalingType.SCALE_ASPECT_FILL;
import static com.pano.rtc.api.IVideoRender.ScalingType.SCALE_ASPECT_FIT;
import static com.pano.rtc.api.IVideoRender.ScalingType.SCALE_ASPECT_ADJUST;

public abstract class CallBaseActivity extends AppCompatActivity implements ControlPanel.OnControlPanelListener {
    public static String kPanoServer = "api.pano.video";
    protected IVideoRender.ScalingType scalingType = SCALE_ASPECT_FILL;
    protected int mLocalProfile = 3; // HD720P
    protected int mRemoteProfile = 5; // Max
    protected boolean mHwAcceleration = true;
    protected boolean mLoudspeaker = false;

    protected String mAppToken = "";
    protected String mChannelId = "";
    protected long mUserId = 0;
    protected String mUserName = "";
    protected boolean mVideoMode = true;
    protected boolean mMode1v1 = false;
    protected Constants.AudioAecType mAudioAecType = Constants.AudioAecType.Default;

    protected RtcEngine mRtcEngine = null;
    protected boolean mIsChannelJoined = false;
    protected boolean mFrontCamera = true;
    protected RtcView mLocalView;

    private ControlPanel mControlPanel;
    protected boolean mIsControlPanelShowed = false;
    protected MediaStats mMediaStats;
    protected boolean mIsMediaStatsShowed = false;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setScreenOnFlag();
        loadSettings();
        configFromIntent(getIntent());

        mControlPanel = ControlPanel.newInstance();
        mMediaStats = MediaStats.newInstance();
    }

    private void loadSettings()
    {
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        int aecType = Integer.parseInt(preferences.getString(MainActivity.kAudioAEC, "1"));
        switch (aecType) {
            case 0:
                mAudioAecType = Constants.AudioAecType.Off;
                break;
            case 2:
                mAudioAecType = Constants.AudioAecType.Software;
                break;
            default:
                mAudioAecType = Constants.AudioAecType.Default;
                break;
        }
        mHwAcceleration = preferences.getBoolean(MainActivity.kVideoHWAcceleration, false);
        mLoudspeaker = preferences.getBoolean(MainActivity.kAudioLoudspeaker, true);
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

    protected void showMediaStats() {
        if (!mIsMediaStatsShowed) {
            FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
            if (!mMediaStats.isAdded()) {
                ft.replace(R.id.fl_media_stats, mMediaStats)
                        .addToBackStack(null);
            }
            try {
                ft.setCustomAnimations(R.anim.panel_appear, R.anim.panel_disappear)
                        .show(mMediaStats)
                        .commitAllowingStateLoss();
            } catch (IllegalStateException e) {
                return;
            }
            mIsMediaStatsShowed = true;
        }
    }

    protected void hideMediaStats() {
        if (mIsMediaStatsShowed) {
            FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
            ft.setCustomAnimations(R.anim.panel_appear, R.anim.panel_disappear)
                    .remove(mMediaStats)
                    .commitAllowingStateLoss();
            mIsMediaStatsShowed = false;
        }
    }

    public void onDlgSettings()
    {
        Dialog dialog = new Dialog(this, R.style.CustomDialogStyle);

        dialog.requestWindowFeature(Window.FEATURE_LEFT_ICON);
        //dialog.setFeatureDrawableResource(Window.FEATURE_LEFT_ICON, R.drawable.your_icon);
        dialog.setContentView(R.layout.dlg_settings);

        Switch swShowStats = dialog.findViewById(R.id.switch_show_stats);
        swShowStats.setChecked(mIsMediaStatsShowed);
        swShowStats.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean showed = swShowStats.isChecked();
                if (mIsMediaStatsShowed != showed) {
                    if (showed) {
                        showMediaStats();
                    } else {
                        hideMediaStats();
                    }
                }
            }
        });

        Switch swLoudSpeaker = dialog.findViewById(R.id.switch_loud_speaker);
        swLoudSpeaker.setChecked(mLoudspeaker);
        swLoudSpeaker.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean enabled = swLoudSpeaker.isChecked();
                if (mLoudspeaker != enabled) {
                    mLoudspeaker = enabled;
                    SharedPreferences preferences = PreferenceManager.
                            getDefaultSharedPreferences(CallBaseActivity.this);
                    SharedPreferences.Editor editor = preferences.edit();
                    editor.putBoolean(MainActivity.kAudioLoudspeaker, mLoudspeaker);
                    editor.apply();
                    if (mRtcEngine != null) {
                        mRtcEngine.setLoudspeakerStatus(mLoudspeaker);
                    }
                }
            }
        });

        Button btnOK = (Button) dialog.findViewById(R.id.btn_settings_ok);
        btnOK.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialog.dismiss();
            }
        });

        dialog.show();
    }

    protected void configFromIntent(Intent intent) {
        mUserId = intent.getLongExtra(MainActivity.kUserID, 0);
        mChannelId = intent.getStringExtra(MainActivity.kChannelID);
        mAppToken = intent.getStringExtra(MainActivity.kAppToken);
        mUserName = intent.getStringExtra(MainActivity.kUserName);
        mMode1v1 = intent.getBooleanExtra(MainActivity.kMode1v1, true);
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
    public void onSwitchCamera()
    {
        mRtcEngine.switchCamera();
        mFrontCamera = !mFrontCamera;
        if (mLocalView != null) {
            mLocalView.setMirror(mFrontCamera);
        }
    }

    @Override
    public void onWhiteboard() {

    }

    @Override
    public void onSettings()
    {
        onDlgSettings();
    }

    @Override
    public void onEndCall()
    {
        onBackPressed();
    }
}
