# Quick Rebuild Guide - Android APK

新しいセッションで Android APK を素早く再ビルドするためのクイックガイド。

## 前提条件確認

```bash
# 必要な環境変数を設定
export ANDROID_SDK_ROOT=$HOME/Android/Sdk
export ANDROID_NDK_HOME=$ANDROID_SDK_ROOT/ndk/25.2.9519653  
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64

# パスの確認
echo "SDK: $ANDROID_SDK_ROOT"
echo "NDK: $ANDROID_NDK_HOME" 
echo "Java: $JAVA_HOME"

# 必要なツールの存在確認
ls $ANDROID_SDK_ROOT/build-tools/34.0.0/aapt
ls $ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake
java -version
```

## クイック APK 再生成 (5分)

```bash
cd /path/to/WLEditor

# 1. Android ネイティブライブラリをビルド (2分)
rm -rf build-android && mkdir build-android && cd build-android
cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=android-23 \
      -DCMAKE_BUILD_TYPE=Release \
      -DANDROID=ON ..
make -j$(nproc)

# 2. ネイティブライブラリを配置
cp libwledit.so ../android/src/main/jniLibs/arm64-v8a/

# 3. Android APK をビルド (3分)
cd ..
$ANDROID_SDK_ROOT/build-tools/34.0.0/aapt package -f -m \
  -J android/src/main/java \
  -M android/AndroidManifest.xml \
  -S android/res \
  -I $ANDROID_SDK_ROOT/platforms/android-34/android.jar

mkdir -p android/bin/classes
javac -d android/bin/classes \
  -sourcepath android/src/main/java \
  -cp $ANDROID_SDK_ROOT/platforms/android-34/android.jar \
  android/src/main/java/com/wleditor/app/*.java

$ANDROID_SDK_ROOT/build-tools/34.0.0/d8 \
  --output android/bin/ \
  android/bin/classes/com/wleditor/app/*.class

$ANDROID_SDK_ROOT/build-tools/34.0.0/aapt package -f \
  -M android/AndroidManifest.xml \
  -S android/res \
  -I $ANDROID_SDK_ROOT/platforms/android-34/android.jar \
  -F app-unsigned.apk android/bin/

cd android/bin && mkdir -p lib/arm64-v8a
cp ../src/main/jniLibs/arm64-v8a/libwledit.so lib/arm64-v8a/
zip -r ../../app-unsigned.apk lib/
cd ../..

$ANDROID_SDK_ROOT/build-tools/34.0.0/zipalign -v 4 \
  app-unsigned.apk wleditor-v1.2.0-android-arm64.apk

echo "APK generated: $(ls -lh wleditor-v1.2.0-android-arm64.apk)"
```

## バージョン更新時の手順

```bash
# 1. バージョン番号を更新
sed -i 's/android:versionCode="[0-9]*"/android:versionCode="3"/' android/AndroidManifest.xml
sed -i 's/android:versionName="[^"]*"/android:versionName="1.3.0"/' android/AndroidManifest.xml
sed -i 's/VERSION 1\.[0-9]\.[0-9]/VERSION 1.3.0/' CMakeLists.txt

# 2. APK 名も更新
# 上記のクイックビルドスクリプトで "wleditor-v1.3.0-android-arm64.apk" に変更

# 3. Git コミットとリリース
git add -A
git commit -m "Android v1.3.0 release"
git tag "v1.3.0"
git push origin main --tags
```

## よくあるトラブルと解決

### エラー: "CMakeLists.txt が見つからない"
```bash
# 現在のディレクトリを確認
pwd
ls CMakeLists.txt  # これが表示されれば OK
```

### エラー: "Qt6_DIR が見つからない"
```bash
# Android ビルドでは Qt6 は不要。ANDROID フラグが正しく設定されているか確認
cmake -DANDROID=ON ..  # -DANDROID=ON が重要
```

### エラー: "aapt コマンドが見つからない"
```bash
# SDK パスの確認と再設定
ls $ANDROID_SDK_ROOT/build-tools/34.0.0/aapt
export PATH=$ANDROID_SDK_ROOT/build-tools/34.0.0:$PATH
```

### エラー: "Java コンパイルエラー"
```bash
# Java バージョン確認
java -version  # OpenJDK 17 であることを確認
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64
```

## デバッグとテスト

```bash
# APK 内容の確認
unzip -l wleditor-v1.2.0-android-arm64.apk

# ネイティブライブラリの確認
unzip -l wleditor-v1.2.0-android-arm64.apk | grep libwledit.so

# Android デバイスでのテスト (ADB 必要)
adb install wleditor-v1.2.0-android-arm64.apk
adb logcat | grep WLEditor  # ネイティブログの確認
```

## 自動化スクリプト

使用頻度が高い場合は、以下を `scripts/build-android-quick.sh` として保存:

```bash
#!/bin/bash
set -e

# 環境変数設定
export ANDROID_SDK_ROOT=$HOME/Android/Sdk
export ANDROID_NDK_HOME=$ANDROID_SDK_ROOT/ndk/25.2.9519653
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64

# バージョン取得 (CMakeLists.txt から)
VERSION=$(grep "project.*VERSION" CMakeLists.txt | sed 's/.*VERSION \([0-9.]*\).*/\1/')
APK_NAME="wleditor-v${VERSION}-android-arm64.apk"

echo "Building WLEditor Android v${VERSION}..."

# [上記のクイックビルドスクリプトを含める]

echo "✅ APK generated: $APK_NAME ($(du -h $APK_NAME | cut -f1))"
```

実行方法:
```bash
chmod +x scripts/build-android-quick.sh
./scripts/build-android-quick.sh
```