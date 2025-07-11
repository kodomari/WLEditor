# WLEditor 開発環境セットアップガイド

このドキュメントは、新しいセッションや開発環境でWLEditorの開発を継続するためのセットアップ手順をまとめています。

## 🛠️ 開発環境要件

### 基本環境
- **OS**: Linux (Ubuntu 22.04+ 推奨)
- **Git**: バージョン管理
- **GitHub CLI**: `gh` コマンド (リリース管理用)

### Qt開発環境

#### Linux開発用
```bash
# Qt 6.5.3+ for Linux
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential
```

#### Android開発用（重要）
- **Qt 6.5.3**: ⚠️ Qt 6.9.1は使用禁止（NDK互換性問題）
- **Android Studio**: Android SDK/NDK管理用
- **Android NDK**: 25.2.9519653
- **Android SDK**: Build Tools 34.0.0

## 📁 プロジェクト構造理解

```bash
# プロジェクトクローン
git clone https://github.com/kodomari/WLEditor.git
cd WLEditor

# 重要なファイル確認
ls docs/                    # ドキュメント
ls src/core/               # 共通エンジン
ls src/desktop/            # Linux版UI
ls src/mobile/             # Android版UI
ls android/                # Android設定
```

### 重要ファイル
- **CMakeLists.txt**: デスクトップビルド用
- **wledit.pro**: Android ビルド用（推奨）
- **mobile.qrc**: QMLリソース
- **android/AndroidManifest.xml**: Android設定

## 🔧 環境変数設定

### Android開発時
```bash
# ~/.bashrc または ~/.zshrc に追加
export ANDROID_SDK_ROOT=$HOME/Android/Sdk
export ANDROID_NDK_ROOT=$HOME/Android/Sdk/ndk/25.2.9519653
export PATH=$PATH:$ANDROID_SDK_ROOT/tools
export PATH=$PATH:$ANDROID_SDK_ROOT/platform-tools
```

### Qt環境（Android用）
```bash
# Qt 6.5.3 Android パス
export QT_ANDROID=$HOME/Qt/6.5.3/android_arm64_v8a
export PATH=$PATH:$QT_ANDROID/bin
```

## 🚀 クイックスタート

### 1. Linux版ビルド
```bash
mkdir build-desktop && cd build-desktop
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./wledit
```

### 2. Android版ビルド（推奨手順）
```bash
# 環境変数確認
echo $ANDROID_SDK_ROOT
echo $ANDROID_NDK_ROOT

# qmakeビルド（CMakeより安定）
$QT_ANDROID/bin/qmake wledit.pro -spec android-clang CONFIG+=release
make -j$(nproc)
make apk

# APK署名
$ANDROID_SDK_ROOT/build-tools/34.0.0/apksigner sign \
  --ks wleditor-release-key.keystore \
  --ks-pass pass:wleditor123 \
  android-build/build/outputs/apk/debug/android-build-debug.apk

# インストール
adb uninstall com.wleditor.app  # 既存アプリ削除
adb install android-build-debug.apk
```

## 🔍 トラブルシューティング

### よくある問題

#### Qt 6.9.1を使用してしまった場合
```
UnsatisfiedLinkError: cannot locate symbol "_ZTVNSt6__ndk13pmr25monotonic_buffer_resourceE"
```
**解決**: Qt 6.5.3を使用してください

#### Android SDK/NDKが見つからない
```
Could not locate Android SDK build tools
```
**解決**: 環境変数を正しく設定してください

#### APK署名エラー
```
INSTALL_FAILED_UPDATE_INCOMPATIBLE
```
**解決**: 既存アプリをアンインストールしてからインストール

### デバッグコマンド
```bash
# Android デバイス確認
adb devices

# ログ確認
adb logcat | grep WLEditor

# Qt環境確認
qmake --version
```

## 📋 開発ワークフロー

### 1. 機能開発
```bash
# 新しいブランチ作成
git checkout -b feature/new-function

# 開発・テスト
# ...

# コミット
git add .
git commit -m "新機能追加: ..."
```

### 2. Android テスト
```bash
# APKビルド
qmake wledit.pro -spec android-clang CONFIG+=release
make apk

# デバイステスト
adb install -r app.apk
# 実機で動作確認
```

### 3. リリース準備
```bash
# バージョンタグ
git tag -a v1.x.x -m "Version 1.x.x"

# GitHub Release
gh release create v1.x.x --title "WLEditor v1.x.x"
gh release upload v1.x.x wleditor-linux-x64
gh release upload v1.x.x wleditor-android.apk
```

## 📚 必読ドキュメント

### 開発開始前に読むべき文書
1. **PROJECT_REQUIREMENTS.md**: プロジェクト全体像
2. **ANDROID_BUILD_SUCCESS.md**: Android ビルド詳細
3. **KEYMAP.md**: WordStar キーバインド仕様

### コード理解
- **src/core/TextEngine.h**: メイン機能インターフェース
- **src/core/TextEngine.cpp**: WordStar機能実装
- **src/mobile/main.qml**: Android UI実装

## ⚠️ 重要な注意事項

### 絶対に避けるべきこと
1. **Qt 6.9.1の使用**: Android でクラッシュします
2. **CMakeでのAndroidビルド**: qmakeより複雑で不安定
3. **署名なしAPK**: インストールできません

### 推奨事項
1. **実機テスト**: Android版は必ず実機で確認
2. **バックアップ**: keystoreファイルの安全な保管
3. **ドキュメント更新**: 変更時は関連ドキュメントも更新

## 🔗 関連リンク

- **GitHub Repository**: https://github.com/kodomari/WLEditor
- **Qt 6.5.3 Downloads**: https://download.qt.io/archive/qt/6.5/6.5.3/
- **Android NDK Downloads**: https://developer.android.com/ndk/downloads
- **WordStar Commands**: http://www.wordstar.org/wordstar/commands/

---

**このドキュメントを参照して、効率的な開発環境を構築してください。特にAndroid開発では、Qt 6.5.3とqmakeの使用が成功の鍵です。**