# Android Build Success Guide for WLEditor

このドキュメントはWLEditorのAndroid版ビルドで得られた重要な知見とノウハウをまとめています。

## 🎯 重要な発見事項

### Qt バージョンとAndroid NDKの互換性問題

**問題**: Qt 6.9.1 でビルドしたAndroidアプリが起動時にクラッシュする
- エラー: `UnsatisfiedLinkError: cannot locate symbol "_ZTVNSt6__ndk13pmr25monotonic_buffer_resourceE"`
- 原因: Qt 6.9.1のC++標準ライブラリシンボルとAndroid NDK 25の非互換性

**解決策**: Qt 6.5.3を使用することで互換性問題を回避
- ✅ Qt 6.5.3 + Android NDK 25の組み合わせは安定動作
- ❌ Qt 6.9.1は避けて、より安定したQt 6.5.3を使用する

### ビルドシステムの選択

**CMake vs qmake**:
- **CMake**: Qt 6.9.1で複雑な設定が必要、Android SDK/NDKパスの問題
- **qmake**: Qt 6.5.3で安定動作、シンプルな設定

**推奨**: Android ビルドには qmake を使用

## 🚀 成功したビルド手順

### 1. 環境設定

```bash
export ANDROID_NDK_ROOT=/home/kod/Android/Sdk/ndk/25.2.9519653
export ANDROID_SDK_ROOT=/home/kod/Android/Sdk
```

### 2. プロジェクトファイル (wledit.pro)

```pro
QT += core quick qml
CONFIG += c++17
TARGET = wledit
TEMPLATE = app

SOURCES += \
    src/core/TextEngine.cpp \
    src/mobile/main.cpp

HEADERS += \
    src/core/TextEngine.h

RESOURCES += mobile.qrc

# Android specific settings
android {
    CONFIG += qml_debug
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    ANDROID_MIN_SDK_VERSION = 26
    ANDROID_TARGET_SDK_VERSION = 33
    ANDROID_VERSION_NAME = 1.3.0
    ANDROID_VERSION_CODE = 13
}

# Desktop specific settings
!android {
    SOURCES += \
        src/desktop/main.cpp \
        src/desktop/MainWindow.cpp
    
    HEADERS += \
        src/desktop/MainWindow.h
    
    QT += widgets
}
```

### 3. ビルドコマンド

```bash
# qmakeでMakefile生成
/home/kod/Qt/6.5.3/android_arm64_v8a/bin/qmake wledit.pro -spec android-clang CONFIG+=release

# ビルド実行
make -j$(nproc)

# APK作成
make apk

# APK署名
/home/kod/Android/Sdk/build-tools/34.0.0/apksigner sign \
  --ks wleditor-release-key.keystore \
  --ks-pass pass:wleditor123 \
  wleditor-v1.3.0-qt653-android-arm64.apk
```

### 4. インストール

```bash
# 既存アプリのアンインストール（署名不一致回避）
adb uninstall com.wleditor.app

# 新しいAPKのインストール
adb install wleditor-v1.3.0-qt653-android-arm64.apk
```

## 🏗️ アーキテクチャ設計

### クロスプラットフォーム対応

```
WLEditor/
├── src/
│   ├── core/           # 共通エンジン
│   │   ├── TextEngine.h
│   │   └── TextEngine.cpp
│   ├── desktop/        # デスクトップ用（Qt Widgets）
│   │   ├── main.cpp
│   │   ├── MainWindow.h
│   │   └── MainWindow.cpp
│   └── mobile/         # モバイル用（Qt Quick/QML）
│       ├── main.cpp
│       └── main.qml
├── android/            # Android設定
│   └── AndroidManifest.xml
├── wledit.pro          # qmakeプロジェクトファイル
└── mobile.qrc          # QMLリソース
```

### コア機能の分離

- `TextEngine`: WordStar機能の中核実装
- プラットフォーム別UI: Desktop（Widgets）とMobile（QML）
- 共通API: Q_INVOKABLEメソッドでQMLから呼び出し可能

## 🛠️ トラブルシューティング

### よくある問題と解決策

1. **APK署名エラー**
   ```
   INSTALL_FAILED_UPDATE_INCOMPATIBLE
   ```
   **解決**: 既存アプリをアンインストールしてから再インストール

2. **Qt ライブラリ不整合**
   ```
   UnsatisfiedLinkError: cannot locate symbol
   ```
   **解決**: Qt 6.5.3を使用、CMakeではなくqmakeを使用

3. **Android SDK build-toolsが見つからない**
   ```
   Could not locate Android SDK build tools
   ```
   **解決**: ANDROID_SDK_ROOT環境変数を正しく設定

4. **QML import エラー**
   ```
   QML import could not be resolved: WLEditor
   ```
   **解決**: qmlRegisterTypeでC++クラスを正しく登録

## ⚙️ 推奨環境

- **Qt**: 6.5.3 (6.9.1は使用禁止)
- **Android NDK**: 25.2.9519653
- **Android SDK Build Tools**: 34.0.0
- **Android Target**: API 33 (Android 13)
- **Android Minimum**: API 26 (Android 8.0)

## ⚠️ 今後の注意点

1. **Qt バージョン**: 新しいQtバージョンは慎重にテスト
2. **NDK互換性**: QtとAndroid NDKの組み合わせを事前確認
3. **署名管理**: keystoreファイルの安全な管理
4. **テスト**: 実機での動作確認を必須とする

## 📈 成果

- ✅ Android版WLEditorが正常に動作
- ✅ Qt 6.5.3 + qmakeでの安定ビルド確立
- ✅ クロスプラットフォーム対応アーキテクチャ完成
- ✅ 起動時クラッシュ問題を完全解決

このドキュメントは今後のAndroidビルド作業の参考として活用してください。