#!/bin/bash
# WLEditor アンインストールスクリプト

echo "🗑️  WLEditor をアンインストール中..."

# バイナリを削除
sudo rm -f /usr/local/bin/wledit

# アイコンを削除
sudo rm -f /usr/share/icons/hicolor/scalable/apps/wledit.svg
sudo rm -f /usr/share/pixmaps/wledit.png
for size in 16 32 48 64 128 256 512; do
    sudo rm -f "/usr/share/icons/hicolor/${size}x${size}/apps/wledit.png"
done

# .desktopファイルを削除
sudo rm -f /usr/share/applications/wledit.desktop

# システムデータベースを更新
sudo update-desktop-database /usr/share/applications 2>/dev/null || true
sudo gtk-update-icon-cache /usr/share/icons/hicolor 2>/dev/null || true

echo "✅ アンインストール完了"
