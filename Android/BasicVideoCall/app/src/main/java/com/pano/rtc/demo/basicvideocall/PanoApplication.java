package com.pano.rtc.demo.basicvideocall;

import android.app.Application;

import com.pano.rtc.api.Constants;
import com.pano.rtc.api.RtcEngine;
import com.pano.rtc.api.RtcEngineConfig;


public class PanoApplication extends Application {
    public static final String APPID = %%填入应用的 APPID%%;
    public static final String APP_TOKEN = %%填入从 PANO 获取的临时 APP Token%%;
    public static final String PANO_SERVER = "api.pano.video";

    private RtcEngine mRtcEngine;
    private PanoEngineCallback mRtcCallback = new PanoEngineCallback();

    protected Constants.AudioAecType mAudioAecType = Constants.AudioAecType.Default;
    protected boolean mHwAcceleration = false;


    @Override
    public void onCreate() {
        super.onCreate();


        // 设置PANO媒体引擎的配置参数
        RtcEngineConfig engineConfig = new RtcEngineConfig();
        engineConfig.appId = APPID;
        engineConfig.server = PANO_SERVER;
        engineConfig.context = getApplicationContext();
        engineConfig.callback = mRtcCallback;
        engineConfig.audioAecType = mAudioAecType;
        engineConfig.videoCodecHwAcceleration = mHwAcceleration;
        try {
            mRtcEngine = RtcEngine.create(engineConfig);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public RtcEngine getPanoEngine() { return mRtcEngine; }
    public PanoEngineCallback getPanoCallback() { return mRtcCallback; }

    public void registerEventHandler(PanoEventHandler handler) { mRtcCallback.addHandler(handler); }
    public void removeEventHandler(PanoEventHandler handler) { mRtcCallback.removeHandler(handler); }

    @Override
    public void onTerminate() {
        super.onTerminate();
        RtcEngine.destroy();
    }
}
