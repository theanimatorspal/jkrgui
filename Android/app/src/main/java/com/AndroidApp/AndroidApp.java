package com.AndroidApp;

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

import org.libsdl.app.SDLActivity;

public class AndroidApp extends SDLActivity {
          public void copyFileOrDir(String path) {
                    AssetManager assetManager = this.getAssets();
                    String assets[] = null;
                    try {
                              assets = assetManager.list(path);
                              if (assets.length == 0) {
                                        copyFile(path);
                              } else {
                                        String fullPath = "/data/data/" + this.getPackageName() + "/" + path;
                                        File dir = new File(fullPath);
                                        if (!dir.exists())
                                                  dir.mkdir();
                                        for (int i = 0; i < assets.length; ++i) {
                                                  copyFileOrDir(path + "/" + assets[i]);
                                        }
                              }
                    } catch (IOException ex) {
                              Log.e("tag", "I/O Exception", ex);
                    }
          }

          private void copyFile(String filename) {
                    AssetManager assetManager = this.getAssets();

                    InputStream in = null;
                    OutputStream out = null;
                    try {
                              in = assetManager.open(filename);
                              String newFileName = "/data/data/" + this.getPackageName() + "/" + filename;
                              Log.e("KSAI::", filename + ", to : " + newFileName);
                              File FileExists = new File(newFileName);
                              if (!FileExists.exists()) {
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
                              }
                    } catch (Exception e) {
                              Log.e("tag", e.getMessage());
                    }

          }

          @Override
          public void onCreate(Bundle savedInstanceState) {
                    Log.e("MyApp", "Something peculiar happened in onCreate!");
                    copyFileOrDir("cache2");
                    copyFileOrDir("JkrGUIv2");
                    copyFileOrDir("res");
                    copyFileOrDir("src");
                    copyFileOrDir("app.lua");
                    copyFileOrDir("build.lua");
                    copyFileOrDir("font.ttf");
                    copyFileOrDir("jkrguiApp.so"); // TODO Don't add the lib prefix
                    String NewCurrentPath = "/data/data/" + this.getPackageName() + "/app.lua";
                    File F = new File(NewCurrentPath);
                    if (F.exists()) {
                              Log.e("KSAI::", "File Exits");
                    } else {
                              Log.e("KSAI::", "File Doesn't Exits;");
                    }
                    Log.e("MyApp", "Something peculiar happened in onCreate! 222");
                    super.onCreate(savedInstanceState);
          }
}