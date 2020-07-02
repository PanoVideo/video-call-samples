package com.pano.rtc.demo.basicvideocall;

import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.fragment.app.Fragment;


public class ControlPage extends Fragment {
    private OnControlPageListener mListener;
    private TextView mTextPage;
    private int mCurrentPageNo = 1;
    private int mTotalPages = 1;

    public ControlPage() {
        // Required empty public constructor
    }

    static ControlPage newInstance() {
        return new ControlPage();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.control_page, container, false);
        v.getBackground().setAlpha(0);

        v.findViewById(R.id.btn_page_add).setOnClickListener( v1 -> {
            mListener.onPageAdd();
        });
        v.findViewById(R.id.btn_page_remove).setOnClickListener( v1 -> {
            mListener.onPageRemove();
        });
        v.findViewById(R.id.btn_page_prev).setOnClickListener( v1 -> {
            mListener.onPagePrev();
        });
        v.findViewById(R.id.btn_page_next).setOnClickListener( v1 -> {
            mListener.onPageNext();
        });

        mTextPage = v.findViewById(R.id.text_page);
        mTextPage.setText(""+mCurrentPageNo+"/"+mTotalPages);
        mTextPage.setTextColor(Color.BLUE);

        return v;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof OnControlPageListener) {
            mListener = (OnControlPageListener) context;
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

    public void updatePageNumber(int pageNo, int totalPages) {
        mCurrentPageNo = pageNo;
        mTotalPages = totalPages;
        if (mTextPage != null) {
            mTextPage.setText(""+mCurrentPageNo+"/"+mTotalPages);
        }
    }

    public interface OnControlPageListener {
        void onPageAdd();
        void onPageRemove();
        void onPagePrev();
        void onPageNext();
    }
}
