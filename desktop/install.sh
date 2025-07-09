#!/bin/bash
set -e

echo "Installing WLEditor..."

# 実行ファイルをシステムにコピー
sudo cp ../build/wledit /usr/local/bin/wleditor
sudo chmod +x /usr/local/bin/wleditor

# アイコンをシステムにコピー
sudo mkdir -p /usr/local/share/icons
sudo cp ../icons/wledit.png /usr/local/share/icons/
sudo cp ../icons/wledit.svg /usr/local/share/icons/

# デスクトップエントリをシステムにコピー
sudo cp wledit.desktop /usr/share/applications/

# デスクトップデータベース更新
sudo update-desktop-database

echo "✅ WLEditor installed successfully!"
echo "📱 You can now find WLEditor in your application menu"
echo "🖥️  Or run 'wleditor' from terminal"
