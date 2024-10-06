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

import org.libsdl.app.SDLActivity;

public class JkrGUIActivity extends SDLActivity implements SensorEventListener{
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