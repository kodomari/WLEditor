# WLEditor プロジェクト要件定義

このドキュメントは、WLEditorプロジェクトの全体像と今後の開発に必要な情報をまとめています。

## 📋 プロジェクト概要

**WLEditor** は、クラシックなWordStarエディタのキーバインドを持つクロスプラットフォーム対応テキストエディタです。

- **言語**: C++ (Qt6ベース)
- **対応プラットフォーム**: Linux x64, Android ARM64
- **現在バージョン**: v1.3.0
- **ライセンス**: GPL-3.0
- **リポジトリ**: https://github.com/kodomari/WLEditor

## 🎯 主要機能

### WordStar キーバインド
- **Ctrl+K+K**: ブロックコピー機能（v1.3.0で修正済み）
- **Ctrl+S**: 左移動
- **Ctrl+D**: 右移動
- **Ctrl+E**: 上移動
- **Ctrl+X**: 下移動
- **その他**: 検索・置換、ファイル操作等のWordStar標準機能

### プラットフォーム対応
- **Linux**: Qt Widgets ベース
- **Android**: Qt Quick/QML ベース
- **共通エンジン**: `TextEngine` クラスで機能統一

## 🏗️ アーキテクチャ

```
WLEditor/
├── src/
│   ├── core/              # 共通機能
│   │   ├── TextEngine.h   # WordStar機能の中核
│   │   └── TextEngine.cpp
│   ├── desktop/           # Linux版UI (Qt Widgets)
│   │   ├── main.cpp
│   │   ├── MainWindow.h
│   │   └── MainWindow.cpp
│   └── mobile/            # Android版UI (Qt Quick/QML)
│       ├── main.cpp
│       └── main.qml
├── android/               # Android設定
│   └── AndroidManifest.xml
├── CMakeLists.txt         # CMakeビルド設定
├── wledit.pro            # qmakeビルド設定（Android推奨）
└── docs/                 # ドキュメント
    ├── ANDROID_BUILD_SUCCESS.md
    ├── KEYMAP.md
    └── BUILDING.md
```

## 🔧 ビルド環境

### Linux x64
- **Qt**: 6.5.3+ (Qt Widgets必須)
- **CMake**: 3.16+
- **コンパイラ**: GCC 13+ / Clang 14+
- **ビルドコマンド**:
  ```bash
  mkdir build && cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release
  make -j$(nproc)
  ```

### Android ARM64
- **Qt**: 6.5.3 (6.9.1は使用禁止 - NDK互換性問題)
- **Android NDK**: 25.2.9519653
- **Android SDK**: API 33, Build Tools 34.0.0
- **ビルドツール**: qmake推奨（CMakeより安定）
- **ビルドコマンド**:
  ```bash
  export ANDROID_NDK_ROOT=/path/to/ndk/25.2.9519653
  export ANDROID_SDK_ROOT=/path/to/sdk
  qmake wledit.pro -spec android-clang CONFIG+=release
  make -j$(nproc)
  make apk
  ```

## 📦 リリース管理

### 現在のリリース
- **v1.3.0**: WordStar ブロックコピー修正 + Android対応
- **アセット**:
  - `wleditor-v1.3.0-linux-x64`: Linux実行ファイル
  - `wleditor-v1.3.0-qt653-android-arm64.apk`: Android APK

### リリース手順
```bash
# 1. バージョンタグ作成
git tag -a v1.x.x -m "Version 1.x.x"

# 2. ビルド実行（Linux/Android）
# 3. GitHub Release作成
gh release create v1.x.x --title "WLEditor v1.x.x" --notes "..."

# 4. アセットアップロード
gh release upload v1.x.x wleditor-v1.x.x-linux-x64
gh release upload v1.x.x wleditor-v1.x.x-android-arm64.apk
```

## ⚠️ 重要な制約事項

### Android開発
1. **Qt 6.9.1は使用禁止**: NDK 25との互換性問題でクラッシュ
2. **Qt 6.5.3を使用**: 安定動作確認済み
3. **qmake推奨**: CMakeより設定が簡単で安定
4. **署名必須**: apksignerで署名、既存アプリは事前アンインストール

### 環境設定
- **Android SDK/NDK**: 正確なパス設定が必要
- **keystoreファイル**: `wleditor-release-key.keystore` (パスワード: wleditor123)

## 🐛 既知の問題

### 解決済み
- ✅ Android起動時クラッシュ（Qt 6.5.3で解決）
- ✅ WordStar Ctrl+K+Kブロックコピー（v1.3.0で修正）
- ✅ APK署名エラー（apksigner使用で解決）

### 未解決
- 現在なし

## 🚀 今後の開発方針

### 短期目標
1. **安定性向上**: 既存機能のテスト強化
2. **ドキュメント整備**: ユーザーマニュアル作成
3. **パフォーマンス最適化**: 大きなファイルの処理改善

### 中期目標
1. **プラットフォーム拡張**: Windows, macOS対応
2. **機能拡張**: シンタックスハイライト、プラグインシステム
3. **UI/UX改善**: モダンなテーマ、カスタマイズ機能

### 長期目標
1. **クラウド連携**: オンラインストレージ対応
2. **協調編集**: リアルタイム共同編集機能
3. **AI支援**: コード補完、自動修正機能

## 📚 参考ドキュメント

### プロジェクト内
- `docs/ANDROID_BUILD_SUCCESS.md`: Android ビルド手順
- `docs/KEYMAP.md`: WordStar キーマッピング
- `docs/BUILDING.md`: 一般的なビルド手順

### 外部リンク
- [Qt6 Documentation](https://doc.qt.io/qt-6/)
- [Android NDK Guide](https://developer.android.com/ndk/guides)
- [WordStar Reference](http://www.wordstar.org/wordstar/commands/commands.htm)

## 👥 開発チーム

- **メイン開発者**: kodomari
- **AI アシスタント**: Claude Code (Anthropic)
- **リポジトリ**: GitHub - kodomari/WLEditor

## 📞 サポート・問い合わせ

- **Issues**: https://github.com/kodomari/WLEditor/issues
- **Discussions**: GitHub Discussions
- **Email**: プロジェクトページ参照

---

**注意**: 新しいセッションでこのプロジェクトを扱う際は、このドキュメントと `docs/ANDROID_BUILD_SUCCESS.md` を必ず参照してください。特にAndroid開発では、Qt 6.5.3の使用とqmakeビルドシステムの選択が重要です。