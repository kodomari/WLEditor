package com.wleditor.app;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Load the native library
        System.loadLibrary("wledit");
        
        // Initialize native application
        nativeInit();
    }
    
    // Native method declarations
    public static native void nativeInit();
    public static native void insertText(String text);
    public static native String getText();
    public static native void deleteChar();
}