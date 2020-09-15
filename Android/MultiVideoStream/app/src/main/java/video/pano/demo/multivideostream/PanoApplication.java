package video.pano.demo.multivideostream;

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
    private PanoStreamCallback mStreamCallback = new PanoStreamCallback();

    protected Constants.AudioAecType mAudioAecType = Constants.AudioAecType.Default;
    protected boolean mHwAcceleration = false;


    @Override
    public void onCreate() {
        super.onCreate();

        initPanoEngine();
    }

    public RtcEngine getPanoEngine() {
        if (mRtcEngine == null) {
            initPanoEngine();
        }
        return mRtcEngine;
    }
    public PanoEngineCallback getPanoCallback() { return mRtcCallback; }

    public void registerEventHandler(PanoEventHandler handler) { mRtcCallback.addHandler(handler); }
    public void removeEventHandler(PanoEventHandler handler) { mRtcCallback.removeHandler(handler); }
    public void registerStreamHandler(PanoStreamHandler handler) { mStreamCallback.addHandler(handler); }
    public void removeStreamHandler(PanoStreamHandler handler) { mStreamCallback.removeHandler(handler); }

    public boolean initPanoEngine() {
        Constants.AudioAecType audioAecType = Constants.AudioAecType.Default;
        boolean hwAcceleration = false;

        if (mRtcEngine == null || audioAecType != mAudioAecType || hwAcceleration != mHwAcceleration) {
            mAudioAecType = audioAecType;
            mHwAcceleration = hwAcceleration;

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
                assert mRtcEngine != null;
                mRtcEngine.getVideoStreamManager().setCallback(mStreamCallback);
            } catch (Exception e) {
                e.printStackTrace();
                return false;
            }
        }
        return true;
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        RtcEngine.destroy();
    }
}
