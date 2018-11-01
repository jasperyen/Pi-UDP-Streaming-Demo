package io.ccyen.pistreaming;

import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Build;
import android.support.design.widget.FloatingActionButton;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Toast;

import java.io.ByteArrayInputStream;
import java.net.SocketException;
import java.nio.ByteBuffer;

public class MainActivity extends Activity {

    private final String[] settingOption = {"bind port", "scale type"};
    private final String[] scaleType = {"crop", "center", "fit", "VR"};

    private FloatingActionButton settingBtn;
    private View view;
    private ImageView imageView, imageViewLeft, imageViewRight;
    private LinearLayout VRLayout;
    private ImageViewHandler viewHandler;
    private UDPStreamingReceiver receiver;
    private boolean isStreaming = false;
    private boolean isVRMode = false;
    private int defaultPort = 17788;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        view = findViewById(android.R.id.content);

        settingBtn = findViewById(R.id.settingButton);
        imageView = findViewById(R.id.imageView);
        imageViewLeft = findViewById(R.id.imageViewLeft);
        imageViewRight = findViewById(R.id.imageViewRight);
        VRLayout = findViewById(R.id.VRLayout);

        imageView.setScaleType(ImageView.ScaleType. CENTER_CROP);
        //imageView.setScaleType(ImageView.ScaleType. FIT_CENTER);
        //imageView.setScaleType(ImageView.ScaleType. FIT_XY);

        imageViewLeft.setScaleType(ImageView.ScaleType. CENTER_CROP);
        imageViewRight.setScaleType(ImageView.ScaleType. CENTER_CROP);

        imageView.bringToFront();

        settingBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showSettingDialog();
            }
        });

        viewHandler = new ImageViewHandler();
        viewHandler.start();
    }

    @Override
    protected void onStart() {
        super.onStart();
        //Toast.makeText(this, "onStart", Toast.LENGTH_LONG).show();

        if (!isStreaming)
            startStreaming(defaultPort);
    }

    @Override
    protected void onStop() {
        super.onStop();
        //Toast.makeText(this, "onStop", Toast.LENGTH_LONG).show();

        if (isStreaming) {
            try {
                receiver.close();
            } catch (InterruptedException e) {}
            isStreaming = false;
        }
    }

    private void showSettingDialog () {
        new AlertDialog.Builder(MainActivity.this).setItems(settingOption, new DialogInterface.OnClickListener(){
            public void onClick(DialogInterface dialog, int which) {
                switch (which) {
                    case 0 :
                        showBindPortDialog();
                        break;
                    case 1 :
                        showScaleDialog();
                        break;
                }
            }
        }).show();
    }

    private void showBindPortDialog () {
        final EditText editText = new EditText(this);
        Dialog dialog = new AlertDialog.Builder(this)
                .setIcon(android.R.drawable.ic_dialog_info)
                .setMessage("port")
                .setView(editText)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        changeBindPort(editText.getText().toString());
                    }
                }).create();
        dialog.show();
    }

    private void showScaleDialog () {
        new AlertDialog.Builder(MainActivity.this).setItems(scaleType, new DialogInterface.OnClickListener(){
            public void onClick(DialogInterface dialog, int which) {
                changeScaleType(which);
            }
        }).create().show();
    }

    private void changeScaleType (int type) {

        switch (type) {
            case 0 :
            case 1 :
            case 2 :
                if (isVRMode) {
                    imageView.bringToFront();
                    imageViewRight.setAlpha(0f);
                    imageViewLeft.setAlpha(0f);
                }
                isVRMode = false;
                break;
            case 3 :
                if (!isVRMode) {
                    VRLayout.bringToFront();
                    imageViewRight.setAlpha(1f);
                    imageViewLeft.setAlpha(1f);
                }
                isVRMode = true;
                break;
        }

        switch (type) {
            case 0 :
                imageView.setScaleType(ImageView.ScaleType. CENTER_CROP);
                break;
            case 1 :
                imageView.setScaleType(ImageView.ScaleType. FIT_CENTER);
                break;
            case 2 :
                imageView.setScaleType(ImageView.ScaleType. FIT_XY);
                break;
        }
    }

    private void changeBindPort(String input) {
        if (input.equals(""))
            return;

        try {
            int port = Integer.parseInt(input);
            if (!isStreaming)
                startStreaming(port);
            else {
                receiver.close();
                receiver = new UDPStreamingReceiver(port);
                receiver.start();
            }

            isStreaming = true;
            Log.i("changeBindPort", "bind port on " + port);
            Toast.makeText(view.getContext(),
                    "bind port on " + port, Toast.LENGTH_LONG).show();
        } catch (Exception e) {
            isStreaming = false;
            Log.w("changeBindPort", e.toString());
            Toast.makeText(view.getContext(),
                    "change port failed in exception : " + e.toString(), Toast.LENGTH_LONG).show();
        }
    }

    private void startStreaming(int port) {
        try {
            receiver = new UDPStreamingReceiver(port);
            receiver.start();

            isStreaming = true;
            Log.i("startStreaming", "bind port on " + port);
            Toast.makeText(view.getContext(), "bind port on " + port, Toast.LENGTH_LONG).show();
        } catch (SocketException e) {
            isStreaming = false;
            Log.w("startStreaming", e.toString());
            Toast.makeText(view.getContext(),
                    "bind port failed in exception : " + e.toString(), Toast.LENGTH_LONG).show();
        }
    }

    public class ImageViewHandler extends Thread {
        private ByteBuffer buffer = null;
        private boolean keepRunning = true;
        private Bitmap bitmap = null;

        @Override
        public void run() {
            while (keepRunning) {
                if (receiver == null || (buffer = receiver.getData()) == null) {
                    try {
                        Thread.sleep(30);
                    } catch (InterruptedException e) {}
                    continue;
                }

                bitmap = BitmapFactory.decodeStream(new ByteArrayInputStream(buffer.array()));
                if (!isVRMode)
                    runOnUiThread(new BitmapUpdater(imageView, bitmap));
                else{
                    runOnUiThread(new BitmapUpdater(imageViewLeft, bitmap));
                    runOnUiThread(new BitmapUpdater(imageViewRight, bitmap));
                }
            }
        }
    }

    public class BitmapUpdater implements Runnable {
        private final ImageView imageView;
        private final Bitmap bitmap;

        public BitmapUpdater(ImageView imageView, Bitmap bitmap) {
            this.imageView = imageView;
            this.bitmap = bitmap;
        }

        @Override
        public void run() {
            imageView.setImageBitmap(bitmap);
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus && Build.VERSION.SDK_INT >= 19) {
            View decorView = getWindow().getDecorView();
            decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
        }
    }
}
