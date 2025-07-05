# Android Build Guide - Lessons Learned

このドキュメントは WLEditor の Android 移植過程で学んだ重要な知見とノウハウをまとめたものです。

## 概要

Ubuntu 上で Qt6 ベースの C++ アプリケーションを Android APK に移植する際の実践的な解決策と回避策を記録しています。

## 主要な技術的課題と解決策

### 1. Qt6 for Android の依存関係問題

**問題:**
- Ubuntu パッケージの Qt6 は Linux 専用でAndroid クロスコンパイルに対応していない
- `apt install qt6-tools-dev` では `androiddeployqt6` ツールが含まれない
- 公式 Qt インストーラーの URL が変更される頻度が高い

**解決策:**
Qt6 を完全に回避し、Android NDK を直接使用する独自のビルドシステムを構築

```cmake
# CMakeLists.txt の重要な部分
if(ANDROID)
    # Qt6 を使用せずに Android ネイティブライブラリを作成
    add_library(wledit SHARED ${SOURCES} ${HEADERS})
    find_library(log-lib log)
    find_library(android-lib android)
    target_link_libraries(wledit ${log-lib} ${android-lib})
    target_compile_definitions(wledit PRIVATE ANDROID_BUILD)
else()
    # Linux 版は従来通り Qt6 を使用
    add_executable(wledit ${SOURCES} ${HEADERS})
    target_link_libraries(wledit Qt6::Core Qt6::Widgets)
endif()
```

### 2. Android 開発環境のセットアップ

**必要なコンポーネント:**
```bash
# 必須環境変数
export ANDROID_SDK_ROOT=$HOME/Android/Sdk
export ANDROID_NDK_HOME=$ANDROID_SDK_ROOT/ndk/25.2.9519653
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64
```

**重要な SDK バージョン:**
- Android NDK: 25.2.9519653 (安定版)
- Build Tools: 34.0.0
- Platform: android-34 (API Level 34)
- Java: OpenJDK 17

### 3. クロスプラットフォーム対応の実装戦略

**アーキテクチャ:**
```
WLEditor/
├── src/
│   ├── main.cpp          # Linux 版メイン
│   ├── MainWindow.cpp    # Qt6 ベース UI (Linux 版)
│   └── android_main.cpp  # Android ネイティブ実装
├── android/
│   ├── AndroidManifest.xml
│   ├── src/main/java/    # Java Activity
│   └── res/              # Android リソース
└── CMakeLists.txt        # 統合ビルド設定
```

**重要な実装方針:**
- コア機能を C++ で共通化
- プラットフォーム固有部分を条件分岐で分離
- Android では Qt6 を使用せず JNI で UI 連携

### 4. APK ビルドプロセスの手動化

**Gradle が利用できない場合の回避策:**

```bash
# 1. リソース処理とR.java生成
$ANDROID_SDK_ROOT/build-tools/34.0.0/aapt package -f -m \
  -J android/src/main/java \
  -M android/AndroidManifest.xml \
  -S android/res \
  -I $ANDROID_SDK_ROOT/platforms/android-34/android.jar

# 2. Java コンパイル
javac -d android/bin/classes \
  -sourcepath android/src/main/java \
  -cp $ANDROID_SDK_ROOT/platforms/android-34/android.jar \
  android/src/main/java/com/wleditor/app/*.java

# 3. DEX 変換
$ANDROID_SDK_ROOT/build-tools/34.0.0/d8 \
  --output android/bin/ \
  android/bin/classes/com/wleditor/app/*.class

# 4. APK パッケージング
$ANDROID_SDK_ROOT/build-tools/34.0.0/aapt package -f \
  -M android/AndroidManifest.xml \
  -S android/res \
  -I $ANDROID_SDK_ROOT/platforms/android-34/android.jar \
  -F app-unsigned.apk android/bin/

# 5. ネイティブライブラリ追加
cd android/bin && mkdir -p lib/arm64-v8a
cp ../src/main/jniLibs/arm64-v8a/libwledit.so lib/arm64-v8a/
zip -r app-unsigned.apk lib/

# 6. APK 最適化
$ANDROID_SDK_ROOT/build-tools/34.0.0/zipalign -v 4 \
  app-unsigned.apk wleditor-v1.2.0-android-arm64.apk
```

### 5. AndroidManifest.xml の重要な設定

