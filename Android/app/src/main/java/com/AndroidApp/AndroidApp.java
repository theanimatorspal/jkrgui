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

import org.JkrGUI.JkrGUIActivity;
import org.libsdl.app.SDLActivity;

public class AndroidApp extends JkrGUIActivity {
          @Override
          public void onCreate(Bundle savedInstanceState) {
                    copyFileOrDir("app.lua", this.getPackageName());
                    copyFileOrDir("font.ttf", this.getPackageName());
                    ChangeDirectory("/data/data/" + this.getPackageName() + "/");
                    super.onCreate(savedInstanceState);
                    RunMain();
          }
}