package com.pano.rtc.demo.basicvideocall;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.fragment.app.Fragment;

import com.pano.rtc.api.model.stats.RtcAudioRecvStats;
import com.pano.rtc.api.model.stats.RtcAudioSendStats;
import com.pano.rtc.api.model.stats.RtcSystemStats;
import com.pano.rtc.api.model.stats.RtcVideoBweStats;
import com.pano.rtc.api.model.stats.RtcVideoRecvStats;
import com.pano.rtc.api.model.stats.RtcVideoSendStats;

import java.util.Locale;


public class MediaStats extends Fragment {

    private TextView mVtxStats;
    private TextView mVrxStats;
    private TextView mAtxStats;
    private TextView mArxStats;
    private RtcVideoBweStats mBweStats;

    public MediaStats() {

    }

    public static MediaStats newInstance() {
        MediaStats fragment = new MediaStats();
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.media_stats, container, false);
        v.getBackground().setAlpha(20);

        mVtxStats = v.findViewById(R.id.tv_vtx_stats);
        mVrxStats = v.findViewById(R.id.tv_vrx_stats);
        mAtxStats = v.findViewById(R.id.tv_atx_stats);
        mArxStats = v.findViewById(R.id.tv_arx_stats);

        return v;
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
    }

    @Override
    public void onDetach() {
        super.onDetach();
    }

    void onVideoSendStats(RtcVideoSendStats stats) {
        if (mVtxStats != null) {
            String str = statsToString(stats);
            mVtxStats.setText(str);
        }
    }

    void onVideoRecvStats(RtcVideoRecvStats stats) {
        if (mVrxStats != null) {
            String str = statsToString(stats);
            mVrxStats.setText(str);
        }
    }

    void onAudioSendStats(RtcAudioSendStats stats) {
        if (mAtxStats != null) {
            String str = statsToString(stats);
            mAtxStats.setText(str);
        }
    }

    void onAudioRecvStats(RtcAudioRecvStats stats) {
        if (mArxStats != null) {
            String str = statsToString(stats);
            mArxStats.setText(str);
        }
    }

    void onVideoBweStats(RtcVideoBweStats stats) {
        mBweStats = stats;
    }

    void onSystemStats(RtcSystemStats stats) {

    }

    private String statsToString(RtcVideoSendStats stats) {
        String str = String.format(Locale.CHINA,
                "===== video tx =====\nbytes:\t\t\t%d\nbr:\t\t\t\t\t%d"
                        + "\nlosscnt:\t%d\nloss:\t\t\t\t%.3f"
                        + "\nrtt:\t\t\t\t\t%d\nw*h:\t\t\t\t%dx%d\nfps:\t\t\t\t%d"
                        + "\npli:\t\t\t\t\t%d",
                stats.bytesSent, stats.bitrate, stats.packetsLost,
                stats.lossRatio,stats.rtt, stats.width, stats.height,
                stats.framerate, stats.plisReceived);

        if (mBweStats != null) {
            String bwe = String.format(Locale.CHINA,
                    "\nbw:\t\t\t\t\t%d\nencbr:\t\t\t%d"
                            + "\ntxbr:\t\t\t\t%d\nrtxbr:\t\t\t%d",
                    mBweStats.bandwidth, mBweStats.encodeBitrate,
                    mBweStats.transmitBitrate, mBweStats.retransmitBitrate);
            str += bwe;
        }

        return str;
    }

    private String statsToString(RtcVideoRecvStats stats) {
        return String.format(Locale.CHINA,
                "===== video rx =====\nbytes:\t\t\t%d\nbr:\t\t\t\t\t%d"
                        + "\nlosscnt:\t%d\nloss:\t\t\t\t%.3f"
                        + "\nw*h:\t\t\t\t%dx%d\nfps:\t\t\t\t%d\npli:\t\t\t\t\t%d",
                stats.bytesReceived, stats.bitrate, stats.packetsLost,
                stats.lossRatio,stats.width, stats.height,
                stats.framerate, stats.plisSent);
    }

    private String statsToString(RtcAudioSendStats stats) {
        return String.format(Locale.CHINA,
                "===== audio tx =====\nbytes:\t\t\t%d\nbr:\t\t\t\t\t%d"
                + "\nlosscnt:\t%d\nloss:\t\t\t\t%.3f"
                + "\nrtt:\t\t\t\t\t%d\nlevel:\t\t\t%d",
                stats.bytesSent, stats.bitrate, stats.packetsLost,
                stats.lossRatio,stats.rtt, stats.inputLevel);
    }

    private String statsToString(RtcAudioRecvStats stats) {
        return String.format(Locale.CHINA,
                "===== audio rx =====\nbytes:\t\t\t%d\nbr:\t\t\t\t\t%d"
                        + "\nlosscnt:\t%d\nloss:\t\t\t\t%.3f"
                        + "\nlevel:\t\t\t%d",
                stats.bytesReceived, stats.bitrate, stats.packetsLost,
                stats.lossRatio,stats.outputLevel);
    }
}
