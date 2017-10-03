package ca.thekidd.supermariowar;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Toast;

import com.andretietz.android.controller.ActionView;
import com.andretietz.android.controller.InputView;
import com.andretietz.android.controller.StartSelectView;

import org.libsdl.app.SDLActivity;

public class MainActivity extends SDLActivity implements InputView.InputEventListener {

    public static native int updateJoysticks();
    private View controllerView;

    @SuppressLint("ClickableViewAccessibility")
    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);

        getLayoutInflater().inflate(R.layout.controller, mLayout, true);
        controllerView = mLayout.findViewById(R.id.controller);

        ActionView actionView = findViewById(R.id.viewAction);
        actionView.setOnButtonListener(this);
        InputView inputView = findViewById(R.id.viewDirection);
        inputView.setOnButtonListener(this);
        StartSelectView startSelectView = findViewById(R.id.viewStartSelect);
        startSelectView.setOnButtonListener(this);
        mLayout.removeView(controllerView);

        if(getPackageManager().hasSystemFeature("android.hardware.touchscreen"))
            Toast.makeText(this, "Tap the top half of the screen to display the on-screen controller.", Toast.LENGTH_LONG).show();
    }

    long lastTouch = -1;
    @Override
    public boolean dispatchTouchEvent(MotionEvent ev) {
        if(System.currentTimeMillis() <= lastTouch + 500)
            return super.dispatchTouchEvent(ev);

        lastTouch = System.currentTimeMillis();
        if (ev.getY() < getWindowManager().getDefaultDisplay().getHeight() / 2) {
            if(controllerView != null) {
                if(controllerView.getParent() != null)
                    mLayout.removeView(controllerView);
                else
                    mLayout.addView(controllerView);
            }
        }

        return super.dispatchTouchEvent(ev);
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        Log.d("Main", "Key down: " + event.getDeviceId() + " " + event.getKeyCode());
        int keyCode = event.getKeyCode();
        if(keyCode == KeyEvent.KEYCODE_ENTER || keyCode == KeyEvent.KEYCODE_DPAD_CENTER)
            keyCode = KeyEvent.KEYCODE_BUTTON_START;
        if(keyCode == KeyEvent.KEYCODE_BACK)
            keyCode = KeyEvent.KEYCODE_BUTTON_SELECT;
        if (SDLActivity.isDeviceSDLJoystick(event.getDeviceId())) {
            // Note that we process events with specific key codes here
            if (event.getAction() == KeyEvent.ACTION_DOWN) {
                SDLActivity.onNativePadDown(event.getDeviceId(), keyCode);
            } else if (event.getAction() == KeyEvent.ACTION_UP) {
                SDLActivity.onNativePadUp(event.getDeviceId(), keyCode);
            }
        }
        return super.dispatchKeyEvent(event);
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "SDL2",
                "SDL2_image",
                "SDL2_mixer",
                "enet",
                "yaml-cpp",
                "main"
        };
    }

    private final int[] smwControls = {
            KeyEvent.KEYCODE_DPAD_LEFT, KeyEvent.KEYCODE_DPAD_RIGHT, KeyEvent.KEYCODE_DPAD_UP, KeyEvent.KEYCODE_DPAD_DOWN, KeyEvent.KEYCODE_BUTTON_X, KeyEvent.KEYCODE_BUTTON_B, KeyEvent.KEYCODE_BUTTON_Y, KeyEvent.KEYCODE_BUTTON_A, KeyEvent.KEYCODE_BUTTON_START, KeyEvent.KEYCODE_BUTTON_SELECT
    };

    private boolean keyDown[] = new boolean[10];
    int LEFT = 0;
    int RIGHT = 1;
    int UP = 2;
    int DOWN = 3;
    int BUTTON_LEFT = 4;
    int BUTTON_RIGHT = 5;
    int BUTTON_UP = 6;
    int BUTTON_DOWN = 7;
    int BUTTON_START = 8;
    int BUTTON_SELECT = 9;

    @Override
    public void onInputEvent(View view, int buttons) {
        switch(view.getId()) {
            case R.id.viewDirection:
                keyEvent(LEFT, (16 & buttons) > 0);
                keyEvent(RIGHT, (1 & buttons) > 0);
                keyEvent(UP, (64 & buttons) > 0);
                keyEvent(DOWN, (4 & buttons) > 0);
                break;
            case R.id.viewAction:
                keyEvent(BUTTON_LEFT, (4 & buttons) > 0);
                keyEvent(BUTTON_RIGHT, (1 & buttons) > 0);
                keyEvent(BUTTON_UP, (8 & buttons) > 0);
                keyEvent(BUTTON_DOWN, (2 & buttons) > 0);
                break;
            case R.id.viewStartSelect:
                keyEvent(BUTTON_START, (1 & buttons) > 0);
                keyEvent(BUTTON_SELECT, (2 & buttons) > 0);
                break;
        }
    }

    private void keyEvent(int key, boolean pressed) {
        boolean prevState = keyDown[key];
        if(pressed != prevState) {
            keyDown[key] = pressed;
            if(pressed) {
                onNativePadDown(-2, smwControls[key]);
            } else {
                onNativePadUp(-2, smwControls[key]);
            }
        }
    }
}
