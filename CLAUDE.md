# Claude Code セッション引き継ぎ情報

このファイルは、新しいClaude Codeセッションでこのプロジェクトを継続する際の重要な情報をまとめています。

## 📋 プロジェクト基本情報

- **プロジェクト名**: WLEditor
- **種類**: WordStar風テキストエディタ
- **言語**: C++ (Qt6)
- **現在バージョン**: v1.3.0
- **対応プラットフォーム**: Linux x64, Android ARM64

## 🎯 最新の状況（2025-07-11時点）

### ✅ 完了済み
1. **v1.3.0リリース**: WordStar Ctrl+K+Kブロックコピー修正
2. **Android版成功**: Qt 6.5.3でクラッシュ問題解決、実機動作確認済み
3. **GitHub Release更新**: 動作するAPKファイルをアップロード
4. **ドキュメント整備**: 包括的な開発ガイド作成

### 📦 現在のリリース状態
- **Linux版**: `wleditor-v1.3.0-linux-x64` (動作確認済み)
- **Android版**: `wleditor-v1.3.0-qt653-android-arm64.apk` (実機動作確認済み)

## ⚠️ 重要な技術的制約

### Android開発
- **Qt 6.9.1は絶対に使用禁止**: NDK 25との互換性問題でクラッシュ
- **Qt 6.5.3を使用**: 安定動作確認済み
- **qmakeを推奨**: CMakeより安定したビルド
- **環境変数**: ANDROID_SDK_ROOT, ANDROID_NDK_ROOT の正確な設定が必要

### ビルド設定
```bash
# Android環境変数（重要）
export ANDROID_NDK_ROOT=/home/kod/Android/Sdk/ndk/25.2.9519653
export ANDROID_SDK_ROOT=/home/kod/Android/Sdk

# Androidビルドコマンド
/home/kod/Qt/6.5.3/android_arm64_v8a/bin/qmake wledit.pro -spec android-clang CONFIG+=release
make -j$(nproc)
make apk
```

## 🏗️ プロジェクト構造

```
WLEditor/
├── src/
│   ├── core/              # 共通エンジン（重要）
│   │   ├── TextEngine.h   # WordStar機能インターフェース
│   │   └── TextEngine.cpp # 実装
│   ├── desktop/           # Linux版UI
│   └── mobile/            # Android版UI
├── android/               # Android設定
├── docs/                  # ドキュメント（必読）
│   ├── PROJECT_REQUIREMENTS.md     # プロジェクト要件
│   ├── DEVELOPMENT_SETUP.md        # 開発環境設定
│   └── ANDROID_BUILD_SUCCESS.md    # Android ビルド手順
├── CMakeLists.txt         # Linux ビルド用
├── wledit.pro            # Android ビルド用（推奨）
└── mobile.qrc            # QMLリソース
```

## 🔧 開発時の必須確認事項

### 新しいセッション開始時
1. **ドキュメント確認**: `docs/PROJECT_REQUIREMENTS.md` を必読
2. **Android環境**: Qt 6.5.3の使用確認
3. **ビルドシステム**: AndroidはqmakeのみBuild systems for Android: qmake only
4. **実機テスト**: Android版は必ず実機で確認

### よくある問題と解決策
```bash
# Qt 6.9.1を使った場合のクラッシュ
UnsatisfiedLinkError: cannot locate symbol "_ZTVNSt6__ndk13pmr25monotonic_buffer_resourceE"
# → Qt 6.5.3を使用

# APK署名エラー
INSTALL_FAILED_UPDATE_INCOMPATIBLE
# → 既存アプリをアンインストール: adb uninstall com.wleditor.app

# Android SDK not found
Could not locate Android SDK build tools
# → 環境変数を正しく設定
```

## 📋 開発継続時のToDoリスト

### 短期目標
- [ ] ユーザーマニュアル作成
- [ ] 大きなファイル処理の最適化
- [ ] 自動テストの整備

### 中期目標
- [ ] Windows/macOS対応
- [ ] シンタックスハイライト機能
- [ ] プラグインシステム

### 長期目標
- [ ] クラウド連携
- [ ] 協調編集機能
- [ ] AI支援機能

## 🔐 重要なファイル・認証情報

### 署名ファイル
- **keystore**: `wleditor-release-key.keystore`
- **パスワード**: wleditor123
- **場所**: プロジェクトルート

### GitHub設定
- **リポジトリ**: https://github.com/kodomari/WLEditor
- **ブランチ**: main
- **リリース**: v1.3.0が最新

## 📚 必読ドキュメント優先順位

1. **CLAUDE.md** (このファイル) - セッション引き継ぎ情報
2. **docs/PROJECT_REQUIREMENTS.md** - プロジェクト全体像
3. **docs/ANDROID_BUILD_SUCCESS.md** - Android開発手順
4. **docs/DEVELOPMENT_SETUP.md** - 開発環境設定

## 🚨 緊急時の対応

### Android ビルドが失敗する場合
1. Qt 6.5.3の使用確認
2. 環境変数の設定確認
3. qmakeの使用確認（CMakeではなく）
4. `docs/ANDROID_BUILD_SUCCESS.md` の手順に従う

### リリース時の手順
```bash
# 1. バージョンタグ
git tag -a v1.x.x -m "Version 1.x.x"

# 2. ビルド（Linux/Android）
# 3. GitHub Release
gh release create v1.x.x --title "WLEditor v1.x.x"
gh release upload v1.x.x wleditor-v1.x.x-linux-x64
gh release upload v1.x.x wleditor-v1.x.x-android.apk
```

---

**重要**: このプロジェクトでは、特にAndroid開発において技術的制約が多数あります。新しいセッションでは必ずこのファイルと関連ドキュメントを確認してから作業を開始してください。