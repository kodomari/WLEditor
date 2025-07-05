#!/bin/bash
set -e

# WLEditor Android APK Quick Build Script
# 使用方法: ./scripts/build-android-quick.sh

echo "🚀 WLEditor Android APK Quick Build Starting..."

# 環境変数設定
export ANDROID_SDK_ROOT=$HOME/Android/Sdk
export ANDROID_NDK_HOME=$ANDROID_SDK_ROOT/ndk/25.2.9519653
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64

# 環境確認
echo "📋 Environment Check:"
echo "SDK: $ANDROID_SDK_ROOT"
echo "NDK: $ANDROID_NDK_HOME"
echo "Java: $JAVA_HOME"

# 必要なツールの存在確認
if [[ ! -f "$ANDROID_SDK_ROOT/build-tools/34.0.0/aapt" ]]; then
    echo "❌ Error: Android build tools not found!"
    echo "Please install Android SDK build-tools 34.0.0"
    exit 1
fi

if [[ ! -f "$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake" ]]; then
    echo "❌ Error: Android NDK not found!"
    echo "Please install Android NDK 25.2.9519653"
    exit 1
fi

# バージョン取得
VERSION=$(grep "project.*VERSION" CMakeLists.txt | sed 's/.*VERSION \([0-9.]*\).*/\1/')
APK_NAME="wleditor-v${VERSION}-android-arm64.apk"

echo "🔨 Building WLEditor Android v${VERSION}..."

# 1. Android ネイティブライブラリをビルド
echo "⚙️  Step 1/3: Building native library..."
rm -rf build-android && mkdir build-android && cd build-android

cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=android-23 \
      -DCMAKE_BUILD_TYPE=Release \
      -DANDROID=ON .. > /dev/null

make -j$(nproc) > /dev/null
echo "✅ Native library built successfully"

# 2. ネイティブライブラリを配置
mkdir -p ../android/src/main/jniLibs/arm64-v8a
cp libwledit.so ../android/src/main/jniLibs/arm64-v8a/

# 3. Android APK をビルド
echo "📦 Step 2/3: Building Android APK..."
cd ..

# R.java 生成
$ANDROID_SDK_ROOT/build-tools/34.0.0/aapt package -f -m \
  -J android/src/main/java \
  -M android/AndroidManifest.xml \
  -S android/res \
  -I $ANDROID_SDK_ROOT/platforms/android-34/android.jar > /dev/null

# Java コンパイル
mkdir -p android/bin/classes
javac -d android/bin/classes \
  -sourcepath android/src/main/java \
  -cp $ANDROID_SDK_ROOT/platforms/android-34/android.jar \
  android/src/main/java/com/wleditor/app/*.java 2>/dev/null

# DEX 変換
$ANDROID_SDK_ROOT/build-tools/34.0.0/d8 \
  --output android/bin/ \
  android/bin/classes/com/wleditor/app/*.class 2>/dev/null

# APK パッケージング
$ANDROID_SDK_ROOT/build-tools/34.0.0/aapt package -f \
  -M android/AndroidManifest.xml \
  -S android/res \
  -I $ANDROID_SDK_ROOT/platforms/android-34/android.jar \
  -F app-unsigned.apk android/bin/ > /dev/null

# ネイティブライブラリ追加
cd android/bin && mkdir -p lib/arm64-v8a
cp ../src/main/jniLibs/arm64-v8a/libwledit.so lib/arm64-v8a/
zip -r ../../app-unsigned.apk lib/ > /dev/null
cd ../..

echo "🔧 Step 3/3: Optimizing APK..."
# APK 最適化
$ANDROID_SDK_ROOT/build-tools/34.0.0/zipalign -v 4 \
  app-unsigned.apk $APK_NAME > /dev/null

# クリーンアップ
rm -f app-unsigned.apk
rm -rf android/bin

# 結果表示
APK_SIZE=$(du -h $APK_NAME | cut -f1)
echo ""
echo "🎉 Build completed successfully!"
echo "📱 APK: $APK_NAME ($APK_SIZE)"
echo ""
echo "📋 APK Contents:"
unzip -l $APK_NAME | grep -E "(libwledit.so|ic_launcher.png|AndroidManifest.xml)" | head -5
echo ""
echo "🚀 Ready for installation: adb install $APK_NAME"