package video.pano.demo.multivideostream;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.fragment.app.Fragment;


public class ControlPanel extends Fragment {

    private OnControlPanelListener mListener;

    private ImageView mAudioView;
    private boolean mAudioMuted = false;
    private ImageView mVideoView;
    private boolean mVideoMuted = false;

    public ControlPanel() {
        // Required empty public constructor
    }

    static ControlPanel newInstance() {
        return new ControlPanel();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.control_panel, container, false);
        v.getBackground().setAlpha(0);

        mAudioView = v.findViewById(R.id.btn_audio);
        mAudioView.setOnClickListener( v1 -> {
            mAudioMuted = !mAudioMuted;
            if (mAudioMuted) {
                mAudioView.setImageResource(R.drawable.btn_audio_mute);
                mListener.onMuteAudio();
            } else {
                mAudioView.setImageResource(R.drawable.btn_audio_unmute);
                mListener.onUnmuteAudio();
            }
        });
        if (mAudioMuted) {
            mAudioView.setImageResource(R.drawable.btn_audio_mute);
        } else {
            mAudioView.setImageResource(R.drawable.btn_audio_unmute);
        }
        mVideoView = v.findViewById(R.id.btn_video);
        mVideoView.setOnClickListener( v1 -> {
            mVideoMuted = !mVideoMuted;
            if (mVideoMuted) {
                mVideoView.setImageResource(R.drawable.btn_video_mute);
                mListener.onMuteVideo();
            } else {
                mVideoView.setImageResource(R.drawable.btn_video_unmute);
                mListener.onUnmuteVideo();
            }
        });
        v.findViewById(R.id.btn_end_call).setOnClickListener( v1 -> {
            mListener.onEndCall();
        });

        return v;
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof OnControlPanelListener) {
            mListener = (OnControlPanelListener) context;
        } else {
            //throw new RuntimeException(context.toString()
            //        + " must implement OnFragmentInteractionListener");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();
        mListener = null;
    }

    public interface OnControlPanelListener {
        void onMuteAudio();
        void onUnmuteAudio();
        void onMuteVideo();
        void onUnmuteVideo();
        void onEndCall();
    }
}
