package org.qtproject.example;

import org.qtproject.qt.android.bindings.QtActivity;
import android.app.Activity;
import android.app.PictureInPictureParams;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;

public class MyActivity extends QtActivity{
    private static final String TAG = "MyActivity";
    private PowerManager.WakeLock wakeLock;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "custom activity created");

        PowerManager powerManager = (PowerManager) getSystemService(POWER_SERVICE);
        wakeLock = powerManager.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "MyApp::WakeLock");
        wakeLock.acquire();
    }

    @Override
    protected void onUserLeaveHint() {
        super.onUserLeaveHint();
        Log.d(TAG, "PIP 모드로 전환됩니다.");

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            PictureInPictureParams.Builder pipBuilder = new PictureInPictureParams.Builder();
            enterPictureInPictureMode(pipBuilder.build());
        }
    }

    @Override
    protected void onDestroy(){
        super.onDestroy();
        if(wakeLock != null && wakeLock.isHeld())
            wakeLock.release();
    }
}
