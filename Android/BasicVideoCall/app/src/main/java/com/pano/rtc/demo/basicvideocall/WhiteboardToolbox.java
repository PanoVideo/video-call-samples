package com.pano.rtc.demo.basicvideocall;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import androidx.fragment.app.Fragment;

import com.pano.rtc.api.Constants;


public class WhiteboardToolbox extends Fragment {
    private OnControlToolboxListener mListener;
    private Constants.WBToolType mToolType = Constants.WBToolType.Select;

    private ImageView mSelectView;
    private ImageView mPathView;
    private ImageView mLineView;
    private ImageView mRectView;
    private ImageView mEllipseView;
    private ImageView mTextView;
    private ImageView mEraserView;

    public WhiteboardToolbox() {
        // Required empty public constructor
    }

    static WhiteboardToolbox newInstance() {
        return new WhiteboardToolbox();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.control_toolbox, container, false);
        v.getBackground().setAlpha(0);

        mSelectView = v.findViewById(R.id.btn_tb_select);
        mSelectView.setOnClickListener( v1 -> {
            if (mToolType != Constants.WBToolType.Select) {
                unselectToolbox(mToolType);
                mToolType = Constants.WBToolType.Select;
            }
            mSelectView.setImageResource(R.drawable.btn_tb_select_1);
            mListener.onToolTypeChanged(mToolType);
        });

        mPathView = v.findViewById(R.id.btn_tb_path);
        mPathView.setOnClickListener( v1 -> {
            if (mToolType != Constants.WBToolType.Path) {
                unselectToolbox(mToolType);
                mToolType = Constants.WBToolType.Path;
            }
            mPathView.setImageResource(R.drawable.btn_tb_path_1);
            mListener.onToolTypeChanged(mToolType);
        });

        mLineView = v.findViewById(R.id.btn_tb_line);
        mLineView.setOnClickListener( v1 -> {
            if (mToolType != Constants.WBToolType.Line) {
                unselectToolbox(mToolType);
                mToolType = Constants.WBToolType.Line;
            }
            mLineView.setImageResource(R.drawable.btn_tb_line_1);
            mListener.onToolTypeChanged(mToolType);
        });

        mRectView = v.findViewById(R.id.btn_tb_rect);
        mRectView.setOnClickListener( v1 -> {
            if (mToolType != Constants.WBToolType.Rect) {
                unselectToolbox(mToolType);
                mToolType = Constants.WBToolType.Rect;
            }
            mRectView.setImageResource(R.drawable.btn_tb_rect_1);
            mListener.onToolTypeChanged(mToolType);
        });

        mEllipseView = v.findViewById(R.id.btn_tb_ellipse);
        mEllipseView.setOnClickListener( v1 -> {
            if (mToolType != Constants.WBToolType.Ellipse) {
                unselectToolbox(mToolType);
                mToolType = Constants.WBToolType.Ellipse;
            }
            mEllipseView.setImageResource(R.drawable.btn_tb_ellipse_1);
            mListener.onToolTypeChanged(mToolType);
        });

        mTextView = v.findViewById(R.id.btn_tb_text);
        mTextView.setOnClickListener( v1 -> {
            if (mToolType != Constants.WBToolType.Text) {
                unselectToolbox(mToolType);
                mToolType = Constants.WBToolType.Text;
            }
            mTextView.setImageResource(R.drawable.btn_tb_text_1);
            mListener.onToolTypeChanged(mToolType);
        });

        mEraserView = v.findViewById(R.id.btn_tb_eraser);
        mEraserView.setOnClickListener( v1 -> {
            if (mToolType != Constants.WBToolType.Eraser) {
                unselectToolbox(mToolType);
                mToolType = Constants.WBToolType.Eraser;
            }
            mEraserView.setImageResource(R.drawable.btn_tb_eraser_1);
            mListener.onToolTypeChanged(mToolType);
        });

        v.findViewById(R.id.btn_tb_undo).setOnClickListener( v1 -> {
            mListener.onToolTypeUndo();
        });

        v.findViewById(R.id.btn_tb_redo).setOnClickListener( v1 -> {
            mListener.onToolTypeRedo();
        });

        v.findViewById(R.id.btn_tb_snapshot).setOnClickListener( v1 -> {
            mListener.onToolTypeSnapshot();
        });

        selectToolbox(mToolType);

        return v;
    }

    private void selectToolbox(Constants.WBToolType toolType) {
        switch (toolType) {
            case Select:
                mSelectView.setImageResource(R.drawable.btn_tb_select_1);
                break;
            case Path:
                mPathView.setImageResource(R.drawable.btn_tb_path_1);
                break;
            case Line:
                mLineView.setImageResource(R.drawable.btn_tb_line_1);
                break;
            case Rect:
                mRectView.setImageResource(R.drawable.btn_tb_rect_1);
                break;
            case Ellipse:
                mEllipseView.setImageResource(R.drawable.btn_tb_ellipse_1);
                break;
            case Text:
                mTextView.setImageResource(R.drawable.btn_tb_text_1);
                break;
            case Eraser:
                mEraserView.setImageResource(R.drawable.btn_tb_eraser_1);
                break;
            default:
                break;
        }
    }

    private void unselectToolbox(Constants.WBToolType toolType) {
        switch (toolType) {
            case Select:
                mSelectView.setImageResource(R.drawable.btn_tb_select);
                break;
            case Path:
                mPathView.setImageResource(R.drawable.btn_tb_path);
                break;
            case Line:
                mLineView.setImageResource(R.drawable.btn_tb_line);
                break;
            case Rect:
                mRectView.setImageResource(R.drawable.btn_tb_rect);
                break;
            case Ellipse:
                mEllipseView.setImageResource(R.drawable.btn_tb_ellipse);
                break;
            case Text:
                mTextView.setImageResource(R.drawable.btn_tb_text);
                break;
            case Eraser:
                mEraserView.setImageResource(R.drawable.btn_tb_eraser);
                break;
            default:
                break;
        }
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof OnControlToolboxListener) {
            mListener = (OnControlToolboxListener) context;
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

    public interface OnControlToolboxListener {
        void onToolTypeChanged(Constants.WBToolType type);
        void onToolTypeUndo();
        void onToolTypeRedo();
        void onToolTypeSnapshot();
    }
}
