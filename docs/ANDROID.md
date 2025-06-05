# Android Development Guide

Building and deploying WLEditor for Android platforms.

## Prerequisites

### Development Environment

- **Qt 6.2+** with Android support
- **Android SDK** (API 23+)
- **Android NDK** (r21+)
- **Java JDK** (8 or 11)
- **CMake** 3.16+

### Platform Support

- **Minimum API**: 23 (Android 6.0)
- **Target API**: 34 (Android 14)
- **Architectures**: ARM64-v8a, ARMv7a

## Setup

### 1. Install Qt for Android

```bash
# Download Qt installer and select Android components:
# - Qt 6.5+ for Android
# - Android OpenSSL Toolkit
# - CMake
# - Ninja
2. Android SDK/NDK Setup
bash# Set environment variables (add to ~/.bashrc)
export ANDROID_HOME=$HOME/Android/Sdk
export ANDROID_NDK=$ANDROID_HOME/ndk/25.2.9519653
export PATH=$PATH:$ANDROID_HOME/platform-tools:$ANDROID_HOME/tools
3. Verify Installation
bash# Check tools
adb version
$ANDROID_NDK/ndk-build --version
Building for Android
Standard Build
bash# Clone repository
git clone https://github.com/yourusername/WLEditor.git
cd WLEditor

# Create Android build directory
mkdir build-android && cd build-android

# Configure for Android
cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=android-23 \
      -DQT_ANDROID_BUILD=ON \
      ..

# Build
make -j$(nproc)
Multi-Architecture Build
bash# Build for multiple architectures
architectures=("arm64-v8a" "armeabi-v7a")

for arch in "${architectures[@]}"; do
    echo "Building for $arch..."
    mkdir -p build-android-$arch
    cd build-android-$arch
    
    cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
          -DANDROID_ABI=$arch \
          -DANDROID_PLATFORM=android-23 \
          ..
    make -j$(nproc)
    cd ..
done
Android-Specific Configuration
CMakeLists.txt Additions
cmake# Android-specific settings
if(ANDROID)
    # Set package source directory
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    
    # Android-specific libraries
    find_library(log-lib log)
    target_link_libraries(wledit ${log-lib})
    
    # Install APK resources
    qt_add_resources(wledit "android_resources"
        BASE "${CMAKE_CURRENT_SOURCE_DIR}/android"
        FILES
            android/res/values/strings.xml
            android/res/drawable/icon.png
    )
endif()
Android Manifest
Create android/AndroidManifest.xml:
xml<?xml version="1.0"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.wleditor.app"
    android:versionCode="1"
    android:versionName="1.0.0">
    
    <uses-sdk android:minSdkVersion="23" android:targetSdkVersion="34"/>
    
    <!-- Permissions -->
    <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
    
    <application android:label="WLEditor"
                android:icon="@drawable/icon"
                android:theme="@style/AppTheme">
        
        <activity android:name="org.qtproject.qt.android.bindings.QtActivity"
                 android:exported="true"
                 android:launchMode="singleTop">
            <intent-filter>
                <action android:name="android.intent.action.MAIN"/>
                <category android:name="android.intent.category.LAUNCHER"/>
            </intent-filter>
            
            <!-- File associations -->
            <intent-filter>
                <action android:name="android.intent.action.VIEW"/>
                <category android:name="android.intent.category.DEFAULT"/>
                <data android:mimeType="text/plain"/>
            </intent-filter>
        </activity>
    </application>
</manifest>
Touch Interface Adaptations
Key Binding Adjustments
Android version includes touch-friendly adaptations:
cpp// Android-specific key handling
#ifdef Q_OS_ANDROID
    // Add virtual key overlay
    // Implement gesture navigation
    // Adapt clipboard for Android share
#endif
UI Modifications

Larger buttons for touch targets
Virtual WordStar key overlay for diamond navigation
Android-style menus and dialogs
Share integration with other apps

Deployment
APK Generation
bash# Use Qt Creator or command line
cd build-android
make apk

# Result: WLEditor.apk
Testing
bash# Install on device/emulator
adb install WLEditor.apk

# Run and debug
adb logcat | grep WLEditor
Release Build
bash# Release configuration
cmake -DCMAKE_BUILD_TYPE=Release \
      -DANDROID_SIGN_APK=ON \
      ..

# Sign APK (requires keystore)
jarsigner -keystore release.keystore WLEditor.apk wleditor
Performance Considerations
Memory Usage

Optimized for mobile: Reduced memory footprint
Efficient text rendering: Hardware-accelerated when available
Smart loading: Large files loaded progressively

Battery Optimization

Minimal background processing
Efficient redraw cycles
Native Android lifecycle handling

File Access
Storage Permissions
Android 6+ requires runtime permissions:
cpp// Request storage permissions
QAndroidJniObject::callStaticMethod<void>(
    "android/app/Activity",
    "requestPermissions",
    "([Ljava/lang/String;I)V",
    permissions.object(),
    REQUEST_CODE
);
File Picker Integration
Uses Android's native file picker:
cpp// Open Android file picker
QAndroidJniObject intent("android/content/Intent");
intent.callObjectMethod("setAction",
    "(Ljava/lang/String;)Landroid/content/Intent;",
    QAndroidJniObject::getStaticObjectField(
        "android/content/Intent",
        "ACTION_OPEN_DOCUMENT",
        "Ljava/lang/String;"
    ).object()
);
Distribution
Google Play Store
Prepare for Play Store submission:

Age rating: Content suitable for all ages
Category: Productivity
Features: Text editing, WordStar compatibility
Screenshots: Include key binding overlay
Description: Emphasize efficiency and classic computing

F-Droid
For open-source distribution:

License compatibility: GPL-3.0 ✓
Build reproducibility: CMake + Qt
No tracking: Privacy-friendly
Metadata: Include fastlane metadata

Troubleshooting
Build Issues
NDK version mismatch:
bash# Use specific NDK version
export ANDROID_NDK=$ANDROID_HOME/ndk/25.2.9519653
Qt libraries not found:
bash# Set Qt Android path
export Qt6_DIR=/path/to/qt6/android_arm64_v8a
Runtime Issues
Font rendering problems:

Include Noto Sans CJK in APK resources
Use Android system fonts as fallback

File access denied:

Ensure storage permissions granted
Use Android scoped storage for API 29+

Future Enhancements

Android Auto integration for voice input
Samsung DeX optimization for desktop mode
Foldable device support
Android 14 features integration

Resources

Qt for Android Documentation
Android NDK Guide
CMake Android Toolchain