```xml
<!-- 最小限の設定例 -->
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.wleditor.app"
    android:versionCode="2"
    android:versionName="1.2.0">
    
    <uses-sdk android:minSdkVersion="23" android:targetSdkVersion="34"/>
    
    <!-- ファイルアクセス権限 -->
    <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
    <uses-permission android:name="android.permission.READ_MEDIA_DOCUMENTS"/>
    
    <application android:label="WLEditor"
                android:icon="@mipmap/ic_launcher"
                android:theme="@android:style/Theme.Material">
        
        <activity android:name=".MainActivity"
                 android:exported="true"
                 android:launchMode="singleTop">
            <intent-filter>
                <action android:name="android.intent.action.MAIN"/>
                <category android:name="android.intent.category.LAUNCHER"/>
            </intent-filter>
            
            <!-- テキストファイル関連付け -->
            <intent-filter>
                <action android:name="android.intent.action.VIEW"/>
                <category android:name="android.intent.category.DEFAULT"/>
                <data android:mimeType="text/plain"/>
            </intent-filter>
            
            <meta-data android:name="android.app.lib_name" android:value="wledit"/>
        </activity>
    </application>
</manifest>
```

### 6. JNI インターフェースの実装

**MainActivity.java:**
```java
public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        System.loadLibrary("wledit");
        nativeInit();
    }
    
    // ネイティブメソッド宣言
    public static native void nativeInit();
    public static native void insertText(String text);
    public static native String getText();
    public static native void deleteChar();
}
```

**android_main.cpp:**
```cpp
extern "C" {
JNIEXPORT void JNICALL
Java_com_wleditor_app_MainActivity_nativeInit(JNIEnv *env, jclass clazz) {
    LOGI("WLEditor Android Native Init");
    // エディタ初期化処理
}
// その他の JNI 関数実装
}
```

## 既知の問題と回避策

### 1. Java 17 と Android SDK の互換性

**問題:** 新しい Java バージョンで bootclasspath オプションが非対応
**解決策:** `-bootclasspath` の代わりに `-cp` を使用

### 2. Gradle Wrapper の動作不良

**問題:** 環境によっては Gradle が正常に動作しない
**解決策:** Android SDK ツールを直接使用した手動ビルド

### 3. アイコンリソースの配置

**重要:** Android は解像度別アイコンが必須
```
android/res/
├── mipmap-mdpi/ic_launcher.png     (48x48)
├── mipmap-hdpi/ic_launcher.png     (72x72)
├── mipmap-xhdpi/ic_launcher.png    (96x96)
├── mipmap-xxhdpi/ic_launcher.png   (144x144)
└── mipmap-xxxhdpi/ic_launcher.png  (192x192)
```

## ファイル構造

```
WLEditor/
├── android/
│   ├── AndroidManifest.xml
│   ├── build.gradle
│   ├── src/main/
│   │   ├── java/com/wleditor/app/
│   │   │   └── MainActivity.java
│   │   └── jniLibs/arm64-v8a/
│   │       └── libwledit.so
│   └── res/
│       ├── mipmap-*/ic_launcher.png
│       └── values/strings.xml
├── src/
│   └── android_main.cpp
├── build-android/
│   └── libwledit.so
├── gradle/wrapper/
├── build.gradle
├── settings.gradle
├── gradle.properties
└── gradlew
```

## 将来の改善案

1. **Qt6 for Android 対応**: 公式サポートが安定したら移行検討
2. **CI/CD 自動化**: GitHub Actions での APK 自動生成
3. **マルチアーキテクチャ**: ARM32, x86_64 対応
4. **署名済み APK**: リリース用の署名プロセス追加
5. **ProGuard 最適化**: APK サイズ削減

## トラブルシューティング

### ビルドエラーの一般的な原因
1. 環境変数の設定不備 (`ANDROID_SDK_ROOT`, `JAVA_HOME`)
2. SDK/NDK バージョンの不一致
3. 権限不足でのファイルアクセス
4. 重複する関数定義 (JNI)

### デバッグのヒント
- `adb logcat` でネイティブログを確認
- `unzip -l *.apk` で APK 内容を検証
- CMake の `-DANDROID=ON` フラグ確認

## 関連ドキュメント
- [ANDROID_SETUP.md](ANDROID_SETUP.md) - 開発環境構築
- [CMakeLists.txt](../CMakeLists.txt) - ビルド設定
- [MainActivity.java](../android/src/main/java/com/wleditor/app/MainActivity.java) - Java 実装