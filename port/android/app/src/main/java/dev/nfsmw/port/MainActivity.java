package dev.nfsmw.port;

import android.app.Activity;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.view.Gravity;
import android.content.res.AssetManager;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.TextView;

public class MainActivity extends Activity implements SurfaceHolder.Callback {
    static { System.loadLibrary("nfsmw_probe"); }

    @Override
    protected void onCreate(Bundle state) {
        super.onCreate(state);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
                | WindowManager.LayoutParams.FLAG_FULLSCREEN);

        SurfaceView sv = new SurfaceView(this);
        sv.getHolder().setFormat(PixelFormat.RGBA_8888);
        sv.getHolder().addCallback(this);
        sv.setOnTouchListener(new android.view.View.OnTouchListener() {
            @Override
            public boolean onTouch(android.view.View v, MotionEvent e) {
                nativeOnTouch(e.getX(), e.getY(), e.getAction());
                return true;
            }
        });

        TextView tv = new TextView(this);
        tv.setTextColor(Color.WHITE);
        tv.setTextSize(16f);
        tv.setPadding(24, 16, 24, 16);
        tv.setBackgroundColor(0x99000000);
        nativeSetAssets(getAssets());
        tv.setText(probe());

        FrameLayout root = new FrameLayout(this);
        root.addView(sv, new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT));
        FrameLayout.LayoutParams tp = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT);
        tp.gravity = Gravity.BOTTOM | Gravity.END;
        tp.setMargins(16, 16, 16, 16);
        root.addView(tv, tp);
        setContentView(root);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        nativeSetSurface(holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        nativeSetSize(w, h);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        nativeSetSurface(null);
    }

    @Override
    protected void onPause() {
        nativeSetPaused(true);
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        nativeSetPaused(false);
    }

    private native String probe();
    private native void nativeSetAssets(AssetManager assets);
    private native void nativeSetSurface(Surface surface);
    private native void nativeSetSize(int width, int height);
    private native void nativeSetPaused(boolean paused);
    private native void nativeOnTouch(float x, float y, int action);
}
