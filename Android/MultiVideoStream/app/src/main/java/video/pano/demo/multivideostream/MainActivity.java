package video.pano.demo.multivideostream;

import android.app.AlertDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.SwitchCompat;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;

import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.Toast;

import com.pano.rtc.api.RtcEngine;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class MainActivity extends AppCompatActivity {
    public static final String kAppToken = "app_token";
    public static final String kChannelID = "channel_id";
    public static final String kUserID = "user_id";
    public static final String kUserName = "user_name";
    public static final String kMode1v1 = "mode1v1";
    public static final String kAudioLoudspeaker = "audio_loudspeaker";
    private static final int PERMISSION_REQUEST_CODE = 10;

    private EditText mChannelId;
    private EditText mUserId;
    private EditText mUserName;

    private SwitchCompat mMode1v1;
    private SwitchCompat mLoudspeaker;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        String channelId = preferences.getString(kChannelID, "");
        mChannelId = findViewById(R.id.editChannelID);
        mChannelId.setText(channelId);
        mChannelId.setRawInputType(Configuration.KEYBOARD_QWERTY);
        mUserId = findViewById(R.id.editUserID);
        long userId = (long)10000 + new Random().nextInt(1000) ;
        userId = preferences.getLong(kUserID, userId);
        mUserId.setText( Long.toString(userId) );
        String userName = preferences.getString(kUserName, "");
        mUserName = findViewById(R.id.editUserName);
        mUserName.setText(userName);
        findViewById(R.id.buttonJoinChannel).setOnClickListener(v -> joinChannel());

        mMode1v1 = findViewById(R.id.switchMode1v1);
        mMode1v1.setChecked(false);

        boolean enabled = preferences.getBoolean(kAudioLoudspeaker, true);
        mLoudspeaker = findViewById(R.id.switchLoudspeaker);
        mLoudspeaker.setOnClickListener(v -> {
            SharedPreferences.Editor editor = preferences.edit();
            editor.putBoolean(kAudioLoudspeaker, mLoudspeaker.isChecked());
            editor.apply();
        });
        mLoudspeaker.setChecked(enabled);

        mChannelId.requestFocus();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onResume() {
        super.onResume();
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        boolean enabled = preferences.getBoolean(kAudioLoudspeaker, true);
        mLoudspeaker.setChecked(enabled);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode) {
            case PERMISSION_REQUEST_CODE:
                if (RtcEngine.checkPermission(this).size() == 0) {
                    startCall(PanoApplication.APP_TOKEN, mChannelId.getText().toString(),
                            mUserId.getText().toString(),
                            mUserName.getText().toString());
                } else {
                    Toast.makeText(MainActivity.this, "Some permissions are denied", Toast.LENGTH_LONG).show();
                }
                break;
            default:
                super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    private void joinChannel() {
        String userId = mUserId.getText().toString();
        String channelId = mChannelId.getText().toString();
        String userName = mUserName.getText().toString();
        if (TextUtils.isEmpty(userId)) {
            mUserId.setError("Required");
            return;
        }
        if (TextUtils.isEmpty(channelId)) {
            mChannelId.setError("Required");
            return;
        }

        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        SharedPreferences.Editor editor = preferences.edit();
        editor.putString(kChannelID, channelId);
        editor.putLong(kUserID, Long.parseLong(userId));
        editor.putString(kUserName, userName);
        editor.apply();

        final List<String> missed = RtcEngine.checkPermission(this);
        if (missed.size() != 0) {

            List<String> showRationale = new ArrayList<>();
            for (String permission : missed) {
                if (ActivityCompat.shouldShowRequestPermissionRationale(this, permission)) {
                    showRationale.add(permission);
                }
            }

            if (showRationale.size() > 0) {
                new AlertDialog.Builder(MainActivity.this)
                        .setMessage("Please allow the permissions")
                        .setPositiveButton("OK", (dialog, which) ->
                                ActivityCompat.requestPermissions(MainActivity.this,
                                        missed.toArray(new String[0]),
                                        PERMISSION_REQUEST_CODE))
                        .setNegativeButton("Cancel", null)
                        .create()
                        .show();
            } else {
                ActivityCompat.requestPermissions(this, missed.toArray(new String[0]), PERMISSION_REQUEST_CODE);
            }

            return;
        }

        startCall(PanoApplication.APP_TOKEN, channelId, userId, userName);
    }

    private void startCall(String token, String channelId, String userId, String userName) {
        Intent intent = new Intent();
        intent.putExtra(kAppToken, token);
        intent.putExtra(kChannelID, channelId);
        intent.putExtra(kUserID, Long.parseLong(userId));
        intent.putExtra(kUserName, userName);
        intent.putExtra(kMode1v1, mMode1v1.isChecked());
        intent.setClass(this, CallActivity.class);
        startActivity(intent);
    }
}