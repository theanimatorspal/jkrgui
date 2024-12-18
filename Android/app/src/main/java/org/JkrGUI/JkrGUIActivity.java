package org.JkrGUI;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;

import android.content.res.AssetManager;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;

import android.Manifest;
import android.content.pm.PackageManager;
import android.content.Context;
import android.os.Bundle;
import android.widget.Toast;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;

import android.hardware.SensorManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.content.Context;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.media.Image;
import android.media.ImageReader;
import android.os.Bundle;
import android.util.Base64;
import android.util.Size;
import android.view.Surface;
import android.view.TextureView;
import android.view.ViewGroup;
import android.hardware.camera2.*;
import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;
import java.util.Collections;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;


import org.libsdl.app.SDLActivity;

public class JkrGUIActivity extends SDLActivity implements SensorEventListener {
    static {
        System.loadLibrary("jkrgui");
    }

    // Sensors
    //
    //
    //
    //
    private SensorManager mSensorManager;
    private Sensor mAccelerometer;
    private int mAccuracy;
    private float[] mAccelerometerData;

    public void InitiateSensors() {
        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            mAccelerometerData = event.values.clone(); // Store accelerometer data
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        if (sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            mAccuracy = accuracy; // Store accelerometer data
        }
    }

    // Call InitiateSensors before this
    public float[] GetAccelerometerData() {
        return mAccelerometerData;
    }

    public void ShowToast(String inString) {
        runOnUiThread(
                () -> {
                    Toast.makeText(this, inString,
                            Toast.LENGTH_SHORT).show();
                });
    }

    private HandlerThread mHandlerThread;
    private Handler mBackgroundHandler;
    private static final int CAMERA_PERMISSION_CODE = 100;
    private CameraDevice cameraDevice;
    private ImageReader imageReader;
    private CameraCaptureSession captureSession;
    private String cameraId;
    private byte[] lastCapturedImage;

    public byte[] InitializeCamera(String inString) {
        mHandlerThread = new HandlerThread("ImageReaderThread");
        mHandlerThread.start();
        mBackgroundHandler = new Handler(mHandlerThread.getLooper());
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, CAMERA_PERMISSION_CODE);
            return new byte[]{0};   
        } else {
            if (inString.equals("FRONT"))
            {
                openCamera(1);
            } else if (inString.equals("BACK"))
            {
                openCamera(0);
            }
            return new byte[]{1};
        }
    }

    public byte[] ShutdownCamera(String inString) {
        try {
            if (captureSession != null) {
                captureSession.close();
                captureSession = null;
            }
            if (cameraDevice != null) {
                cameraDevice.close();
                cameraDevice = null;
            }
            if (imageReader != null) {
                imageReader.close();
                imageReader = null;
            }
        } catch (Exception e) {
            Log.e("CameraActivity", "Error shutting down camera", e);
        }
        return new byte[]{1};
    }

    public byte[] GetCameraImage(String inString) {
        return lastCapturedImage;
    }

    private void openCamera(int cameraType) {
        CameraManager cameraManager = (CameraManager) getSystemService(CAMERA_SERVICE);
        try {
            for (String id : cameraManager.getCameraIdList()) {
                CameraCharacteristics characteristics = cameraManager.getCameraCharacteristics(id);
                int lensFacing = characteristics.get(CameraCharacteristics.LENS_FACING);
                if ((cameraType == 0 && lensFacing == CameraCharacteristics.LENS_FACING_BACK) ||
                    (cameraType == 1 && lensFacing == CameraCharacteristics.LENS_FACING_FRONT)) {
                    cameraId = id;
                    break;
                }
            }

            imageReader = ImageReader.newInstance(640, 480, android.graphics.ImageFormat.JPEG, 2);
            // imageReader.setOnImageAvailableListener(imageAvailableListener, null);
            imageReader.setOnImageAvailableListener(imageAvailableListener, mBackgroundHandler);

            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                    == PackageManager.PERMISSION_GRANTED) {
                cameraManager.openCamera(cameraId, stateCallback, mBackgroundHandler);
            }
        } catch (Exception e) {
            Log.e("CameraActivity", "Error opening camera", e);
        }
    }

    private final CameraDevice.StateCallback stateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(@NonNull CameraDevice camera) {
            cameraDevice = camera;
            startCaptureSession();
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice camera) {
            camera.close();
            cameraDevice = null;
        }

        @Override
        public void onError(@NonNull CameraDevice camera, int error) {
            camera.close();
            cameraDevice = null;
        }
    };

    private void startCaptureSession() {
        try {
            cameraDevice.createCaptureSession(
                Collections.singletonList(imageReader.getSurface()),
                new CameraCaptureSession.StateCallback() {
                    @Override
                    public void onConfigured(@NonNull CameraCaptureSession session) {
                        captureSession = session;
                        CaptureRequest.Builder requestBuilder = null;
                        try {
                            requestBuilder = cameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_STILL_CAPTURE);
                            requestBuilder.addTarget(imageReader.getSurface());
                            session.setRepeatingRequest(requestBuilder.build(), null, null);
                        } catch (CameraAccessException e) {
                            e.printStackTrace();
                        }
                    }

                    @Override
                    public void onConfigureFailed(@NonNull CameraCaptureSession session) {
                        Log.e("CameraActivity", "Capture session configuration failed");
                    }
                }, null
            );
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private final ImageReader.OnImageAvailableListener imageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            try (Image image = reader.acquireNextImage()) {
                if (image != null) {
                    lastCapturedImage = imageToCharArray(image);
                    Log.d("CameraActivity", "Captured Image Size: " + lastCapturedImage.length);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    };

    private byte[] imageToCharArray(Image image) {
        ByteBuffer buffer = image.getPlanes()[0].getBuffer();
        byte[] bytes = new byte[buffer.remaining()];
        buffer.get(bytes);
        return bytes;
    }

    public void copyFileOrDir(String path, String inPackageName) {
        AssetManager assetManager = this.getAssets();
        String assets[] = null;
        try {
            assets = assetManager.list(path);
            if (assets.length == 0) {
                copyFile(path, inPackageName);
            } else {
                String fullPath = "/data/data/" + inPackageName + "/" + path;
                File dir = new File(fullPath);
                if (!dir.exists())
                    dir.mkdir();
                for (int i = 0; i < assets.length; ++i) {
                    copyFileOrDir(path + "/" + assets[i], inPackageName);
                }
            }
        } catch (IOException ex) {
            Log.e("tag", "I/O Exception", ex);
        }
    }

    private void copyFile(String filename, String inPackageName) {
        AssetManager assetManager = this.getAssets();

        InputStream in = null;
        OutputStream out = null;
        try {
            in = assetManager.open(filename);
            String newFileName = "/data/data/" + inPackageName + "/" + filename;
            Log.e("KSAI::", filename + ", to : " + newFileName);
            out = new FileOutputStream(newFileName);

            byte[] buffer = new byte[1024];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }
            in.close();
            in = null;
            out.flush();
            out.close();
            out = null;
        } catch (Exception e) {
            Log.e("tag", e.getMessage());
        }

    }

    public native void InitJNI();

    public native void ChangeDirectory(String inString);
}