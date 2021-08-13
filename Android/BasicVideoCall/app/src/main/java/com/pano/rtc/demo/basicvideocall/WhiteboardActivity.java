package com.pano.rtc.demo.basicvideocall;

import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.EditText;

import java.util.List;

import com.pano.rtc.api.Constants;
import com.pano.rtc.api.PanoCoursePageView;
import com.pano.rtc.api.PanoWebView;
import com.pano.rtc.api.RtcWbView;
import com.pano.rtc.api.RtcWhiteboard;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.fragment.app.FragmentTransaction;

import java.nio.charset.StandardCharsets;

public class WhiteboardActivity extends AppCompatActivity implements PanoWhiteboardHandler,
        WhiteboardToolbox.OnControlToolboxListener, ControlPage.OnControlPageListener {
    private static final String TAG = "PanoCall";
    private RtcWhiteboard mWhiteboard;
    private long mLocalUserId = 0;
    private boolean mIsToolboxShowed = false;
    private WhiteboardToolbox mToolbox;
    private boolean mIsControlPageShowed = false;
    private ControlPage mControlPage;
    private ConstraintLayout clDummy;


    public static void launch(Context context, long localUserId) {
        //final Bundle bundle = new Bundle();
        //bundle.putBinder("whiteboard", new WhiteboardBinder(wb));
        Intent intent = new Intent();
        //intent.putExtras(bundle);
        intent.putExtra("local_user_id", localUserId);
        intent.setClass(context, WhiteboardActivity.class);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_whiteboard);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setDisplayShowHomeEnabled(true);
        toolbar.setNavigationOnClickListener(v -> {
            onBackPressed();
        });
        setTitle("Whiteboard");

        configFromIntent(getIntent());

        mToolbox = WhiteboardToolbox.newInstance();
        mControlPage = ControlPage.newInstance();

        PanoApplication app = (PanoApplication)getApplication();
        app.registerWhiteboardHandler(this);
        mWhiteboard = app.getPanoEngine().getWhiteboard();

        clDummy = findViewById(R.id.cl_wb_dummy);
        PanoCoursePageView panoCoursePageView = findViewById(R.id.wbSurfaceView);
        RtcWbView wbView = panoCoursePageView.getAttachRtcWbView();
        mWhiteboard.open(wbView);

        mWhiteboard.setLineWidth(10);
        mWhiteboard.setForegroundColor(0.5f, 0.15f,0.33f, 1);
        mWhiteboard.setBackgroundColor(0.75f, 0.75f,0.75f, 1);
        mWhiteboard.setFontSize(36);

        mWhiteboard.setRoleType(Constants.WBRoleType.Admin);
        mWhiteboard.setToolType(Constants.WBToolType.Select);

        mControlPage.updatePageNumber(mWhiteboard.getCurrentPageNumber(), mWhiteboard.getTotalNumberOfPages());

        mWhiteboard.sendMessage(mLocalUserId, "Hello PANO! 你好，拍乐云！".getBytes());
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_whiteboard, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.action_toolbox) {
            if (mIsToolboxShowed) {
                hideToolbox();
            } else {
                hideControlPage();
                showToolbax();
            }
            return true;
        } else if (id == R.id.action_page) {
            if (mIsControlPageShowed) {
                hideControlPage();
            } else {
                hideToolbox();
                showControlPage();
            }
            return true;
        } else if (id == R.id.action_background) {
            changeBackground();
        } else if (id == R.id.action_fullscreen) {
            int v = clDummy.getVisibility();
            clDummy.setVisibility(v == View.GONE ? View.VISIBLE : View.GONE);
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onDestroy()
    {
        PanoApplication app = (PanoApplication)getApplication();
        app.removeWhiteboardHandler(this);
        mWhiteboard = null;
        super.onDestroy();
    }


    protected void configFromIntent(Intent intent) {
        mLocalUserId = intent.getLongExtra("local_user_id", 0);
    }

    @Override
    public void onBackPressed() {
        mWhiteboard.close();

        int count = getSupportFragmentManager().getBackStackEntryCount();
        for (int i=0; i<count; i++) {
            getSupportFragmentManager().popBackStack();
        }
        super.onBackPressed();
    }

    void showToolbax() {
        if (!mIsToolboxShowed) {
            FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
            if (!mToolbox.isAdded()) {
                ft.replace(R.id.fl_control_toolbox, mToolbox)
                        .addToBackStack(null);
            }
            try {
                ft.setCustomAnimations(R.anim.panel_appear, R.anim.panel_disappear)
                        .show(mToolbox)
                        .commitAllowingStateLoss();
            } catch (IllegalStateException e) {
                return;
            }
            mIsToolboxShowed = true;
        }
    }

    void hideToolbox() {
        if (mIsToolboxShowed) {
            FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
            ft.setCustomAnimations(R.anim.panel_appear, R.anim.panel_disappear)
                    .remove(mToolbox)
                    .commitAllowingStateLoss();
            mIsToolboxShowed = false;
        }
    }

    void showControlPage() {
        if (!mIsControlPageShowed) {
            FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
            if (!mControlPage.isAdded()) {
                ft.replace(R.id.fl_control_page, mControlPage)
                        .addToBackStack(null);
            }
            try {
                ft.setCustomAnimations(R.anim.panel_appear, R.anim.panel_disappear)
                        .show(mControlPage)
                        .commitAllowingStateLoss();
            } catch (IllegalStateException e) {
                return;
            }
            mIsControlPageShowed = true;
        }
    }

    void hideControlPage() {
        if (mIsControlPageShowed) {
            FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
            ft.setCustomAnimations(R.anim.panel_appear, R.anim.panel_disappear)
                    .remove(mControlPage)
                    .commitAllowingStateLoss();
            mIsControlPageShowed = false;
        }
    }

    void changeBackground() {
        Dialog dialog = new Dialog(this, R.style.CustomDialogStyle);

        dialog.requestWindowFeature(Window.FEATURE_LEFT_ICON);
        //dialog.setFeatureDrawableResource(Window.FEATURE_LEFT_ICON, R.drawable.your_icon);
        dialog.setContentView(R.layout.dlg_wb_background);

        EditText editImageUrl = dialog.findViewById(R.id.edit_wb_image_url);
        dialog.findViewById(R.id.btn_wb_bkgnd_ok).setOnClickListener(v -> {
            dialog.dismiss();
            String imageUrl = editImageUrl.getText().toString();
            if (!imageUrl.isEmpty()) {
                mWhiteboard.setBackgroundImage(imageUrl);
            }
        });
        dialog.findViewById(R.id.btn_wb_bkgnd_cancel).setOnClickListener(v -> dialog.dismiss());

        dialog.show();
    }


    ////////////////////////////////////////////////////////////////////
    // Callbacks
    @Override
    public void onPageNumberChanged(int curPage, int totalPages) {
        Log.i(TAG, "+++++ onPageNumberChanged: cur="+curPage+", total="+totalPages);
        runOnUiThread(()-> {
            if (mControlPage != null) {
                mControlPage.updatePageNumber(curPage, totalPages);
            }
        });
    }
    @Override
    public void onImageStateChanged(String url, Constants.WBImageState state) {
        Log.i(TAG, "+++++ onImageStateChanged: url="+url+", state="+state);
    }
    @Override
    public void onViewScaleChanged(float scale) {
        Log.i(TAG, "+++++ onViewScaleChanged: scale="+scale);
    }
    @Override
    public void onRoleTypeChanged(Constants.WBRoleType newRole) {
        Log.i(TAG, "+++++ onRoleTypeChanged: newRole="+newRole);
    }

    @Override
    public void onContentUpdated() {
        //Log.i(TAG, "+++++ onContentUpdated");
    }

    @Override
    public void onSnapshotComplete(Constants.QResult result, String filename) {
        Log.i(TAG, "+++++ onSnapshotComplete="+filename);
    }

    @Override
    public void onMessage(long userId, byte[] bytes) {
        String msg = new String(bytes, StandardCharsets.UTF_8);
        Log.i(TAG, "+++++ onMessage: userId="+userId+", msg="+msg);
    }

    @Override
    public void onAddBackgroundImages(Constants.QResult result, String fileId) {
        Log.i(TAG, "onAddBackgroundImages " + result + ", " + fileId);
    }

    @Override
    public void onDocTranscodeStatus(Constants.QResult result, String fileId, int progress, int totalPages) {
        Log.i(TAG, "onDocTranscodeStatus " + result + ", " + fileId);
    }

    @Override
    public void onCreateDoc(Constants.QResult result, String fileId) {
        Log.i(TAG, "onCreateDoc " + result + ", " + fileId);
    }

    @Override
    public void onDeleteDoc(Constants.QResult result, String fileId) {
        Log.i(TAG, "onDeleteDoc " + result + ", " + fileId);
    }

    @Override
    public void onSwitchDoc(Constants.QResult result, String fileId) {
        Log.i(TAG, "onSwitchDoc " + result + ", " + fileId);
        /*List<String> fileList = mWhiteboard.enumerateFiles();
        int count = fileList.size();
        for (int i = 0; i < count; i++) {
            Log.i(TAG, "onSwitchDoc enum " + i + ": " + fileList.get(i) );
        }
        String curFile = mWhiteboard.getCurrentFileId();
        Log.i(TAG, "onSwitchDoc current: " + curFile);*/
    }


    ////////////////////////////////////////////////////////////////////////
    @Override
    public void onToolTypeChanged(Constants.WBToolType type) {
        mWhiteboard.setToolType(type);
    }
    @Override
    public void onToolTypeUndo() {
        mWhiteboard.undo();
    }
    @Override
    public void onToolTypeRedo() {
        mWhiteboard.redo();
    }
    @Override
    public void onToolTypeSnapshot() {
        String dir = this.getBaseContext().getExternalFilesDir(android.os.Environment.DIRECTORY_PICTURES).getPath();
        if (!dir.isEmpty()) {
            mWhiteboard.snapshot(Constants.WBSnapshotMode.All, dir);
        }
    }
    ////////////////////////////////////////////////////////////////////////
    //
    @Override
    public void onPageAdd() {
        mWhiteboard.addPage(false);
    }
    @Override
    public void onPageRemove() {
        mWhiteboard.removePage(mWhiteboard.getCurrentPageNumber());
    }
    @Override
    public void onPagePrev() {
        mWhiteboard.prevPage();
    }
    @Override
    public void onPageNext() {
        mWhiteboard.nextPage();
    }
}
